// NAME:R.Neel Prabhanjan
// Roll Number:CS20B056
// Course: CS3205 Jan. 2023 semester
// Lab number: 5
// Date of submission: 28-4-23
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

//Global variables so that they can be accessed from threads also.
int id,HELLO_INTERVAL=1,LSA_INTERVAL=5,SPF_INTERVAL=20;
string input_file,output_file;
int N,E;
int lsa_num=0;
long long int start;
vector <vector <pair <int,int>>> G;
vector <vector <int>> T;
vector <vector <int>> paths;
vector <int> short_dist;
vector <int> neighbours;
vector <int> neighbours_costs;
vector <int> lsa_seq;
vector <string> lsa_messages;

//Global sockets varabiles.
int gsockfd;
struct sockaddr_in gservaddr,gcliaddr;
socklen_t glen=sizeof(gcliaddr);

int sockfd;
struct sockaddr_in servaddr;
socklen_t server_addr_len=sizeof(servaddr);

//Function to find the vertex with minimum short_distance value, from the set of vertices not yet included in shortest path tree.
int min_short_dist(vector <int> pres)
{
    int minval=INT_MAX,mini;

    //Finding the shortest distance vertex.
    for(int i=0 ; i<N ; i++)
    {
        if (pres[i]==0 && short_dist[i]<=minval)
        {
            minval=short_dist[i];
            mini=i;
        }
    }

    return mini;
}

//Function to print the final shortest short_distance array and shortest paths from the source vertex.
void path_finder(vector <int> parent)
{
    int i,j,k;

    //Iterating over parents to find the paths.
    for(i=0 ; i<N ; i++)
    {
        j=i;

        while(j!=-1)
        {
            paths[i].push_back(j);

            j=parent[j];
        }

        reverse(paths[i].begin(),paths[i].end());
    }

    //Resetting the parent values to -1.
    for(i=0 ; i<N ; i++)
    {
        parent[i]=-1;
    }
}

// Function to implement Dijkstra's algorithm for a graph represented using adjacency matrix representation
void dijkstra(vector <vector <int>> G , int src)
{
    int i,j,k;
    
    //Array to store the shortest path tree
    vector <int> parent(N,-1);
    //pres[i] will be true if vertex i is included in shortest path tree or shortest short_distance from src to i is finalized  
    vector <int> pres(N,0);
    
    //Initializing short dist from source to source as 0.
    short_dist[src]=0;
    parent[src]=-1;

    //Find shortest path for all vertices
    for (int i=0 ; i<N-1; i++)
    {
        int v=min_short_dist(pres);

        //Mark the picked vertex as processed
        pres[v]=1;

        //Update short_dist value of the adjacent vertices of the picked vertex
        for (int j=0 ; j<N ; j++)
        {
            //Update short_dist[v] only if it's not in pres, there is an edge from u to v, and total weight of path from src to v through u is smaller than current value of short_dist[v]
            if(pres[j]==0 && G[v][j] && short_dist[v]!=INT_MAX && short_dist[v]+G[v][j]<short_dist[j])
            {
                short_dist[j]=short_dist[v]+G[v][j];
                parent[j]=v;
            }
        }
    }

    //Identity the shortest path.
    path_finder(parent);
}

//Thread to send and receive hello packets?
void* hello_sender_receiver(void* arg)
{
    int i,j,k;

    //Creating the hello packet.
    string sbuffer="HELLO "+to_string(id);

    //Sending hello packets periodically.
    while(lsa_num<10)
    {
        sleep(HELLO_INTERVAL);

        for(i=0 ; i<neighbours.size() ; i++)
        {
            servaddr.sin_port=htons(10030+neighbours[i]);

            //Send hello packet.
            sendto(sockfd,sbuffer.c_str(),sbuffer.length(),0,(const struct sockaddr*)&servaddr,sizeof(servaddr));

            //cout << "Sent '" << sbuffer << "' from router-" << id << " to router-" << neighbours[i] << endl;

            string rbuffer(1024,'\0');
            int bytes;

            //Receiver hello reply packet.
            bytes=recvfrom(sockfd,&rbuffer[0],rbuffer.capacity(),0,(struct sockaddr *)&servaddr,&server_addr_len);

            rbuffer.resize(bytes);

            //cout << "Received '" << rbuffer << "' from router-" << neighbours[i] << " to router-" << id << endl;

            string cost="";

            for(j=rbuffer.length()-1 ; j>=0 ; j--)
            {
                if(rbuffer[j]==' ') break;

                cost=rbuffer[j]+cost;
            }

            //Update the neighbours'costs.
            neighbours_costs[neighbours[i]]=stoi(cost);
        }
    }

    return NULL;
}

//Thread to reply the received hello packets.
void* hello_replier(void* arg)
{
    int i,j,k;

    while(1)
    {
        string rbuffer(1024,'\0');

        //Receive the hello packets.
        int bytes=recvfrom(gsockfd,&rbuffer[0],rbuffer.capacity(),0,(struct sockaddr *)&gcliaddr,&glen);
    
        rbuffer.resize(bytes);

        string message="";

        for(i=0 ; i<rbuffer.size() ; i++)
        {
            if(rbuffer[i]==' ') break;

            message+=rbuffer[i];
        }
        j=i+1;

        //Check if message is hello or LSA.
        if(message=="HELLO")
        {
            //cout << "Received '" << rbuffer << " to router-" << id << endl;

            string s="";

            for(i=rbuffer.length()-1 ; i>=0 ; i--)
            {
                if(rbuffer[i]==' ') break;

                s=rbuffer[i]+s;
            }

            //Decode source ID.
            int srcid=stoi(s);

            //Generate random cost.
            int cost=rand()%(G[id][srcid].second-G[id][srcid].first+1)+G[id][srcid].first;

            string sbuffer="HELLOREPLY "+to_string(id)+" "+s+" "+to_string(cost);

            //Send hello reply.
            sendto(gsockfd,sbuffer.c_str(),sbuffer.length(),0,(const struct sockaddr*)&gcliaddr,glen);

            //cout << "Sent '" << sbuffer << "' from router-" << id << endl;
        }
        else if(message=="LSA")
        {
            //cout << "Received '" << rbuffer << " to router-" << id << endl;

            string s="";

            for(i=j ; i<rbuffer.length() ; i++)
            {
                if(rbuffer[i]==' ') break;

                s+=rbuffer[i];
            }
            j=i+1;

            //Decode src ID.
            int srcid=stoi(s);

            s="";

            for(i=j ; i<rbuffer.size() ; i++)
            {
                if(rbuffer[i]==' ') break;

                s+=rbuffer[i];
            }
            j=i+1;

            //Decode seq. number.
            int seq=stoi(s);

            if(lsa_seq[srcid]>=seq) continue;

            //Update last largest seq. number received from that source id.
            lsa_seq[srcid]=seq;

            lsa_messages.push_back(rbuffer);

            for(i=0 ; i<neighbours.size() ; i++)
            {
                if(neighbours[i]==srcid) continue;

                servaddr.sin_port=htons(10030+neighbours[i]);

                //Send LSA message to neighbours.
                sendto(sockfd,rbuffer.c_str(),rbuffer.length(),0,(const struct sockaddr*)&servaddr,sizeof(servaddr));

                //cout << "Sent '" << rbuffer << "' from router-" << id << " to router-" << neighbours[i] << endl;
            }
        }
    }

    return NULL;
}

//Thread to send LSA message periodically.
void* LSA_sender(void* arg)
{
    int i,j,k;

    while(lsa_num<10)
    {
        sleep(LSA_INTERVAL);

        string LSA="";

        //Construct LSA messages.
        LSA="LSA "+to_string(id)+" "+to_string(lsa_num)+" "+to_string(neighbours.size());

        for(i=0 ; i<neighbours.size() ; i++)
        {
            LSA+=" "+to_string(neighbours[i])+" "+to_string(neighbours_costs[neighbours[i]]);
        }

        lsa_messages.push_back(LSA);

        for(i=0 ; i<neighbours.size() ; i++)
        {
            servaddr.sin_port=htons(10030+neighbours[i]);

            //Send LSA message to neighbours.
            sendto(sockfd,LSA.c_str(),LSA.length(),0,(const struct sockaddr*)&servaddr,sizeof(servaddr));

            //cout << "Sent '" << LSA << "' from router-" << id << " to router-" << neighbours[i] << endl;
        }

        lsa_num++;
    }

    return NULL;
}

//Thread to build Routing table periodically.
void* RT_builder(void* arg)
{
    int i,j,k;

    string ofile="";

    for(i=0 ; i<output_file.length()-4 ; i++)
    {
        ofile+=output_file[i];
    }
    
    ofile+="-"+to_string(id)+".txt";

    //Opening the output file.
    ofstream out_file(ofile);

    while(1)
    {
        sleep(SPF_INTERVAL);

        //Process the LSA messages and update the graph.
        for(i=0 ; i<lsa_messages.size() ; i++)
        {
            string lsa=lsa_messages[i];

            for(j=0 ; j<lsa.length() ; j++)
            {
                if(lsa[j]==' ')
                {
                    j++;

                    break;
                }
            }

            string s="";

            for( ; j<lsa.length() ; j++)
            {
                if(lsa[j]==' ')
                {
                    j++;

                    break;
                }

                s+=lsa[j];
            }

            int srcid;

            srcid=stoi(s);

            int sp=0;

            for( ; j<lsa.length() ; j++)
            {
                if(lsa[j]==' ') sp++;

                if(sp==2)
                {
                    j++;

                    break;
                }
            }

            //Update the graph with the costs of edges.
            sp=0;
            string neigh="",cost="";

            for( ; j<lsa.length()+1 ; j++)
            {
                if(j==lsa.length() || lsa[j]==' ')
                {
                    sp++;
                    if(j!=lsa.length()) continue;
                }

                if(sp!=0 && sp%2==0)
                {
                    int a=stoi(neigh);
                    int b=stoi(cost);

                    T[srcid][a]=b;
                    T[a][srcid]=b;

                    neigh="";
                    cost="";
                }

                if(j==lsa.length()) break;

                if(sp%2==0) neigh+=lsa[j];
                else if(sp%2==1) cost+=lsa[j];
            }
        }

        /*for(i=0 ; i<N ; i++)
        {
            for(j=0 ; j<N ; j++)
            {
                cout << T[i][j] << " ";
                //T[i][j]=0;
            }
            cout << endl;
        }*/

        //Find the shortest paths using dijkstra's algorithm.
        dijkstra(T,id);

        auto now = time_point_cast<seconds>(system_clock::now());
        long long int value = now.time_since_epoch().count();

        //Print the routing table in the output file.

        out_file << "Routing table for Node no. " << id << " at time " << value-start << endl;

        for(i=0 ; i<39 ; i++)
        {
            out_file << "=";
        }
        out_file << endl;

        out_file << left << setw(20) << "Destination" << setw(20) << "Path" << "Cost" << endl;

        //Print shortest paths and costs in the output file.
        for(i=0 ; i<N ; i++)
        {
            if(short_dist[i]==0) continue;

            out_file << left << setw(20) << i;

            for(j=0 ; j<paths[i].size() ; j++)
            {
                if(j!=paths[i].size()-1)
                {
                    out_file << left << paths[i][j] << "-";
                }
                else out_file << left << setw(20) << paths[i][j];
            }

            out_file << left << short_dist[i] << endl;
        }
        out_file << endl;

        //Reset paths and short_dist arrays.
        for(i=0 ; i<N ; i++)
        {
            vector <int> v(0);
            for(j=0 ; j<N ; j++)
            {
                //cout << T[i][j] << " ";
                T[i][j]=0;
            }
            //cout << endl;

            paths[i]=v;
            short_dist[i]=INT_MAX;
        }
        
    }

    return NULL;
}

int main(int argc, char* argv[])
{
    int i,j,k;

    srand(time(0));

    //Decoding the flags.
    for(i=1 ; i<argc ; i++)
    {
        string flag=argv[i++];

        if(flag=="-i")
        {
            string s=argv[i];

            id=stoi(s);

            //cout << "id=" << id << endl;
        }
        else if(flag=="-f")
        {
            input_file=argv[i];

            //cout << "input_file=" << input_file << endl;
        }
        else if(flag=="-o")
        {
            output_file=argv[i];

            //cout << "output_file=" << output_file << endl;
        }
        else if(flag=="-h")
        {
            string s=argv[i];
            
            HELLO_INTERVAL=stoi(s);

            //cout << "HELLO_INTERVAL=" << HELLO_INTERVAL << endl;
        }
        else if(flag=="-a")
        {
            string s=argv[i];

            LSA_INTERVAL=stoi(s);

            //cout << "LSA_INTERVAL=" << LSA_INTERVAL << endl;
        }
        else if(flag=="-s")
        {
            string s=argv[i];

            SPF_INTERVAL=stoi(s);

            //cout << "SPF_INTERVAL=" << SPF_INTERVAL << endl;
        }
    }

    //Reading the input file.
    ifstream in_file(input_file);

    in_file >> N >> E;

    //Entering the entries in the adjacency matrix.
    for(i=0 ; i<N ; i++)
    {
        vector <pair <int,int>> v(N,pair <int,int> (0,0));
        vector <int> u(N,0);
        vector <int> w(0);

        G.push_back(v);
        T.push_back(u);
        short_dist.push_back(INT_MAX);
        paths.push_back(w);
    }

    for(i=0 ; i<E ; i++)
    {
        int a,b,min_cost,max_cost;

        in_file >> a >> b >> min_cost >> max_cost;

        G[a][b]=pair <int,int> (min_cost,max_cost);
        G[b][a]=pair <int,int> (min_cost,max_cost);
    }

    for(j=0 ; j<N ; j++)
    {
        if(G[id][j].first!=0 || G[id][j].second!=0)
        {
            neighbours.push_back(j);
        }

        neighbours_costs.push_back(0);
        lsa_seq.push_back(-1);
    }

    //cout << "N=" << N << " E=" << E << endl;

    /*for(i=0 ; i<N ; i++)
    {
        for(j=0 ; j<N ; j++)
        {
            cout << "(" << G[i][j].first << "," << G[i][j].second << ")" << " ";
        }
        cout << endl;
    }

    cout << "NEIGHBOURS" << endl;

    for(i=0 ; i<neighbours.size() ; i++)
    {
        cout << neighbours[i] << " ";
    }
    cout << endl;*/

    //Socket initializations.
    sockfd=socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd==0)
    {
        perror("socket failed\n");

        exit(EXIT_FAILURE);
    }

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=INADDR_ANY;
    servaddr.sin_port=htons(10030);

    gsockfd=socket(AF_INET,SOCK_DGRAM,0);
    if(gsockfd==0)
    {
        perror("socket failed\n");

        exit(EXIT_FAILURE);
    }

    memset(&gservaddr,0,sizeof(gservaddr));
    gservaddr.sin_family=AF_INET;
    gservaddr.sin_addr.s_addr=INADDR_ANY;
    gservaddr.sin_port=htons(10030+id);

    if(bind(gsockfd,(const struct sockaddr *)&gservaddr,sizeof(gservaddr))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    auto now = time_point_cast<seconds>(system_clock::now());
    start = now.time_since_epoch().count();

    //Creating threads for each of the functionality.
    pthread_t tid[4];

    pthread_create(&tid[0],NULL,hello_sender_receiver,NULL);
    pthread_create(&tid[1],NULL,hello_replier,NULL);
    pthread_create(&tid[2],NULL,LSA_sender,NULL);
    pthread_create(&tid[3],NULL,RT_builder,NULL);

    pthread_join(tid[2],NULL);

    return 0;
}