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
	int sock;
	char buf[BUF_SIZE];
	char str1[] = "Hello\n";
	char str2[] = "Bye!\n";
	struct sockaddr_in serv_adr;

	FILE *readfp, *writefp, *filefp;
	
	int fd;

	if(argc!=4) {
		printf("Usage : %s <IP> <Port> <filename>\n", argv[0]);
		exit(1);
	}
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if(sock ==-1)
		error_handling("socket() error");
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_adr.sin_port = htons(atoi(argv[2]));
	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("connect() error");
	
	readfp = fdopen(sock, "r");
	writefp = fdopen(sock, "w");

	fd = open(argv[3], O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU|S_IRWXG|S_IRWXO);
	filefp = fdopen(fd, "w");

	fputs(str1, writefp);
	fflush(writefp);
	while(1)
	{	
		if(fgets(buf, sizeof(buf), readfp) == NULL)
			break;
		fputs(buf, filefp);
		fflush(filefp);
	}
	fputs(str2, writefp);
	fflush(writefp);
	fclose(filefp);
	fclose(writefp);
	fclose(readfp);

	return 0;
}

void error_handling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);

}
