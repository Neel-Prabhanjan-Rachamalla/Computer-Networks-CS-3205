// NAME:R.Neel Prabhanjan
// Roll Number:CS20B056
// Course: CS3205 Jan. 2023 semester
// Lab number: 4
// Date of submission: 5-4-23
// I confirm that the source file is entirely written by me without
// resorting to any dishonest means.
// Website(s) that I used for basic socket programming code are:
// URL(s): GFG

#include <bits/stdc++.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <random>
#include <chrono>

//Namespaces.
using namespace std;
using namespace std::chrono;

#define BUFF_SIZE 1024

//Global variables inorder to access them in threads also.
int debug=0,port_num,PACKET_LENGTH,PACKET_GEN_RATE,MAX_PACKETS,WINDOW_SIZE,MAX_BUFFER_SIZE,pos,buffer_size=0,timeout=0;
int l=0,r=0,RTT_avg=0,RTT_avg_milli=0,RTT_avg_micro=0,hrtt=0,term=0,psent=0;
string ip_addr;
vector <string> packets_buffer;
vector <int> sentnum;
vector <long long int> start_times;
map <int,int> to;

//Global socket variables.
int sockfd;
struct sockaddr_in servaddr;
socklen_t server_addr_len=sizeof(servaddr);

//Thread for cheking termination condition constantly.
void* terminator(void* arg)
{
    //Termination checking loop.
    while(l<MAX_PACKETS && term==0);

    return NULL;
}

//Thread for Periodic Packet Generation.
void* packet_generator(void* arg)
{
    int i,j,k;
    int packs=0;
    //Seconds/Packet calculation.
    double interval=1.0/PACKET_GEN_RATE;

    while(l<MAX_PACKETS && term==0)
    {
        packs++;

        string packet="";

        //Random string generator.
        for(i=0 ; i<PACKET_LENGTH ; i++)
        {
            int num=97+rand()%26;

            packet+=num;
        }

        //Converting number to binary string.
        bitset <8> lf((packets_buffer.size())%256);

        packet=lf.to_string()+packet;

        //Pushing packet into the buffer.
        if(buffer_size!=MAX_BUFFER_SIZE)
        {
            packets_buffer.push_back(packet);
            buffer_size++;
        }

        //Sleeping thread for sometime to maintain constant rate of packet generation.
        this_thread::sleep_for(duration<double>(interval));

        cout << "";
        //cout << buffer_size << " packets are present" << endl;
    }

    return NULL;
}

//Thread for sending packets to the receiver.
void* packet_sender(void* arg)
{
    while(l<MAX_PACKETS && term==0)
    {
        string sbuffer;

        //If timeout setting the window to empty.
        if(timeout==1)
        {
            r=l;
        }

        //Checking if buffer is full or window size exceeds.
        if(r-l>=WINDOW_SIZE || r>=packets_buffer.size() || r>=MAX_PACKETS) continue;

        sbuffer=packets_buffer[r];

        //Setting termination if retransmissions exceeds 5.
        if(sentnum[r]>5)
        {
            term=1;
            continue;
        }

        //Sending packet to the sender.
        sendto(sockfd,sbuffer.c_str(),sbuffer.length(),0,(const struct sockaddr*)&servaddr,sizeof(servaddr));
        sentnum[r]++;

        psent++;

        //Setting start times for each packet.
        auto now = time_point_cast<microseconds>(system_clock::now());
        long long int value = now.time_since_epoch().count();

        start_times[r++]=value;

        if(timeout==1 && l==r-1) timeout=0;

        //cout << "Sent Packet-" << r-1 << " at "  << start_times[r-1] << " and psent = " << psent << endl;
    }

    return NULL;
}

//Thread for receiving acks sent by the receiver.
void* ack_receiver(void* arg)
{
    while(l<MAX_PACKETS && term==0)
    {
        string rbuffer(1024,'\0');
        int bytes;

        //Receving ack packet from the receiver.
        bytes=recvfrom(sockfd,&rbuffer[0],rbuffer.capacity(),0,(struct sockaddr *)&servaddr,&server_addr_len);

        rbuffer.resize(bytes);

        //cout << "rbuffer=" << rbuffer << endl;

        int seq=stoi(rbuffer);

        //Checking if ack sequence number matches.
        if(seq==l)
        {
            auto now = time_point_cast<microseconds>(system_clock::now());
            long long int value = now.time_since_epoch().count();

            long long int milli=value/1000;
            long long int micro=value%1000;

            //Calculating average RTT.
            RTT_avg=(RTT_avg*hrtt+(value-start_times[l]))/(hrtt+1);

            RTT_avg_milli=RTT_avg/1000;
            RTT_avg_micro=RTT_avg%1000;

            hrtt++;

            //cout << "Received Ack-" << seq << " at " << value << " but " << RTT_avg_milli << endl;

            //Printing debug statements.
            if(debug==1)
            {
                cout << "SEQ-" << l%256 << endl;
                cout << "TIME GENERATED = " << start_times[l]/1000 << ":" << start_times[l]%1000 << endl;
                cout << "RTT = " << (value-start_times[l])/1000 << endl;

                if(to.find(l)!=to.end()) cout << "NUMBER OF ATTEMPTS = " << to[l] << endl;
                else cout << "NUMBER OF ATTEMPTS = " << 1 << endl;

                cout << endl;
            }

            l++;

            buffer_size--;
        }
    }

    return NULL;
}

//Thread for checking whether timeout has occured.
void* timeout_checker(void* arg)
{
    while(l<MAX_PACKETS && term==0)
    {
        auto now = time_point_cast<microseconds>(system_clock::now());
        long long int value = now.time_since_epoch().count();

        //Checking if timeout has occured.
        if(timeout==0 && l!=r && start_times[l]!=-1 && ((l<10 && (value-start_times[l])/1000>100) || (l>=10 && (value-start_times[l])/1000>(2*RTT_avg)/1000)))
        {
            //cout << "timedout at " << l << " but " << (value-start_times[l])/1000 << " and " << (2*RTT_avg)/1000 << " at " << value << endl;
            timeout=1;
        }
    }

    return NULL;
}

int main(int argc, char* argv[])
{
    int i,j,k;

    //Decoding the flags.
    for(i=1 ; i<argc ; i++)
    {
        string opt=argv[i];

        if(opt=="-d") debug=1;
        else if(opt=="-s")
        {
            ip_addr=argv[i+1];
            //cout << "ip_addr = " << ip_addr << endl;

            i++;
        }
        else if(opt=="-p")
        {
            port_num=atoi(argv[i+1]);
            //cout << "port_num = " << port_num << endl;

            i++;
        }
        else if(opt=="-l")
        {
            PACKET_LENGTH=atoi(argv[i+1]);
            //cout << "PACKET_LENGTH = " << PACKET_LENGTH << endl;

            i++;
        }
        else if(opt=="-r")
        {
            PACKET_GEN_RATE=atoi(argv[i+1]);
            //cout << "PACKET_GEN_RATE = " << PACKET_GEN_RATE << endl;

            i++;
        }
        else if(opt=="-n")
        {
            MAX_PACKETS=atoi(argv[i+1]);
            //cout << "max_packets = " << MAX_PACKETS << endl;

            i++;
        }
        else if(opt=="-w")
        {
            WINDOW_SIZE=atoi(argv[i+1]);
            //cout << "window_size = " << WINDOW_SIZE << endl;

            i++;
        }
        else if(opt=="-f")
        {
            MAX_BUFFER_SIZE=atoi(argv[i+1]);
            //cout << "max_buffer_size = " << MAX_BUFFER_SIZE << endl;

            i++;
        }
    }

    //cout << debug << endl;

    //Socket initialization.
    sockfd=socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd==0)
    {
        perror("socket failed\n");

        exit(EXIT_FAILURE);
    }

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=INADDR_ANY;
    servaddr.sin_port=htons(port_num);

    start_times.resize(MAX_PACKETS);
    sentnum.resize(MAX_PACKETS);

    for(int i=0 ; i<MAX_PACKETS ; i++)
    {
        start_times[i]=-1;
    }

    srand(time(NULL));

    //Thread creation and launching.
    pthread_t tid[5];

    pthread_create(&tid[0],NULL,packet_generator,NULL);
    pthread_create(&tid[1],NULL,packet_sender,NULL);
    pthread_create(&tid[2],NULL,ack_receiver,NULL);
    pthread_create(&tid[3],NULL,timeout_checker,NULL);
    pthread_create(&tid[4],NULL,terminator,NULL);

    /*for(i=0 ; i<packets_buffer.size() ; i++)
    {
        cout << packets_buffer[i] << endl;
    }*/
    
    /*pthread_join(tid[0],NULL);
    pthread_join(tid[1],NULL);
    pthread_join(tid[2],NULL);
    pthread_join(tid[3],NULL);*/

    pthread_join(tid[4],NULL);

    //Printing the statistics.
    cout << "PACKET_GEN_RATE = " << PACKET_GEN_RATE << endl;
    cout << "PACKET_LENGTH = " << PACKET_LENGTH << endl;
    cout << "RETRANSMISSION RATIO = " << ((float)psent)/hrtt << endl;
    cout << "AVERAGE RTT = " << RTT_avg_milli << ":" << RTT_avg_micro << endl;

    /*for(int i=0 ; i<start_times.size() ; i++)
    {
        cout << start_times[i] << " ";
    }
    cout << endl;*/

    return 0;
}