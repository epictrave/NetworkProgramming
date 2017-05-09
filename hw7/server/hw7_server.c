#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/uio.h>

#define BUF_SIZE 100

void error_handling(char* message);
int StrUpr( char *str );

int main(int argc, char* argv[])
{
	int serv_sock, clnt_sock;
	
	struct iovec vec[3];
	char buf1[BUF_SIZE] = {0,};
	char buf2[BUF_SIZE] = {0,};
	char buf3[BUF_SIZE] = {0,};
	char temp[BUF_SIZE] = {0,};
	
	int str_len;
	struct sockaddr_in serv_addr, clnt_addr;
	socklen_t clnt_adr_size; 

	struct timeval timeout;
	fd_set reads, cpy_reads;
	int fd_max, fd_num, i;

	if(argc!=2) 
	{
		printf("Usage : %s <port>\n", argv[0]);
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
	
	FD_ZERO(&reads);
	FD_SET(serv_sock, &reads);
	fd_max = serv_sock;
	while(1)
	{
		cpy_reads = reads;
		timeout.tv_sec = 5;
		timeout.tv_usec = 5000;

		if((fd_num = select(fd_max+1, &cpy_reads, 0, 0, &timeout))==-1)
		{
			printf("aaa");	
			break;
		}
		if(fd_num==0)
			continue;
		for(i = 0; i<fd_max+1; i++)
		{
			if(FD_ISSET(i, &cpy_reads))
			{
				if(i == serv_sock)
				{
					clnt_adr_size = sizeof(clnt_adr_size);
					clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_adr_size);
				        if(clnt_sock==-1)
					                error_handling("accept() error");
					FD_SET(clnt_sock, &reads);
					if(fd_max<clnt_sock)
						fd_max = clnt_sock;
					printf("connected client : %d\n",clnt_sock);
				}
				else
				{
					vec[0].iov_base = buf1;
					vec[0].iov_len = BUF_SIZE;
					vec[1].iov_base = buf2;
					vec[1].iov_len = BUF_SIZE;
					vec[2].iov_base = buf3;
					vec[2].iov_len = BUF_SIZE;
					
					str_len = readv(i, vec, 3);
					
					StrUpr(buf1);
					StrUpr(buf2);
					StrUpr(buf3);
					
					temp[0] = buf1[0];
					temp[1] = buf2[0];
					temp[2] = buf3[0];
					temp[3] = 0;	
					
					memset(buf1, 0, sizeof(buf1));
					strcpy(buf1, temp);
					memset(temp, 0, sizeof(temp));
					
					writev(i, vec, 3);
					if(str_len == 0)
					{
						FD_CLR(i, &reads);
						printf("closed client : %d\n", i);
						close(i);
					}
				}
			}
		}
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
int StrUpr( char *str ) 
{ 
    int loop = 0; 
    while( str[loop] != '\0' ) 
    {
       if( str[loop] >= 97 && str[loop] <= 122 )
            str[loop] = str[loop] - 32;
	loop++;
    }
    return loop; 
}
