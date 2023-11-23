// NAME: R.Neel Prabhanjan
// Roll Number: CS20B056
// Course: CS3205 Jan. 2023 semester
// Lab number: 2
// Date of submission: 3-3-23
// I confirm that the source file is entirely written by me without resorting to any dishonest means.

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

#define BUF_SIZE 1024

int PORT_NUM;

struct server_ipaddr
{
    char name[BUF_SIZE];
    char ip_addr[BUF_SIZE];
};

struct ads_servers
{
    char name[BUF_SIZE];
    struct server_ipaddr servers[5];
};

struct server_ipaddr servers[10];
struct ads_servers ads_servers[6];

void other_server(int i)
{
    int sockfd;
    struct sockaddr_in servaddr,cliaddr;
    char buffer[BUF_SIZE];
    socklen_t len=sizeof(cliaddr);

    sockfd=socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd==0)
    {
        perror("socket failed\n");

        exit(EXIT_FAILURE);
    }

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(PORT_NUM+53+i);

    if(bind(sockfd,(const struct sockaddr *)&servaddr,sizeof(servaddr))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    while(1)
    {
        recvfrom(sockfd,(char *)buffer,BUF_SIZE,0,(struct sockaddr *)&cliaddr,&len);

        if(buffer[0]=='b' && buffer[1]=='y' && buffer[2]=='e')
        {
            break;
        }

        if(i==1)
        {
            FILE* fptr;
            fptr=fopen("RDS.output","a");

            if(fptr==NULL)
            {
                printf("Error!");

                exit(1);           
            }

            fprintf(fptr,"QUERY :- %s",buffer);

            if(buffer[strlen(buffer)-2]=='m' && buffer[strlen(buffer)-3]=='o' && buffer[strlen(buffer)-4]=='c' && buffer[strlen(buffer)-5]=='.')
            {
                int s;
                for(s=0 ; s<strlen(servers[2].ip_addr)-1 ; s++)
                {
                    buffer[s]=servers[2].ip_addr[s];
                }
                buffer[s++]=' ';
                buffer[s]='\0';
                char buf2[BUF_SIZE];
                sprintf(buf2,"%d",PORT_NUM+55);
                strcat(buffer,buf2);
                buffer[s+strlen(buf2)]='\0';
            }
            else if(buffer[strlen(buffer)-2]=='u' && buffer[strlen(buffer)-3]=='d' && buffer[strlen(buffer)-4]=='e' && buffer[strlen(buffer)-5]=='.')
            {
                int s;
                for(s=0 ; s<strlen(servers[3].ip_addr)-1 ; s++)
                {
                    buffer[s]=servers[3].ip_addr[s];
                }
                buffer[s++]=' ';
                buffer[s]='\0';
                char buf2[BUF_SIZE];
                sprintf(buf2,"%d",PORT_NUM+56);
                strcat(buffer,buf2);
                buffer[s+strlen(buf2)]='\0';
            }
            else
            {
                char buf2[BUF_SIZE]="No DNS Record Found\0";

                strcpy(buffer,buf2);
            }

            fprintf(fptr,"RESPONSE :- %s\n",buffer);

            fclose(fptr);

            sendto(sockfd,(char *)buffer,BUF_SIZE,0,(const struct sockaddr*)&cliaddr,len);
        }
        else if(i==2 || i==3)
        {
            FILE* fptr;
            fptr=fopen("TDS.output","a");

            if(fptr==NULL)
            {
                printf("Error!");

                exit(1);           
            }

            fprintf(fptr,"QUERY :- %s",buffer);

            int j=0,k,dots;

            while(buffer[j]!=' ')
            {
                if(buffer[j]=='.') dots++;

                j++;
            }
            j=0;

            int l;

            while(dots>0)
            {
                if(buffer[j]=='.')
                {
                    dots--;

                    if(dots==1) k=j+1;
                    if(dots==0) l=j-1;
                }

                j++;
            }

            int t;
            for(t=0 ; t<6 ; t++)
            {
                if((l-k+1)==strlen(ads_servers[t].name))
                {
                    //printf("hi %d %d %ld %s\n",t,(l-k+1),strlen(ads_servers[t].name),ads_servers[t].name);
                    int m;
                    for(m=0 ; m<(l-k+1) ; m++)
                    {
                        if(ads_servers[t].name[m]!=buffer[(m+k)])
                        {
                            //printf("%c %c\n",ads_servers[t].name[m],buffer[(m+l)]);
                            break;
                        }
                    }

                    if(m==l-k+1)
                    {
                        int s;
                        for(s=0 ; s<strlen(servers[t+4].ip_addr)-1 ; s++)
                        {
                            buffer[s]=servers[t+4].ip_addr[s];
                        }
                        buffer[s++]=' ';
                        buffer[s]='\0';
                        char buf2[BUF_SIZE];
                        sprintf(buf2,"%d",PORT_NUM+t+57);
                        strcat(buffer,buf2);
                        buffer[s+strlen(buf2)]='\0';

                        break;
                    }
                }
            }

            if(t==6)
            {
                char buf2[BUF_SIZE]="No DNS Record Found\0";

                strcpy(buffer,buf2);
            }

            fprintf(fptr,"RESPONSE :- %s\n",buffer);

            fclose(fptr);

            sendto(sockfd,(char *)buffer,BUF_SIZE,0,(const struct sockaddr*)&cliaddr,len);
        }
        else
        {
            FILE* fptr;
            fptr=fopen("ADS.output","a");

            if(fptr==NULL)
            {
                printf("Error!");

                exit(1);           
            }

            fprintf(fptr,"QUERY :- %s",buffer);

            int s;
            buffer[strlen(buffer)-1]='\0';

            for(s=0 ; s<5 ; s++)
            {
                if(strcmp(buffer,ads_servers[i-4].servers[s].name)==0)
                {
                    strcpy(buffer,ads_servers[i-4].servers[s].ip_addr);

                    break;
                }
            }

            if(s==5)
            {
                char buf2[BUF_SIZE]="No DNS Record Found\0";

                strcpy(buffer,buf2);
            }

            fprintf(fptr,"RESPONSE :- %s",buffer);
            if(buffer[0]=='N') fprintf(fptr,"\n");

            fclose(fptr);

            sendto(sockfd,(char *)buffer,BUF_SIZE,0,(const struct sockaddr*)&cliaddr,len);
        }

        //printf("%s from %d dns server\n",buffer,i);
    }
}

void local_dns()
{
    int i,j,k;

    int sockfd[10];
    struct sockaddr_in servaddr[10],cliaddr;
    char buffer[10][BUF_SIZE];
    socklen_t len=sizeof(cliaddr);

    for(i=0 ; i<10 ; i++)
    {
        sockfd[i]=socket(AF_INET,SOCK_DGRAM,0);
        if(sockfd[i]==0)
        {
            perror("socket failed\n");

            exit(EXIT_FAILURE);
        }

        memset(&servaddr[i],0,sizeof(servaddr[i]));
        servaddr[i].sin_family=AF_INET;
        servaddr[i].sin_addr.s_addr=htonl(INADDR_ANY);
        servaddr[i].sin_port=htons(PORT_NUM+53+i);

        if(i==0)
        {
            if(bind(sockfd[i],(const struct sockaddr *)&servaddr[i],sizeof(servaddr[i]))<0)
            {
                perror("bind failed");
                exit(EXIT_FAILURE);
            }
        }
    }

    while(1)
    {
        FILE* fptr;
        fptr=fopen("NR.output","a");

        if(fptr==NULL)
        {
            printf("Error!");

            exit(1);           
        }

        recvfrom(sockfd[0],(char *)buffer[0],BUF_SIZE,0,(struct sockaddr *)&cliaddr,&len);

        if(buffer[0][0]=='b' && buffer[0][1]=='y' && buffer[0][2]=='e')
        {
            for(i=1 ; i<10 ; i++)
            {
                sendto(sockfd[i],(const char *)buffer[0],BUF_SIZE,0,(const struct sockaddr*)&servaddr[i],len);
            }

            break;
        }

        fprintf(fptr,"QUERY :- %s",buffer);

        sendto(sockfd[1],(const char *)buffer[0],BUF_SIZE,0,(const struct sockaddr*)&servaddr[1],len);

        recvfrom(sockfd[1],(char *)buffer[1],BUF_SIZE,0,(struct sockaddr *)&servaddr[1],&len);

        int tld;
        int c=0,num=0;
        while(buffer[1][c]!=' ') c++;
        c++;
        while(buffer[1][c]!='\0') num=num*10+buffer[1][c++]-48;
        if(num==PORT_NUM+55) tld=2;
        else if(num==PORT_NUM+56)  tld=3;
        else
        {
            fprintf(fptr,"RESPONSE :- %s\n",buffer[1]);

            fclose(fptr);

            sendto(sockfd[0],(const char *)(buffer[1]),BUF_SIZE,0,(const struct sockaddr*)&cliaddr,len);

            continue;
        }

        sendto(sockfd[tld],(const char *)buffer[0],BUF_SIZE,0,(const struct sockaddr*)&servaddr[tld],len);

        recvfrom(sockfd[tld],(char *)(buffer[tld]),BUF_SIZE,0,(struct sockaddr *)&servaddr[tld],&len);

        int ads;
        //printf("%d\n",buffer[tld][strlen(buffer[tld])-1]);
        c=0,num=0;
        while(buffer[tld][c]!=' ') c++;
        c++;
        while(buffer[tld][c]!='\0') num=num*10+buffer[tld][c++]-48;
        if(num-PORT_NUM-53>3 && num-PORT_NUM-53<10) ads=num-PORT_NUM-53;
        else
        {
            fprintf(fptr,"RESPONSE :- %s\n",buffer[tld]);

            fclose(fptr);

            sendto(sockfd[0],(const char *)(buffer[tld]),BUF_SIZE,0,(const struct sockaddr*)&cliaddr,len);

            continue;
        }

        sendto(sockfd[ads],(const char *)buffer[0],BUF_SIZE,0,(const struct sockaddr*)&servaddr[ads],len);

        recvfrom(sockfd[ads],(char *)(buffer[ads]),BUF_SIZE,0,(struct sockaddr *)&servaddr[ads],&len);

        fprintf(fptr,"RESPONSE :- %s",buffer[ads]);
        if(buffer[ads][0]=='N') fprintf(fptr,"\n");

        fclose(fptr);

        sendto(sockfd[0],(const char *)(buffer[ads]),BUF_SIZE,0,(const struct sockaddr*)&cliaddr,len);
    }
}

int main(int argc , char* argv[])
{
    char* input_file=argv[2];
    PORT_NUM=atoi(argv[1]);

    int i,j,k,l,m;

    FILE* fp;
    char line[BUF_SIZE];

    fp=fopen(input_file,"r");

    if(fp==NULL)
    {
        printf("Input file not found !\n");

        return 0;
    }

    fgets(line,sizeof(line),fp);

    int line_num=0;

    while(line_num<10)
    {
        fgets(line,sizeof(line),fp);

        i=0,j=0;

        while(line[i]!=' ')
        {
            servers[line_num].name[j++]=line[i++];
        }
        servers[line_num].name[j]='\0';
        i++;
        j=0;
        while(line[i]!='\n')
        {
            servers[line_num].ip_addr[j++]=line[i++];
        }
        servers[line_num].ip_addr[j]='\0';

        line_num++;
    }

    line_num=0;

    i=0;

    while(i<6)
    {
        fgets(line,sizeof(line),fp);

        j=0;

        while(j<5)
        {
            fgets(line,sizeof(line),fp);

            k=0,l=0;
            int dots=0;

            while(line[l]!=' ')
            {
                if(line[l]=='.') dots++;

                ads_servers[i].servers[j].name[k++]=line[l++];
            }
            ads_servers[i].servers[j].name[k]='\0';
            l++;
            k=0;
            while(line[l]!='\n')
            {
                ads_servers[i].servers[j].ip_addr[k++]=line[l++];
            }
            ads_servers[i].servers[j].ip_addr[k]='\0';

            k=0,l=0;

            while(dots>0)
            {
                if(line[l]=='.')
                {
                    dots--;

                    l++;

                    continue;
                }

                if(dots==1) ads_servers[i].name[k++]=line[l];

                l++;
            }
            ads_servers[i].name[k]='\0';

            j++;
        }

        i++;
    }

    /*for(i=0 ; i<10 ; i++)
    {
        printf("%s %s\n",servers[i].name,servers[i].ip_addr);
    }

    for(i=0 ; i<6 ; i++)
    {
        printf("%s\n",ads_servers[i].name);

        for(j=0 ; j<5 ; j++)
        {
            printf("%s %s\n",ads_servers[i].servers[j].name,ads_servers[i].servers[j].ip_addr);
        }
    }*/

    fclose(fp);

    FILE* fptr;
    fptr=fopen("NR.output","w");
    fclose(fptr);
    fptr=fopen("RDS.output","w");
    fclose(fptr);
    fptr=fopen("TDS.output","w");
    fclose(fptr);
    fptr=fopen("ADS.output","w");
    fclose(fptr);

    pid_t pid[10];

    for(i=0 ; i<10 ; i++)
    {
        if(pid[i]=fork()==0)
        {
            if(i==0) local_dns();
            else other_server(i);
        
            exit(EXIT_SUCCESS);
        }
    }

    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[BUF_SIZE];
    //socklen_t len=sizeof(servaddr);

    sockfd=socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd==0)
    {
        perror("socket failed\n");

        exit(EXIT_FAILURE);
    }

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=INADDR_ANY;
    servaddr.sin_port=htons(PORT_NUM+53);

    while(1)
    {
        printf("Enter Server Name: ");
        fgets(buffer,BUF_SIZE,stdin);

        sendto(sockfd,(const char *)buffer,BUF_SIZE,0,(const struct sockaddr*)&servaddr,sizeof(servaddr));

        if(buffer[0]=='b' && buffer[1]=='y' && buffer[2]=='e')
        {
            for(i=0 ; i<10 ; i++)
            {
                kill(pid[i],SIGCHLD);
            }
            printf("All Server Processes are killed. Exiting.\n");

            return 0;
        }

        recvfrom(sockfd,buffer,BUF_SIZE,0,(struct sockaddr *)&servaddr,sizeof(servaddr));

        if(buffer[0]<=57) printf("DNS Mapping: %s\n",buffer);
        else printf("%s\n",buffer);
    }

    return 0;
}

