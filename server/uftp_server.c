/* Elmer Baca Holguin
 * Network Systems Lab 1
 * Server Side implementation of UDP program
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ctype.h>

#define MAXLINE 1024
#define MAX_SIZE 65535 //this is max size of udp packet in bytes
#define no_file "File Not Found!"
#define write_fail "File failed to be created"
#define del_suc "Delete Succeeded"
#define del_fail "Delete failed"

int sendFile(FILE *fp, char* buf, off_t size);
void clearBuffs(char *rcv_buf, char *snd_buff);


int main(int argc, char *argv[]) 
{
	int sockfd, port, size, opt =1, n_socket;
	char rcv_buf[MAX_SIZE], snd_buf[MAX_SIZE], conf[] = "exit", *split, *next, cont[] = "cont";
	struct sockaddr_in	servaddr, cliaddr;
	socklen_t len, n;

	//getting the port number from command line
	if (argc > 1)
	{
		if((port = atoi(argv[1])) == 0)
		{
			printf("please enter a number\n");
			exit(-1);
		}
		if(port < 5000 || port > 65535)
		{
			printf("please enter a port number greater than 5000 and less than 65535\n");
			exit(-1);
		}
	}
	else
	{
		printf("please enter a port number\n");
		exit(-1);
	}

	// creating socket file descriptor
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == 0) 
	{
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}	
	
	// just zeroing out our structs
	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));

	// filling in server information
	servaddr.sin_family = AF_INET;	// IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY; //Any IP address can connect
	servaddr.sin_port = htons(port);

	// bind the socket with the server address
	if(bind(sockfd, (const struct sockaddr *)&servaddr,sizeof(servaddr)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	len = sizeof(cliaddr); 

	while(1)
	{
		clearBuffs(rcv_buf, snd_buf);
		printf("The server is ready to receive\n");
		n = recvfrom(sockfd, (char *)rcv_buf, MAXLINE,
				0, (struct sockaddr *)&cliaddr,
					&len);
		
		rcv_buf[n] = '\0';
		printf("rec_buf: %s\n",rcv_buf);
		//parsing the string sent to us
		split = strtok(rcv_buf, " ");
		if(strcmp(split, "get") == 0) //get command
		{
			printf("gonna get a file\n");
			next = strtok(NULL, " ");
			FILE *s_fp = fopen(next, "r");
			if(s_fp) //checking if file opened
			{
				struct stat stats;
				stat(next, &stats);
				size = sendFile(s_fp, snd_buf, stats.st_size);
				//sending the file info
				int cn = sendto(sockfd, (char*)snd_buf, size ,0 ,
						(const struct sockaddr*) &cliaddr, len);
				fclose(s_fp);
				printf("file \"%s\" was sent to the client\n", next);
			}
			else
			{
				//sending error message
				int cn = sendto(sockfd, (char*)no_file, strlen(no_file), 0,
						(const struct sockaddr*) &cliaddr, len);
				printf("file \"%s\" was not found\n", next);
			}
		}
		else if(strcmp(split, "put") == 0)
		{
			split = strtok(NULL, " ");
			FILE *r_fp;	
			r_fp = fopen(split, "w");
			memset(rcv_buf, '\0', MAX_SIZE);
			n = recvfrom(sockfd, (char *)rcv_buf, MAX_SIZE, 0,
					(struct sockaddr *) &servaddr, &len);
			if(r_fp)
			{	
				fwrite(rcv_buf, 1, n, r_fp);
				fclose(r_fp);
				printf("file was stored\n");
			}
			else
			{
				printf("file failed to be created\n");
			}
		} 
		else if(strcmp(split, "delete") == 0)
		{
			split = strtok(NULL, " ");
			if(remove(split) == 0)
			{
				printf("file: %s was deleted\n", split);
				int cn = sendto(sockfd, (const char*)del_suc, strlen(del_suc),0,
						(const struct sockaddr*) &cliaddr, len);
			}
			else
			{
				printf("file: %s doesn't exist, no delete\n", split);
				int cn = sendto(sockfd, (const char*)del_fail, strlen(del_fail),0,
						(const struct sockaddr*) &cliaddr, len);
			}
		}
		else if(strcmp(split, "ls") == 0)
		{
			printf("gonna ls!\n");
			DIR *d;
			struct dirent *dir;
			d = opendir(".");
			if(d)
			{
				while((dir = readdir(d)) != NULL)
				{
					strcat(snd_buf, dir->d_name);
					strcat(snd_buf, "\n");
				}
				closedir(d);
				int cn = sendto(sockfd, (const char*)snd_buf, strlen(snd_buf),0,
						(const struct sockaddr*) &cliaddr, len);
			}

		}
		else if(strcmp(split, "exit") == 0)
		{
			printf("gonna exit\n");
			int cn = sendto(sockfd, (const char*)conf, strlen(conf),0, 
					(const struct sockaddr *) &cliaddr,len);
			close(sockfd);
			return 0;
		}
		else
		{
			printf("command not recongnized\n");
			printf("token: %s\n", split);
		}
		printf("\n");
			
	}
}

int sendFile(FILE *fp, char* buf, off_t f_size)
{
	if(fp == NULL)
	{
		//file failed to open, send error message 
		strcpy(buf, no_file);
		int len = strlen(no_file);
		buf[len] = '\0';
		return strlen(buf);
	}
	else
	{
		//is this the best way to transfer a file?
		int num = fread(buf, 1, f_size, fp);
		return num;
	}
}

void clearBuffs(char *rcv_buf, char *snd_buf)
{
	memset(rcv_buf, '\0', MAX_SIZE);
	memset(snd_buf, '\0', MAX_SIZE);
}
