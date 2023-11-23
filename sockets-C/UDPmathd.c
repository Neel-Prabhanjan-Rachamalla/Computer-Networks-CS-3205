/* UDPmathd.c - main */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>

#include <errno.h>

int	passiveUDP(const char *service);
int	errexit(const char *format, ...);

#define BUFSIZE 4096

int
passivesock(const char *service, const char *transport, int qlen);

/*------------------------------------------------------------------------
 * main - Iterative UDP server for TIME service
 *------------------------------------------------------------------------
 */
int
main(int argc, char *argv[])
{
	struct sockaddr_in fsin;	/* the from address of a client	*/
	char	*service = "echo";	/* service name or port number	*/
	char	buf[BUFSIZ];
	int	sock;			/* server socket		*/
	unsigned int	alen;		/* from-address length		*/

	switch (argc) {
	case	1:
		break;
	case	2:
		service = argv[1];
		break;
	default:
		errexit("usage: UDPtimed [port]\n");
	}

	sock = passivesock(service, "udp", 0);
	/* Last parameter is Queue length and not applicable for UDP sockets*/
	while (1)
    {
        int count;
        alen = sizeof(fsin);

        count = recvfrom(sock, buf, sizeof(buf), 0,
                (struct sockaddr *)&fsin, &alen);

        if (count < 0)
            errexit("recvfrom: %s\n", strerror(errno));

        if(strlen(buf)==5 && buf[0]=='e' && buf[1]=='x' && buf[2]=='i' && buf[3]=='t')
        {
            printf("\nExiting .....\n");

            break;
        }

        printf("\nReceived : %s",buf);

        int i=4;
        int a=0,b=0;
        int op;

        if(buf[0]=='a' && buf[1]=='d' && buf[2]=='d') op=1;
        else if(buf[0]=='m' && buf[1]=='u' && buf[2]=='l') op=2;
        else if(buf[0]=='m' && buf[1]=='o' && buf[2]=='d') op=3;
        else if(buf[0]=='h' && buf[1]=='y' && buf[2]=='p') op=4;

        for(; buf[i]!=' ' ; i++)
        {
            int c=buf[i]-48;

            a=a*10+c;
        }
        i++;
        for(; i<strlen(buf)-1 ; i++)
        {
            int c=buf[i]-48;

            b=b*10+c;
        }

        int ans;

        if(op==1) ans=a+b;
        else if(op==2) ans=a*b;
        else if(op==3) ans=a%b;
        else if(op==4) ans=sqrt(a*a+b*b);

        char buf2[BUFSIZE];

        sprintf(buf2,"%d",ans);

        (void) sendto(sock, buf2, sizeof(buf2), 0,
                (struct sockaddr *)&fsin, sizeof(fsin));
	}
}
