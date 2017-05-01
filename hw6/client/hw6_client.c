#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 100
#define MAX_COUNT 20

void error_handling(char* message);


int main(int argc, char* argv[]) 
{
	int sock;
	struct sockaddr_in serv_addr;
	char number[BUF_SIZE];
	char message[BUF_SIZE];
	int i, num=0;
		
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
	for(i=0; i<MAX_COUNT; i++) {
	
		printf("insert number (1 to 100) : ");
                scanf("%d",&num);
		if(num >=1 && num<=100) {
			memset(number, 0, sizeof(number));
			sprintf(number, "%d", num);
			
			write(sock, number, strlen(number));
			read(sock, message, sizeof(message));
			printf("response : %s\n", message);
			if(!strcmp(message, "Correct") || !strcmp(message, "Wrong"))
				break;
			memset(message, 0, sizeof(message));

		}
		else {
			printf("Insert only 1 to 100\n");
			i--;
		}
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
