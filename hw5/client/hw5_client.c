#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <netdb.h>

#define BUF_SIZE 1000
#define OPTION_NUM 15
void error_handling(char* message);

int main(int argc, char* argv[])
{
	
	int sock;
	int option, optlen;
	int option_name[OPTION_NUM] = { SO_SNDBUF, SO_DONTROUTE, SO_REUSEADDR, SO_OOBINLINE, SO_BROADCAST, 
		SO_TYPE, SO_ERROR, SO_RCVBUF, SO_KEEPALIVE, IP_MULTICAST_IF, IP_MULTICAST_LOOP, 
		IP_MULTICAST_TTL, IP_TOS, IP_TTL, TCP_NODELAY };
	int state;
	struct sockaddr_in serv_adr;

	char option_str[OPTION_NUM][BUF_SIZE] = {"SO_SNDBUF", "SO_DONTROUTE", "SO_REUSEADDR", "SO_OOBINLINE", "SO_BROADCAST",
		"SO_TYPE", "SO_ERROR", "SO_RCVBUF", "SO_KEEPALIVE", "IP_MULTICAST_IF", "IP_MULTICAST_LOOP", 
		"IP_MULTICAST_TTL", "IP_TOS", "IP_TTL", "TCP_NODELAY" };
	int i;
	char message[BUF_SIZE];

	struct hostent* host;
	if(argc !=3) 
	{
		printf("Usage : %s <Domain name> <port>\n", argv[0]);
		exit(1);
	}

	sock = socket(PF_INET, SOCK_STREAM, 0);
	
	if(sock == -1)
		error_handling("socket() error");

	host = gethostbyname(argv[1]);
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr = *(struct in_addr*)host->h_addr_list[0];
	serv_adr.sin_port = htons(atoi(argv[2]));
	
	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) ==-1)
		error_handling("connect() error");


	optlen = sizeof(option);
	strcpy(message, "\nClient\n");
	if(write(sock, message, strlen(message)) ==-1)
		error_handling("write() error");
	for(i=0; i<OPTION_NUM; i++) 
	{	
		if(i<9)
			state = getsockopt(sock, SOL_SOCKET, option_name[i], &option, &optlen);
		else if(i== OPTION_NUM - 1)
			state =  getsockopt(sock, IPPROTO_TCP, option_name[i], &option, &optlen);
		else
			state = getsockopt(sock, IPPROTO_IP, option_name[i], &option, &optlen);
		if(state)
			error_handling("getsock() error");
		
		memset(message, 0, sizeof(message));
		sprintf(message, "%s : %d\n", option_str[i], option);	
		write(sock, message, strlen(message));


	}


	close(sock);	
	return 0;
}

void error_handling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);	
}
