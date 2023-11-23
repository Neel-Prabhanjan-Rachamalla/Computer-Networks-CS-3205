/* UDPechod.c - main */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

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

		printf("\nReceived from Client : %s",buf);

		printf("Sending to Client : %s",buf);

		(void) sendto(sock, buf, sizeof(buf), 0,
				(struct sockaddr *)&fsin, sizeof(fsin));
	}
}
