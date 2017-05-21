#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>

#define BUF_SIZE 1024

void error_handling(char *message);

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	FILE *readfp, *writefp, *filefp;
	int fd, i;
	struct sockaddr_in serv_adr, clnt_adr;
	socklen_t clnt_adr_sz;
	char buf[BUF_SIZE] = {0,};

	if(argc!=3)
	{
		printf("Usage : %s <port> <filename>\n", argv[0]);
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
	if(listen(serv_sock, 5) ==-1)
		error_handling("listen() error");
	if(fd ==-1)
		error_handling("file open() error");

	clnt_adr_sz = sizeof(clnt_adr);

	for(i=0; i<5;i++)
	{	
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
		if(clnt_sock ==-1)
			error_handling("accept() error");
		readfp = fdopen(clnt_sock, "r");
		writefp = fdopen(dup(clnt_sock), "w");

		fd = open(argv[2], O_RDONLY);
		filefp = fdopen(fd, "r");

		fgets(buf, sizeof(buf), readfp);
                fputs(buf, stdout);
		fflush(stdout);
		while(1)
		{
			if(fgets(buf, sizeof(buf), filefp)==NULL)
				break;
			fputs(buf, writefp);
			fflush(writefp);
		}
		
		shutdown(fileno(writefp), SHUT_WR);
		fclose(writefp);
	
		fgets(buf, sizeof(buf), readfp);
		fputs(buf, stdout);
		fflush(stdout);
		fclose(readfp);
		fclose(filefp);
	}
	close(serv_sock);
	return 0;
}

void error_handling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);

}
