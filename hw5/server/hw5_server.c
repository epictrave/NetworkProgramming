#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <sys/types.h>

#define BUF_SIZE 1000
#define OPTION_NUM 15
void error_handling(char* message);

int main(int argc, char* argv[])
{
	
	int serv_sock, clnt_sock;
	int option, optlen;
	int option_name[OPTION_NUM] = { SO_SNDBUF, SO_DONTROUTE, SO_REUSEADDR, SO_OOBINLINE, SO_BROADCAST, 
		SO_TYPE, SO_ERROR, SO_RCVBUF, SO_KEEPALIVE, IP_MULTICAST_IF, IP_MULTICAST_LOOP, 
		IP_MULTICAST_TTL, IP_TOS, IP_TTL, TCP_NODELAY };
	int state;
	struct sockaddr_in serv_adr, clnt_adr;

	char option_str[OPTION_NUM][BUF_SIZE] = {"SO_SNDBUF", "SO_DONTROUTE", "SO_REUSEADDR", "SO_OOBINLINE", "SO_BROADCAST",
		"SO_TYPE", "SO_ERROR", "SO_RCVBUF", "SO_KEEPALIVE", "IP_MULTICAST_IF", "IP_MULTICAST_LOOP", 
		"IP_MULTICAST_TTL", "IP_TOS", "IP_TTL", "TCP_NODELAY" };
	int i;
	char message[BUF_SIZE];
	socklen_t clnt_adr_sz;
	int readlen;
	if(argc !=2) 
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	
	if(serv_sock == -1)
		error_handling("socket() error");
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) ==-1)
		error_handling("bind() error");

	if(listen(serv_sock, 5) ==-1)
		error_handling("listen() error");

	optlen = sizeof(option);
	printf("Server\n");
	for(i=0; i<OPTION_NUM; i++) 
	{	
		if(i<9)
			state = getsockopt(serv_sock, SOL_SOCKET, option_name[i], &option, &optlen);
		else if(i==OPTION_NUM -1)
			state =  getsockopt(serv_sock, IPPROTO_TCP, option_name[i], &option, &optlen);
		else
			state = getsockopt(serv_sock, IPPROTO_IP, option_name[i], &option, &optlen);
		if(state)
			error_handling("getsock() error");
		printf("%s : %d\n", option_str[i], option); 
	}
	
	clnt_adr_sz = sizeof(clnt_adr);
	clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);

	if(clnt_sock == -1)
		error_handling("accept() error");

	while(1)
	 {
		readlen = read(clnt_sock, &message, sizeof(message));
		if(readlen ==-1)
			error_handling("read() error");
		else if(readlen == 0)
			break;
		message[readlen] ='\0';
		printf("%s", message);
	}
	close(clnt_sock);	
	close(serv_sock);
	return 0;
}

void error_handling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);	
}
