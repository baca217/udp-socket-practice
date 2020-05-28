/* Elmer Baca Holguin
 * Network Systems: Lab 1
 * Client side implementation of UDP program
 */ 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>

#define MAXLINE 65535
#define no_file "File Not Found!"
#define error "error"
#define get "get"
#define put "put"
#define ls "ls"
#define delete "delete"
#define exit_text "exit"

void clearBuffs(char *rcv_buf, char *snd_buf, char *input, char *command);

int main(int argc, char *argv[]){
	if(argc < 3)
	{
		printf("not enough arguments\n");
		return 1;
	}
	int sockfd, port = atoi(argv[2]);
	char rcv_buf[MAXLINE], snd_buf[MAXLINE], *split, input[1000], command[50];
	struct sockaddr_in	servaddr;
	socklen_t n, len;

	// creating socket file descriptor
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket creation failed");
                exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));

	// Filling server information
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	len = sizeof(servaddr);	

	while(1)
	{
		// initial usage message
		printf("-------------------------------------------\n");
		printf ("Commands:\nget [file_name]\nput [file_name]");
		printf ("\ndelete [file_name]\nls\nexit");
		printf ("\n\nEnter your command: ");

		if (fgets(input, sizeof input, stdin)) {
			input[strcspn(input, "\n")] = '\0';
		}

		//figuring out what the client input is expecting
		split = strtok(input, " ");
		if(strcmp(split, get) == 0)
		{
			split = strtok(NULL, " ");
			snprintf(snd_buf, sizeof(snd_buf), "%s %s", get, split);
			n = sendto(sockfd, (char *)snd_buf, strlen(snd_buf), 0,
				(struct sockaddr *) &servaddr, 	len);
			n = recvfrom(sockfd, (char *)rcv_buf, MAXLINE,
					MSG_WAITALL, (struct sockaddr *) &servaddr,
						&len);
			rcv_buf[n] = '\0';
			FILE *r_fp;	
			if(strcmp(rcv_buf, "File Not Found!") == 0)
			{
				printf("File was not found!\n");
			}
			else
			{
				if(r_fp = fopen(split, "w"))
				{
					fwrite(rcv_buf, 1, n, r_fp);
					fclose(r_fp);
					printf("File %s was received\n",split);
				}
				else
				{
					printf("File %s failed to be created\n",split);
				}
			}
		}
		else if(strcmp(split, put) == 0)
		{
			FILE *s_fp;
			split = strtok(NULL, " ");
			s_fp = fopen(split, "r");
			if(s_fp)
			{
				struct stat stats;
				stat(split, &stats);
				snprintf(snd_buf, sizeof(snd_buf), "%s %s", put, split);
				int cn = sendto(sockfd, snd_buf, strlen(snd_buf), 0,
						(const struct sockaddr*) &servaddr, len);

				memset(snd_buf,'\0',MAXLINE);
                 		int size = fread(snd_buf, 1, stats.st_size, s_fp);
				cn = sendto(sockfd, (char*)snd_buf, size, 0,
						(const struct sockaddr*) &servaddr, len);
				fclose(s_fp);
				printf("File %s was sent\n",split);
			}
			else
			{
				printf("file \"%s\" failed to open or was not found\n",split);
			}
		}
		else if(strcmp(split, ls) == 0)
		{	
			n = sendto(sockfd, (char *)ls, strlen(ls), 0,
				(struct sockaddr *) &servaddr, 	len);
			n = recvfrom(sockfd, (char *)rcv_buf, MAXLINE,
					MSG_WAITALL, (struct sockaddr *) &servaddr,
						&len);	
			printf("%s\n", rcv_buf);
		}
		else if(strcmp(split, delete) == 0)
		{
			split = strtok(NULL, " ");
			snprintf(snd_buf, sizeof(snd_buf), "%s %s", delete, split);
			n = sendto(sockfd, (char *)snd_buf, strlen(snd_buf), 0,
				(struct sockaddr *) &servaddr, 	len);
			n = recvfrom(sockfd, (char *)rcv_buf, MAXLINE,
					MSG_WAITALL, (struct sockaddr *) &servaddr,
						&len);
			rcv_buf[n] = '\0';
			printf("%s\n",rcv_buf);
		}
		else if(strcmp(split, exit_text) == 0)
		{
			n = sendto(sockfd, (char *)split, strlen(split), 0,
				(struct sockaddr *) &servaddr, 	len);	
			close(sockfd);
			return 0;
		}
		else
		{
			printf("command was not recognized\n");
		}
		clearBuffs(rcv_buf, snd_buf, input, command);
	}
}

void clearBuffs(char *rcv_buf, char *snd_buf, char *input, char *command)
{
	memset(rcv_buf, '\0', MAXLINE);
	memset(snd_buf, '\0', MAXLINE);
	memset(input, '\0', 1000);
	memset(command, '\0', 50);
}
