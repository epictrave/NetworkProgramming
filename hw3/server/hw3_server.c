#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>

#define BUF_SIZE 1024
#define OPSZ 4
#define CLIENT_NUM 6

void error_handling(char *message);
int calculate(int opnum, int opnds[], char oprator, int fd);
void newline(int fd);
int power(int a, int b);

int main(int argc, char* argv[])
{
	int serv_sock, clnt_sock;
	char opinfo[BUF_SIZE];
	char message[BUF_SIZE];
	char* val;
	int result, opnd_cnt, i;
	int recv_cnt, recv_len;
	struct sockaddr_in serv_adr, clnt_adr;
	socklen_t clnt_adr_sz;
	int fd, str_len;
	
	if(argc!=4)
	{
		printf("Usage : %s <port> <read filename> <write filename>\n", argv[0]);
		exit(1);
	}

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock==-1)
		error_handling("socket() error");
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	if(listen(serv_sock, 5) == -1)
		error_handling("listen() error");
	clnt_adr_sz = sizeof(clnt_adr);

	fd = open(argv[2], O_RDONLY);
	
	if(fd == -1)
		error_handling("open() error");
	
	str_len = read(fd, message, sizeof(message));
	
	if(str_len == -1)
		error_handling("read() error");
	
	close(fd);

	fd = open(argv[3], O_CREAT|O_TRUNC|O_WRONLY, S_IWUSR|S_IRUSR|S_IRGRP|S_IROTH);
	if(fd == -1)
		error_handling("open() error");
	if(write(fd, message, str_len) == -1)
		error_handling("write() error");
	newline(fd);
	for(i=0; i<CLIENT_NUM; i++) 
	{
		opnd_cnt = 0;
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
		if(clnt_sock == -1)
			error_handling("accept() error");
		read(clnt_sock, &opnd_cnt, 1);

		recv_len = 0;
		while((opnd_cnt*OPSZ+1)>recv_len)
		{
			recv_cnt = read(clnt_sock, &opinfo[recv_len], BUF_SIZE-1);
			recv_len += recv_cnt;
			printf("%c", opinfo[recv_len]);

		}
		
		result = calculate(opnd_cnt, (int*)opinfo, opinfo[recv_len-1], fd);
		write(clnt_sock, (char*)&result, sizeof(result));
		
		close(clnt_sock);
	}
	close(fd);
	close(serv_sock);
	return 0;
}

void error_handling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

int calculate(int opnum, int opnds[], char op, int fd)
{
	int result = opnds[0], i;
	char message[BUF_SIZE];
		
	switch(op)
	{
		case '+' :
			for(i=1; i<opnum; i++) result += opnds[i];
			break;
		case '-' :
			for(i=1; i<opnum; i++) result -= opnds[i];
                        break;
		case '*' :
			for(i=1; i<opnum; i++) result *= opnds[i];
		        break;
		case '%' :
			for(i=1; i<opnum; i++) result %= opnds[i];
			break;
		case '/' :
			for(i=1; i<opnum; i++) result /= opnds[i];
			break;
		case '^' :
			for(i=1; i<opnum; i++) result = power(result, opnds[i]);
			break;
	}
	for(i=0; i<opnum; i++) 
	{
		memset(message, 0, sizeof(message));
		sprintf(message, "%d\n", opnds[i]);
		write(fd, message, strlen(message));
	}
	memset(message, 0, sizeof(message));
	sprintf(message, "%c\n", op);
	write(fd, message, strlen(message));
	memset(message, 0, sizeof(message));
	sprintf(message, "%d\n\n", result);
	write(fd, message, strlen(message));
	return result;
}

void newline(int fd)
{
	char newline = '\n';
	if(write(fd, &newline, sizeof(newline))==-1)
		error_handling("write() error");
}

int power(int a, int b)
{
	if(b==0)
		return 1;
	else
		return a * power(a, b-1);
}
