//Group member names:
//1.Sreejita Saha  21CS30052
//2.Ratan Junior  21CS30041
//Assignment-3

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <regex.h>
#include <pthread.h> // Include the pthread library

#define MAX_BUFFER_SIZE 1024

// Function to handle client connections
void* handleClient(void* arg) {
    int clientSocket = *(int*)arg;
    char buffer[MAX_BUFFER_SIZE];
    char username[MAX_BUFFER_SIZE];
    memset(username, 0, sizeof(username));

    recv(clientSocket,buffer, sizeof(buffer), 0);
    // printf("%s\n",buffer);
    if(strncmp(buffer,"MAIL FROM:",10)!=0)
    {
        memset(buffer,0,sizeof(buffer));
        sprintf(buffer,"503 Bad sequence of commands");
        send(clientSocket,buffer,strlen(buffer),0);
        memset(buffer,0,sizeof(buffer));
        // recv(newsockfd,buffer,sizeof(buffer),0);
    }
    else
    {
        memset(buffer,0,sizeof(buffer));
        sprintf(buffer,"250 %s...sender ok",buffer+12);
        
        send(clientSocket,buffer,strlen(buffer),0);
        memset(buffer,0,sizeof(buffer));
        // recv(newsockfd,buffer,sizeof(buffer),0);
    }

    recv(clientSocket,buffer, sizeof(buffer), 0);
    // printf("%s\n",buffer);
    if(strncmp(buffer,"RCPT TO:",8)!=0)
    {
        memset(buffer,0,sizeof(buffer));
        sprintf(buffer,"503 Bad sequence of commands");
        send(clientSocket,buffer,strlen(buffer),0);
        memset(buffer,0,sizeof(buffer));
        // recv(newsockfd,buffer,sizeof(buffer),0);
    }
    else
    {
        strcpy(username,buffer+10);
        memset(buffer,0,sizeof(buffer));
        sprintf(buffer,"250 root...Recipient ok");
        
        
        send(clientSocket,buffer,strlen(buffer),0);
        memset(buffer,0,sizeof(buffer));
        // recv(newsockfd,buffer,sizeof(buffer),0);
    }

    recv(clientSocket,buffer, sizeof(buffer), 0);
    
    if(strncmp(buffer,"DATA",4)!=0)
    {
        memset(buffer,0,sizeof(buffer));
        sprintf(buffer,"503 Bad sequence of commands");
        send(clientSocket,buffer,strlen(buffer),0);
        memset(buffer,0,sizeof(buffer));
        // recv(newsockfd,buffer,sizeof(buffer),0);
    }
    else
    {
        memset(buffer,0,sizeof(buffer));
        sprintf(buffer,"354 Start mail input; end with <CRLF>.<CRLF>");
        
        send(clientSocket,buffer,strlen(buffer),0);
        memset(buffer,0,sizeof(buffer));
        // recv(newsockfd,buffer,sizeof(buffer),0);
    }

    char user[100];
    int i;
    for(i=0;username[i]!='@';i++)
    {
        user[i]=username[i];
    }
    user[i]='\0';
    

    // Read the recipient's username from the client
    
    // Create a subdirectory with the recipient's username
    char directoryName[MAX_BUFFER_SIZE];
    snprintf(directoryName, sizeof(directoryName), "./%s", user);
    mkdir(directoryName, 0777);

    // Open a file to store the received mail
    char filePath[MAX_BUFFER_SIZE];
    snprintf(filePath, sizeof(filePath), "%s/mail.txt", directoryName);
    int fileDescriptor = open(filePath, O_WRONLY | O_CREAT | O_APPEND, 0666);
    if (fileDescriptor < 0) {
        perror("Opening file failed");
        exit(EXIT_FAILURE);
    }

    // Read the mail content from the client and write it to the file
    // char buffer[MAX_BUFFER_SIZE];
    ssize_t bytesRead;
    int flag=0;
        while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer),0)) > 0) {
        
        char *p=buffer+strlen(buffer)-5;
        
        write(fileDescriptor, buffer, bytesRead);
        if(strncmp(buffer+strlen(buffer)-5,"\r\n.\r\n",5)==0)
        {
            
            memset(buffer,0,sizeof(buffer));
            sprintf(buffer,"250 OK Message accepted for delivery");
            int byte=send(clientSocket,buffer,strlen(buffer),0);
            
            memset(buffer,0,sizeof(buffer));
            break;
        }
    }

    

    // Close the file and client socket
    close(fileDescriptor);
    // close(clientSocket);

    // pthread_exit(NULL);
}

int main(int argc, char* argv[]) {

    char buffer[MAX_BUFFER_SIZE];
    // Create a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int newsockfd;
    socklen_t clilen;
    struct sockaddr_in cli_addr;

    if(argc!=2)
    {
        printf("Add port\n");
        exit(1);
    }
    int port=atoi(argv[1]);


    // Bind the socket to a specific address and port
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port); // SMTP port

    if (bind(sockfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(sockfd, 5) < 0) {
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Accept a client connection
        clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr,
					&clilen) ;

		if (newsockfd < 0) {
			printf("Accept error\n");
			exit(0);
		}

        // Create a new thread to handle the client connection
        // pthread_t thread;
        // if (pthread_create(&thread, NULL, handleClient, &clientSocket) != 0) {
        //     perror("Thread creation failed");
        //     exit(EXIT_FAILURE);
        // }

        // // Detach the thread to allow it to run independently
        // pthread_detach(thread);
        if(fork()==0)
        {
            close(sockfd);

            char hostname[50];
            int result = getnameinfo((struct sockaddr *)&serverAddress, sizeof(serverAddress), hostname, 50, NULL, 0, 0);

            if (result != 0) {
                perror("Error getting hostname");
                exit(EXIT_FAILURE);
            }
            
            fflush(stdout);

            memset(buffer,0,sizeof(buffer));

            sprintf(buffer,"220 <%s> Service Ready",hostname);
            send(newsockfd,buffer,strlen(buffer),0);
            memset(buffer,0,sizeof(buffer));
            
            fflush(stdout);
            int rec = recv(newsockfd,buffer,sizeof(buffer),0);
            //print if it shows error
            
            fflush(stdout);
            if(rec<0)
            {
                perror("Error in receiving");
                exit(EXIT_FAILURE);
            }
           
            if(strncmp(buffer,"HELO",4)!=0)
            {
                memset(buffer,0,sizeof(buffer));
                sprintf(buffer,"503 Bad sequence of commands");
                send(newsockfd,buffer,strlen(buffer),0);
                memset(buffer,0,sizeof(buffer));
                // recv(newsockfd,buffer,sizeof(buffer),0);
            }
            else
            {
                
                memset(buffer,0,sizeof(buffer));
                sprintf(buffer,"250 OK Hello %s",hostname);
                send(newsockfd,buffer,strlen(buffer),0);
                memset(buffer,0,sizeof(buffer));
                // recv(newsockfd,buffer,sizeof(buffer),0);
            }
            
            

            handleClient(&newsockfd);

            recv(newsockfd,buffer, sizeof(buffer), 0);
            if(strncmp(buffer,"QUIT",4)!=0)
            {
                memset(buffer,0,sizeof(buffer));
                sprintf(buffer,"503 Bad sequence of commands");
                send(newsockfd,buffer,strlen(buffer),0);
                memset(buffer,0,sizeof(buffer));
                // recv(newsockfd,buffer,sizeof(buffer),0);
            }
            else
            {
                memset(buffer,0,sizeof(buffer));
                sprintf(buffer,"221 %s closing connection",hostname);
                send(newsockfd,buffer,strlen(buffer),0);
                
                memset(buffer,0,sizeof(buffer));
                // recv(newsockfd,buffer,sizeof(buffer),0);
            }
            close(newsockfd);
            exit(0);


        }
        close(newsockfd);
    }

    // Close the server socket
    // close(sockfd);

    return 0;
}


