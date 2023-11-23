/* TCPfile.c - main, TCPfile */

#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <errno.h>

void	TCPecho(const char *host, const char *service);
int	errexit(const char *format, ...);

int	connectsock(const char *host, const char *service,
		const char *transport);

#define	LINELEN		4096

/*------------------------------------------------------------------------
 * main - TCP client for ECHO service
 *------------------------------------------------------------------------
 */
int main(int argc, char *argv[])
{
	char	*host = "localhost";	/* host to use if none supplied	*/
	char	*service = "echo";	/* default service name		*/

	switch (argc) {
	case 1:
		host = "localhost";
		break;
	case 3:
		service = argv[2];
		/* FALL THROUGH */
	case 2:
		host = argv[1];
		break;
	default:
		fprintf(stderr, "usage: TCPecho [host [port]]\n");
		exit(1);
	}

	TCPecho(host, service);

	exit(0);
}

/*------------------------------------------------------------------------
 * TCPecho - send input to ECHO service on specified host and print reply
 *------------------------------------------------------------------------
 */
void 
TCPecho(const char *host, const char *service)
{
	char	buf[LINELEN+1];		/* buffer for one line of text	*/
	int	s, n;			/* socket descriptor, read count*/
	int	outchars, inchars;	/* characters sent and received	*/
	struct sockaddr_in serveraddr;
	socklen_t len;

	s = 	connectsock(host, service, "tcp");
	if (s < 0)
	  {
		fprintf(stderr, "usage: TCPecho connectsock failed. \n");
		exit(1);
	  }
	
	len = sizeof(serveraddr);
	getpeername(s, (struct sockaddr*)&serveraddr, &len);

	/*printf("************** \n");
	printf("Server IP address: %s\n", inet_ntoa(serveraddr.sin_addr));
	printf("Server port      : %d\n", ntohs(serveraddr.sin_port));
	printf("************** \n");*/

	while (1)
    {
        printf("Enter filename and number of last bytes (filename and bytes should be space separated) : ");
        fgets(buf, sizeof(buf), stdin);

		buf[LINELEN] = '\0';	/* insure line null-terminated	*/
		outchars = strlen(buf);
		(void) write(s, buf, sizeof(buf));

		if(outchars==5 && buf[0]=='e' && buf[1]=='x' && buf[2]=='i' && buf[3]=='t')
		{
			printf("\nExiting .....\n");

			exit(0);
		}

		char file[BUFSIZ];
        char file1[BUFSIZ];

		int i=0,j=0;

		for( ; buf[i]!=' ' ; i++,j++)
        {
            file[i]=file1[j]=buf[i];
            if(buf[i]=='.')
            {
                file1[j++]='1';
                file1[j]='.';
            }
        }
            
		file[i]=file1[j]='\0';

        i++;

        int bytes=0;

		for(; i<strlen(buf)-1 ; i++)
        {
            int c=buf[i]-48;

            bytes=bytes*10+c;
        }

		/* read it back */
		
        n = read(s, buf, sizeof(buf));
        if (n < 0)
            errexit("socket read failed: %s\n",
                strerror(errno));

		if(strlen(buf)==6 && buf[0]=='S' && buf[1]=='O' && buf[2]=='R' && buf[3]=='R' && buf[4]=='Y' && buf[5]=='!')
		{
			printf("Server says that the file does not exist.\n\n");
		}
		else
		{
			FILE *fd3;

			fd3=fopen(file1,"w");

			fprintf(fd3,"%s",buf);

			fclose(fd3);

			printf("Last %d bytes of \"%s\" saved in \"%s\" successfully.\n\n",bytes,file,file1);
		}
	}
}
