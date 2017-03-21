#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZE 100
void error_handling(char* message);

int main(int argc, char* argv[])
{
	int serv_sock;
	int clnt_sock;
	int fd;
	
	char buf[BUF_SIZE]="";
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;  //int는 환경마다 다르게 인식할 수 있으므로 socklent_t로 변경

	int str_len;	
	if(argc!=3) 
	{
		printf("Usage : %s <port> <filename>\n", argv[0]);
		exit(1);
	}
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock == -1)
		error_handling("socket() error");
	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) ==-1)
		error_handling("bind() error");
	
	if(listen(serv_sock, 5) == -1)
		error_handling("listen() error");
	
	clnt_addr_size = sizeof(clnt_addr);
	clnt_sock  = accept(serv_sock, (struct sockaddr*) &clnt_addr, &clnt_addr_size);
	if(clnt_sock == -1)
		error_handling("accept() error");
	
	fd = open(argv[2], O_RDONLY);
	if(fd == -1)
		error_handling("open() error");
	str_len = read(fd,buf, sizeof(buf));  //쓰레기값이 아닌 실제 읽은 값길이만큼 반환
	if(str_len == -1)
		error_handling("read() error");

	write(clnt_sock, buf, str_len);  //정보가 든 길이만큼 보냄
	
	close(clnt_sock);
	close(serv_sock);
	close(fd);
	return 0;
}

void error_handling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
