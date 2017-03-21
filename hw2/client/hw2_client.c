#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
void error_handling(char* message);

int main(int argc, char* argv[]) 
{
	int sock;
	int fd;
	struct sockaddr_in serv_addr;
	char message[100]="";
	int str_len=0;
	int read_len=0;
	int idx=0;
	
	if(argc != 4)
	{
		printf("Usage : %s <IP> <port> <filename>\n", argv[0]);
		exit(1);
	}
	
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if(sock == -1)
		error_handling("socket() error");
	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	if(!inet_aton(argv[1], &serv_addr.sin_addr))
		error_handling("Conversion error");  //inet_addr()대신 inet_aton()을 사용
	serv_addr.sin_port = htons(atoi(argv[2]));  

	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error!");
	
	while(read_len =read(sock, &message[idx++], 1))
	{
		if(read_len==-1)
			error_handling("read() error");
		str_len +=read_len;
	}
	printf("Function read call count : %d\n", str_len);
	fd = open(argv[3], O_CREAT|O_TRUNC|O_WRONLY, S_IWUSR|S_IRUSR|S_IRGRP|S_IROTH);
	if(fd == -1)
		error_handling("open() error");

	if(write(fd, message, str_len)==-1)
		error_handling("write() error");

	close(sock);
	close(fd);
	return 0;
}

void error_handling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
