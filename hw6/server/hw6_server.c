#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <time.h>

#define BUF_SIZE 100
#define MAX_COUNT 20

void error_handling(char* message);
void child_pid(int sig);

int main(int argc, char* argv[])
{
	int serv_sock;
	int clnt_sock;
	
	char message[BUF_SIZE];
	char str1[BUF_SIZE];
	char str2[BUF_SIZE];
	char buf[BUF_SIZE];
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size; 

	pid_t pid;
	struct sigaction act;
	
	int parent_pipe[2];
	int child_pipe[2];
	int randomNum;
	int num, answer;
	int count = 0;
;
	if(argc!=2) 
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	act.sa_handler = child_pid;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;
        sigaction(SIGCHLD, &act, 0);

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
	while(1) {
		clnt_addr_size = sizeof(clnt_addr);
		clnt_sock  = accept(serv_sock, (struct sockaddr*) &clnt_addr, &clnt_addr_size);
		if(clnt_sock == -1)
			error_handling("accept() error");
	
		pipe(parent_pipe);
		pipe(child_pipe);
		pid = fork();

		if(pid != 0) { // parent process
			srand(time(NULL));
			randomNum = ( rand() % 100 ) + 1;

			sprintf(str1, "%d", randomNum);
			write(parent_pipe[1], str1, strlen(str1));
			memset(str1, 0, sizeof(str1));
			while(1) {
				read(child_pipe[0], str1, sizeof(str1));
				printf("%s\n", str1);	
				if(!strcmp(str1, "Correct")||!strcmp(str1, "Wrong"))
					break;
				memset(str1, 0, sizeof(str1));
			}
			
		}
		else { //child process
			read(parent_pipe[0], str2, sizeof(str2)); //read randomNum form parent process
			answer = atoi(str2);
			while(1) {	
				count++;
				memset(message, 0, sizeof(message));
				read(clnt_sock, message, sizeof(str2)); //read num from client
				num = atoi(message);
				write(child_pipe[1], message, strlen(message)); //write num to parent process;
				if(answer == num) {
					usleep(10);			
					count = 0;
					memset(message, 0, sizeof(message));
                	                strcpy(message, "Correct");
                        	        write(clnt_sock, message, strlen(message));
	                                write(child_pipe[1], message, strlen(message));
					close(clnt_sock);
        	                        return 0;
				}
				else if(count==MAX_COUNT) {
        	                        usleep(10); //빠르게 Wrong 넘겨줄 경우 한줄 쓰기가 안되어 0.001초 대기 위에도 동일
					count = 0;
                	                memset(message, 0, sizeof(message));
	                                strcpy(message, "Wrong");
        	                        write(clnt_sock, message, strlen(message));
                	                write(child_pipe[1], message, strlen(message));
					close(clnt_sock);
                        	        return 0;
	                        }
				else if(answer < num)
				{
					memset(message, 0, sizeof(message));
					strcpy(message, "DOWN");
					write(clnt_sock, message, strlen(message));
				}
				else {
					memset(message, 0, sizeof(message));
					strcpy(message, "UP");
        	                        write(clnt_sock, message, strlen(message));
				}
			}
		}
		close(clnt_sock);
                usleep(1000);  //클라이언트 종료후 바로 accept실행하면 오류 나므로 0.1초 대기

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

void child_pid(int sig)
{
	int status;
	pid_t id = waitpid(-1, &status, WNOHANG);

	printf("Child Process Id : %d\n", id);
}
