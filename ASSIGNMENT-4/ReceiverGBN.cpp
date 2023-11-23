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
using namespace std;
using namespace std::chrono;

int MAX_PACKETS,NFE=0;
double RANDOM_DROP_PROB;

int main(int argc, char* argv[])
{
    int debug=0,port_num,rounds=0;
    int i,j,k;

    //Decoding the flags.
    for(i=1 ; i<argc ; i++)
    {
        string opt=argv[i];

        if(opt=="-d") debug=1;
        else if(opt=="-p")
        {
            port_num=atoi(argv[i+1]);
            //cout << "port_num = " << port_num << endl;

            i++;
        }
        else if(opt=="-n")
        {
            MAX_PACKETS=atoi(argv[i+1]);
            //cout << "max_packets = " << MAX_PACKETS << endl;

            i++;
        }
        else if(opt=="-e")
        {
            RANDOM_DROP_PROB=atof(argv[i+1]);
            //cout << "random_drop_prob = " << RANDOM_DROP_PROB << endl;

            i++;
        }
    }

    //cout << debug << endl;

    srand(time(0));

    //Socket initialization.
    int sockfd;
    struct sockaddr_in servaddr,cliaddr;
    socklen_t len=sizeof(cliaddr);

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

    if(bind(sockfd,(const struct sockaddr *)&servaddr,sizeof(servaddr))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    while(NFE+rounds*256<MAX_PACKETS)
    {
        string buffer(1024,'\0');

        //Receving the packet from the sender.
        int bytes=recvfrom(sockfd,&buffer[0],buffer.capacity(),0,(struct sockaddr *)&cliaddr,&len);
        
        buffer.resize(bytes);

        int seq=0;

        //Decoding the sequence number of the packet received.
        for(int i=0 ; i<8 ; i++)
        {
            if(buffer[i]=='1') seq+=pow(2,7-i);
        }

        auto now = time_point_cast<microseconds>(system_clock::now());
        long long int value = now.time_since_epoch().count();

        //cout << "Recevied-" << seq << "-" << buffer << " at " << value << endl;

        //Calculating probability for packet drop.
        double rand_num=((double)rand())/RAND_MAX;

        //Printing the debug statements.
        if(debug==1)
        {
            cout << "SEQ-" << seq << endl;
            cout << "TIME RECEIVED = " << value/1000 << ":" << value%1000 << endl;
            if(rand_num>RANDOM_DROP_PROB) cout << "PACKET DROPPED = FALSE" << endl;
            else cout << "PACKET DROPPED = TRUE" << endl;

            cout << endl;
        }

        //Checking if packet is dropped or not.
        if(rand_num>RANDOM_DROP_PROB)
        {
            if(NFE==seq)
            {
                buffer=to_string(seq+rounds*256);
                NFE++;
                NFE=NFE%256;

                if(NFE==0) rounds++;
            }
            else buffer=to_string(NFE-1+rounds*256);

            //Sending ack packet to the sender.
            sendto(sockfd,buffer.c_str(),buffer.length(),0,(const struct sockaddr*)&cliaddr,len);

            auto now = time_point_cast<microseconds>(system_clock::now());
            long long int value = now.time_since_epoch().count();

            //cout << "Sent - " << buffer << " at " << value << endl;
        }
    }

    return 0;
}