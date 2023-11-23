/* TCPfiled.c - main, TCPfiled */

#define	_USE_BSD
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <netinet/in.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define	QLEN		  32	/* maximum connection queue length	*/
#define	BUFSIZE		4096

#include <errno.h>

void	reaper(int);
int	TCPechod(int fd);
int	errexit(const char *format, ...);
int	passivesock(const char *service, const char *transport, int qlen);

/*------------------------------------------------------------------------
 * main - Concurrent TCP server for ECHO service
 *------------------------------------------------------------------------
 */
int
main(int argc, char *argv[])
{
	char	*service = "echo";	/* service name or port number	*/
	struct	sockaddr_in fsin;	/* the address of a client	*/
	unsigned int	alen;		/* length of client's address	*/
	int	msock;			/* master server socket		*/
	int	ssock;			/* slave server socket		*/

	switch (argc) {
	case	1:
		break;
	case	2:
		service = argv[1];
		break;
	default:
		errexit("usage: TCPechod [port]\n");
	}

	msock = passivesock(service, "tcp", QLEN);

	(void) signal(SIGCHLD, reaper);

	while (1) {
	  alen = sizeof(fsin);
	  ssock = accept(msock, (struct sockaddr *)&fsin, &alen);

	  if (ssock < 0) {
			if (errno == EINTR)
				continue;
			errexit("accept: %s\n", strerror(errno));
		}

	  switch (fork()) {

	  case 0:		/* child */
	    (void) close(msock);
	    exit(TCPechod(ssock));

	  default:	/* parent */
	    (void) close(ssock);
	    break;

		case -1:
			errexit("fork: %s\n", strerror(errno));
		}
	}
}

/*------------------------------------------------------------------------
 * TCPechod - echo data until end of file
 *------------------------------------------------------------------------
 */
int
TCPechod(int s)
{
	char	buf[BUFSIZ];
	int	cc;
	struct sockaddr_in clientaddr;
	socklen_t len;

	len = sizeof(clientaddr);
	getpeername(s, (struct sockaddr*)&clientaddr, &len);

	/*printf("************** \n");
	printf("Client IP address: %s\n", inet_ntoa(clientaddr.sin_addr));
	printf("Client port      : %d\n", ntohs(clientaddr.sin_port));
	printf("************** \n");*/
	
	while (cc = read(s, buf, sizeof(buf)))
    {
	  if (cc < 0)
	    errexit("echo read: %s\n", strerror(errno));

		if(strlen(buf)==5 && buf[0]=='e' && buf[1]=='x' && buf[2]=='i' && buf[3]=='t')
		{
			printf("\nExiting .....\n");

			exit(0);
		}

		char file[BUFSIZE];
        char file1[BUFSIZE];

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

        int fd;

        fd = open(file, O_RDONLY);
        if (fd == -1)
        {
            char buf2[BUFSIZE]="SORRY!";

            if (write(s, buf2, sizeof(buf2)) < 0)
	            errexit("echo write: %s\n", strerror(errno));
        }
        else
        {
            int read_byte;
            char buf3[BUFSIZE];

            lseek(fd, -bytes, SEEK_END);  
            read_byte = read(fd, buf3, bytes);
            buf3[read_byte] = '\0';

			if (write(s, buf3, sizeof(buf3)) < 0)
	            errexit("echo write: %s\n", strerror(errno));
        }
	  
	    /*printf("Client sent: %s", buf);*/
	}

	return 0;
}

/*------------------------------------------------------------------------
 * reaper - clean up zombie children
 *------------------------------------------------------------------------
 */
void
reaper(int sig)
{
	int	status;

	while (wait3(&status, WNOHANG, (struct rusage *)0) >= 0)
		/* empty */;
}
