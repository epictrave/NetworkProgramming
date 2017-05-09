#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/uio.h>

#define BUF_SIZE 100

void error_handling(char* message);

int main(int argc, char* argv[]) 
{
	int sock;
	struct sockaddr_in serv_addr;
	struct iovec vec[3];
	char buf1[BUF_SIZE] = "2012061020 이장훈";
	char buf2[BUF_SIZE] = "Hello";
	char buf3[BUF_SIZE] = "World";
	int str_len; 

	if(argc != 3)
	{
		printf("Usage : %s <IP> <port> \n", argv[0]);
		exit(1);
	}
	
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if(sock == -1)
		error_handling("socket() error");
	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);  
	serv_addr.sin_port = htons(atoi(argv[2]));  

	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error!");

	vec[0].iov_base = buf1;
	vec[0].iov_len = BUF_SIZE;
        vec[1].iov_base = buf2;
        vec[1].iov_len = BUF_SIZE;
        vec[2].iov_base = buf3;
        vec[2].iov_len = BUF_SIZE;

	printf("Frist message : %s\n", buf1);
	printf("Second message : %s\n", buf2);
	printf("Third message : %s\n\n", buf3);
	writev(sock, vec, 3);
	readv(sock, vec,3);
	printf("Frist message from server : %s\n", buf1);
	printf("Second message from server : %s\n", buf2);
	printf("Third message from server : %s\n", buf3);
	sleep(1);
	close(sock);


	return 0;
}

void error_handling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
