#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include<sys/wait.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>

void serviceClient(int);

int main(int argc, char *argv[])
{
  	int sd, client, portNumber, status;
  	struct sockaddr_in servAdd;      // client socket address

 	if(argc != 2)
	{
    		printf("Call model: %s <Port Number>\n", argv[0]);
    		exit(0);
  	}
  	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
    		fprintf(stderr, "Cannot create socket\n");
    		exit(1);
  	}
  	sd = socket(AF_INET, SOCK_STREAM, 0);
  	servAdd.sin_family = AF_INET;
  	servAdd.sin_addr.s_addr = htonl(INADDR_ANY);
  	sscanf(argv[1], "%d", &portNumber);
  	servAdd.sin_port = htons((uint16_t)portNumber);

  	bind(sd, (struct sockaddr *) &servAdd, sizeof(servAdd));
  	listen(sd, 5);
	printf("Waiting for Connection !!\n");

    	client = accept(sd, NULL, NULL);
    	printf("Got a client, start Sharing\n");
    	if(!fork())
	{
      		serviceClient(client);
		close(client);
	}
	else
    		wait(&status);
	exit(0);
}

void serviceClient(int sd)
{
	char message[255];
	char *command;
	char *filename;
	char *details;
        int n,m,fd1,fd2, pid,c,size=0,filesize;
        write(sd, "Enter the command", 18);
        while(1)
	{
	   	if(n=read(sd, message, 255))
	   	{
	     		message[n]='\0';
	     		if(!strcasecmp(message, "quit\n"))
			{
	        		exit(0);
	     		}
			else
			{
				c = 0;
    				char *tokens = strtok(message, " \n");
    				while (tokens != NULL && c < 2)
    				{
        				if (c == 0)
            					command = tokens;
        				else
            					filename = tokens;
        				c++;
        				tokens = strtok(NULL, " \n");
    				}
				if(strcmp(command,"put")==0)
				{
					fd1 = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0777);
					read(sd,&filesize,sizeof(int));

					details = (char *)malloc(filesize);
					if(m=read(sd,details,filesize))
					{
						details[m]='\0';
                                        	write(fd1,details,filesize);
					}
					fprintf(stderr,"%s Successfully Uploaded by the Client\n", filename);
					free(details);
				}
				if(strcmp(command,"get")==0)
                        	{
                        	        //fprintf(stderr,"%s\n",filename);
					fd2 = open(filename,O_RDONLY);
					filesize = lseek(fd2,0L, SEEK_END);
					lseek(fd2,0L, SEEK_SET);
					details = (char *)malloc(filesize);
					read(fd2,details,filesize);
					write(sd,&filesize,sizeof(int));
					write(sd,details,filesize);
					fprintf(stderr,"%s Successfully Downloaded by the Client\n",filename);
					free(details);
				}
			}
	   	}
	}
}

