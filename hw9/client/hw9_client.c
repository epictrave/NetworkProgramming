#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#define BUF_SIZE 1024
#define EPOLL_SIZE 4
#define RECIEVE_BUF_SIZE 4

void error_handling(char *message);

int main(int argc, char* argv[])
{
	int sock;
	char message[RECIEVE_BUF_SIZE+1];
	char str1[BUF_SIZE];
	char str2[BUF_SIZE];
	char str3[BUF_SIZE];
	int str_len, i;
	FILE *readfp, *writefp;

	struct sockaddr_in serv_adr;

	struct epoll_event *ep_events;
	struct epoll_event event;
	int epfd, event_cnt;

	if(argc != 3) 
	{
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if(sock==-1)
		error_handling("socket() error");
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_adr.sin_port = htons(atoi(argv[2]));

	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("connect() error");
	
	readfp = fdopen(sock, "r");
	writefp = fdopen(dup(sock), "w");

	fgets(str1, BUF_SIZE, stdin);
	fgets(str2, BUF_SIZE, stdin);
	fgets(str3, BUF_SIZE, stdin);
	/*
	write(sock, str1, strlen(str1));
	write(sock, str2, strlen(str2));
	write(sock, str3, strlen(str3));
	*/
	fputs(str1, writefp);
	fputs(str2, writefp);
	fputs(str3, writefp);
	fflush(writefp);
	
	shutdown(fileno(writefp), SHUT_WR);
	fclose(writefp);

	epfd = epoll_create(EPOLL_SIZE);
	ep_events = malloc(sizeof(struct epoll_event)*EPOLL_SIZE);

	event.events = EPOLLIN;
	event.data.fd = sock;
	epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &event);
	while(1)
	{
		memset(message, 0, sizeof(message));	
		event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);	
		if(event_cnt ==-1)
		{
			puts("epoll_wait() error");
			break;
		}
		puts("return epoll_wait()");
	
		for(i=0; i<event_cnt; i++)
		{
			if(ep_events[i].data.fd == sock)
			{
				str_len = read(ep_events[i].data.fd, message, RECIEVE_BUF_SIZE-1);
				message[str_len] = 0;
				if(str_len ==0)
				{
					epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
					close(ep_events[i].data.fd);
					break;
				}
				else
					printf("%s\n", message);
			}

		}
		if(strlen(message) == 0)
			break;	
	}

	fclose(readfp);	
	return 0;	
}

void error_handling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);

}
