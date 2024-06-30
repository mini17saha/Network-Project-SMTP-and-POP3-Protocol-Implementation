//pop3 server code to manage mails and login 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <regex.h>
#include <fcntl.h>

// #include <pthread.h> // Include the pthread library

#define MAX_BUFFER_SIZE 1024

int isValidUser(char* user_pass, int flag) {
    FILE *file = fopen("user.txt","r");
    char buffer[MAX_BUFFER_SIZE];
    char username[MAX_BUFFER_SIZE];
    char password[MAX_BUFFER_SIZE];
    memset(username, 0, sizeof(username));
    memset(password, 0, sizeof(password));
    memset(buffer, 0, sizeof(buffer));
    int n;
    // printf("%s %d\n",user_pass,strlen(user_pass));
    while((fgets(buffer, sizeof(buffer),file)) > 0) {
        int i = 0;
        // printf("%s\n",buffer);
        int l= strlen(buffer);
        while(i < l) {
            int j = 0;
            while(buffer[i] != ' ') {
                // printf("%c\n",buffer[i]);
                username[j] = buffer[i];
                i++;
                j++;
            }
            username[j] = '\0';
            strncat(username,"\r\n",2);
            // printf("hello username\n");
            // printf("%d\n",strlen(username));
            // printf("%d\n",strlen(user_pass));
            i++;
            j = 0;
            while(buffer[i] != '\n') {
                password[j] = buffer[i];
                i++;
                j++;
            }
            password[j] = '\0';

            
            strncat(password,"\r\n",2);
            // printf("%s\n",username);
            // printf("%s\n",password);
            
            if(flag==1)
            {
                if(strcmp(user_pass, username) == 0)
                {
                    return 1;
                }
                else
                {
                    return 0;
                }
            }
            
            else if(flag ==2)
            {
                if(strcmp(user_pass, password) == 0)
                {
                    return 1;
                }
                else
                {
                    return 0;
                }
            }
            memset(username, 0, sizeof(username));
            memset(password, 0, sizeof(password));
            
        }
    }
    fclose(file);
    return 0;
}

// Function to handle client connections

void* handleClient(void* arg) {
    int clientSocket = *(int*)arg;
    char buffer[MAX_BUFFER_SIZE];
    char username[100];
    int j;
    // memset(username, 0, sizeof(username));
    memset(buffer, 0, sizeof(buffer));
    // printf("hello\n");
    // FILE* file = fopen("user.txt","r");
    // printf("hello\n");
    recv(clientSocket,buffer, sizeof(buffer), 0);
    
    // printf("%s\n",buffer);
    if(strncmp(buffer,"USER",4)==0)
    {
        // printf("hello 2\n");

        int check = isValidUser(buffer+5, 1);
        // printf("%d\n",check);
        if(check == 1)
        {
            // strcpy(username, buffer+5);
            //remove \r\n from username
            // printf("%s\n",buffer+5);
            // strncpy(username,buffer+5,strlen(buffer+5)-2);
            strcpy(username,buffer+5);
            // printf("user=%d\n",strlen(username));
            memset(buffer,0,sizeof(buffer));
            sprintf(buffer,"+OK user found\r\n");
            // printf("%s\n",buffer);
            fflush(stdout);
            send(clientSocket,buffer,strlen(buffer),0);
            memset(buffer,0,sizeof(buffer));
        }
        else
        {
            memset(buffer,0,sizeof(buffer));
            sprintf(buffer,"-ERR user not found\r\n");
            // printf("%s\n",buffer);
            fflush(stdout);
            send(clientSocket,buffer,strlen(buffer),0);
            memset(buffer,0,sizeof(buffer));
        }
        
        
        // recv(newsockfd,buffer,sizeof(buffer),0);
    }
    memset(buffer,0,sizeof(buffer));
    recv(clientSocket,buffer, sizeof(buffer), 0);
    // printf("%s\n",buffer);
    if(strncmp(buffer,"PASS",4)==0)
    {
        // printf("hello pass\n");
        if(isValidUser(buffer+5, 2)==1)
        {
            memset(buffer,0,sizeof(buffer));
            sprintf(buffer,"+OK %s's mailbox \r\n",username);
            // printf("%s\n",buffer);
            send(clientSocket,buffer,strlen(buffer),0);
            memset(buffer,0,sizeof(buffer));
        }
        else
        {
            memset(buffer,0,sizeof(buffer));
            sprintf(buffer,"-ERR incorrect password\r\n");
            // printf("%s\n",buffer);
            send(clientSocket,buffer,strlen(buffer),0);
            memset(buffer,0,sizeof(buffer));
        }
        
    }

    

    //access mail.txt for the user mails
    // printf("after user check\n");
    char mymailbox[100];
    char user[100];
    // printf("%s\n",username);
    // printf("%d\n",strlen(username));
    // strncpy(user,username,strlen(username)-2);
    int i;
    for(i=0;i<strlen(username)-2;i++)
    {
        // printf("%c\n",username[i]);
        user[i]=username[i];
    }
    user[i]='\0';
    // user[strlen(username)-2]='\0';
    //remove \r\n from username and in user
    // strncpy(user,username,strlen(username)-2);
    // // user[strlen(username)-2]='\0';
    // printf("%s\n",user);
    // printf("%d\n",strlen(user));
    
    strcpy(mymailbox,user);
    strncat(mymailbox,"/mail.txt",9);
    // printf("%s\n",mymailbox);
    // printf("checking\n");
    // strcpy(mymailbox,"srjt/mail.txt");

    FILE* mail = fopen(mymailbox,"r");
    // printf("checking\n");
    memset(buffer,0,sizeof(buffer));
    // printf("%s\n",mymailbox);
    

    //check every line of mail for \r\n.\r\n
    int n;
    int count = 0;
    int bytes = 0;
    int status[1024];
    int size[1024];
    char mail_array[1024][MAX_BUFFER_SIZE];
    int c_new=0;
    int bytes_new=0;
    int map[1024];
    for(i=0;i<count;i++)
    {
        status[i]=0;
        size[i]=0;
        memset(mail_array[i],0,sizeof(mail_array[i]));
    }
    // printf("before fgets\n");
    
    while((fgets(buffer,sizeof(buffer),mail)) > 0 )
    {
        
        if(strncmp(buffer,".\r\n",3)==0)
        {
            memset(buffer,0,sizeof(buffer));
            
            count++;
            

            
        }
        else
        {
            bytes += strlen(buffer);
            size[count]+=strlen(buffer);
            
            strcat(mail_array[count],buffer);
            memset(buffer,0,sizeof(buffer));
        }
        // printf("hellofile\n");
        

    }
    fclose(mail);
    // printf("%d\n",count);
    // printf("%d\n",bytes);
    
    

    

    for(i=0;i<count;i++)
    {

        map[i]=i+1;
    }

    // mail = fopen(mymailbox,"r");
    memset(buffer,0,sizeof(buffer));

    while(1)
    {
    memset(buffer,0,sizeof(buffer));
    recv(clientSocket,buffer,sizeof(buffer),0);
    if(strncmp(buffer,"STAT",4)==0)
    {
        // printf("hello stat\n");
        bytes_new = 0;
        c_new = 0;
        for(i=0;i<count;i++)
        {
            if(status[i]==0)
            {
                bytes_new += size[i];
                c_new++;

            }
        }
        memset(buffer,0,sizeof(buffer));
        sprintf(buffer,"+OK %d %d\r\n",c_new,bytes_new);
        send(clientSocket,buffer,strlen(buffer),0);
        memset(buffer,0,sizeof(buffer));
    }

    // fclose(mail);
    
   

    
   
    recv(clientSocket,buffer,sizeof(buffer),0);
    if(strncmp(buffer,"LIST",4)==0)
    {
        memset(buffer,0,sizeof(buffer));
        sprintf(buffer,"+OK %d messages (%d octets)\r\n",c_new,bytes_new);
        send(clientSocket,buffer,strlen(buffer),0);
        memset(buffer,0,sizeof(buffer));
        j=0;
        for(i=0;i<count;i++)
        {
            if(status[i]==0){
            sprintf(buffer,"%d %d\r\n",map[i],size[i]);
            send(clientSocket,buffer,strlen(buffer),0);
            memset(buffer,0,sizeof(buffer));
            // j++;
            }
        }
        sprintf(buffer,"\r\n.\r\n");
        send(clientSocket,buffer,strlen(buffer),0);
        memset(buffer,0,sizeof(buffer));
    }



    mail = fopen(mymailbox,"r");
    memset(buffer,0,sizeof(buffer));

    //assign each mail a number
    i = 0;
    // sprintf(buffer,"%d",c_new);
    // send(clientSocket,buffer,strlen(buffer),0);
    memset(buffer,0,sizeof(buffer));
    j=0;
    while((fgets(buffer,sizeof(buffer),mail)) > 0 )
    {
        char list[200];
        // printf("%s\n",buffer);
        // printf("%d\n",j++);
        if(status[i]==0)
        {
        if(strncmp(buffer,".\r\n",3)!=0)
        {
            
            // j++;
            if(strncmp(buffer,"From:",5)==0)
            {
                // char from[100];
                // strcat(from,buffer+5);
                // strcat(list,from);
                // memset(buffer,0,sizeof(buffer));
                char key[10];
                sprintf(key,"%d ",map[i]);
                send(clientSocket,key,strlen(key),0);
                // memset(buffer,0,sizeof(buffer));
                send(clientSocket,buffer+5,strlen(buffer+5),0);
                memset(buffer,0,sizeof(buffer));
            }
            else if(strncmp(buffer,"Received:",9)==0)
            {
                send(clientSocket,buffer+9,strlen(buffer+9),0);
                memset(buffer,0,sizeof(buffer));
            }
            else if(strncmp(buffer,"Subject:",8)==0)
            {
                send(clientSocket,buffer+8,strlen(buffer+8),0);
                memset(buffer,0,sizeof(buffer));
            }
            
        }
        else
        {

            memset(buffer,0,sizeof(buffer));
            strcpy(buffer,"\n");
            send(clientSocket,buffer,strlen(buffer),0);
            memset(buffer,0,sizeof(buffer));
            i++;

        }
        }
        else{
            if(strncmp(buffer,".\r\n",3==0))
            {
                memset(buffer,0,sizeof(buffer));
                i++;
            }
            else
            {
                memset(buffer,0,sizeof(buffer));
                
            }
            
        
        }
    }
    fclose(mail);
    // printf("outside file\n");
    memset(buffer,0,sizeof(buffer));
    sprintf(buffer,"\r\n.\r\n");
    send(clientSocket,buffer,strlen(buffer),0);
    memset(buffer,0,sizeof(buffer));
    // fclose(mail);
    //  if (fseek(mail, 0, SEEK_SET) == -1) {
    //     perror("Error rewinding file");
    //     fclose(mail);
    //     return 1;
    // }

    
    
    recv(clientSocket,buffer,sizeof(buffer),0);
    if(strncmp(buffer,"RETR",4)==0)
    {
        if(strncmp(buffer+5,"-1",2)==0)
        {
            memset(buffer,0,sizeof(buffer));
            continue;
        }
        int mailno = atoi(buffer+4);
        // printf("%d\n",mailno);
        memset(buffer,0,sizeof(buffer));
       
        for(i=0;i<count;i++)
        {
            if(map[i]==mailno)
            {
                sprintf(buffer,"+OK %d octets\r\n",size[i]);
                send(clientSocket,buffer,strlen(buffer),0);
                memset(buffer,0,sizeof(buffer));
                char* token = strtok(mail_array[i],"\n");
                while(token!=NULL)
                {
                    memset(buffer,0,sizeof(buffer));
                    sprintf(buffer,"%s\r\n",token);
                    send(clientSocket,buffer,strlen(buffer),0);
                    token = strtok(NULL,"\n");
                }
                memset(buffer,0,sizeof(buffer));
                sprintf(buffer,"\r\n.\r\n");
                send(clientSocket,buffer,strlen(buffer),0);
                memset(buffer,0,sizeof(buffer));
                
                break;
            }

        }
        if(i==count)
        {
            memset(buffer,0,sizeof(buffer));
            sprintf(buffer,"-ERR no such message\r\n");
            send(clientSocket,buffer,strlen(buffer),0);
            memset(buffer,0,sizeof(buffer));
        }
       
    }

    memset(buffer,0,sizeof(buffer));
    recv(clientSocket,buffer,sizeof(buffer),0);
    if(strncmp(buffer,"DELE",4)==0)
    {
        int mailno = atoi(buffer+4);
        // printf("%d\n",mailno);
        memset(buffer,0,sizeof(buffer));
        for(i=0;i<count;i++)
        {
            if(map[i]==mailno)
            {
                status[i]=1;
                memset(buffer,0,sizeof(buffer));
                sprintf(buffer,"+OK message %d deleted\r\n",mailno);
                send(clientSocket,buffer,strlen(buffer),0);
                memset(buffer,0,sizeof(buffer));
                break;
            }
        }
        j=1;
        for(i=0;i<count;i++)
        {
            if(status[i]==0)
            {
                map[i]=j;
                j++;
            }
        }
        
    }
    memset(buffer,0,sizeof(buffer));
    recv(clientSocket,buffer,sizeof(buffer),0);
    if(strncmp(buffer,"QUIT",4)==0)
    {
        break;
    }
    }
    // memset(buffer,0,sizeof(buffer));


    
    return NULL;
}

//check if ther user and pass are valid



int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    char buffer[MAX_BUFFER_SIZE];

    if(argc!=2)
    {
        printf("Add port\n");
        exit(1);
    }
   

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    // Initialize socket structure
    // bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // Bind the host address
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    // Start listening for the clients
    if (listen(sockfd, 5) < 0) {
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }
    

    while (1) {
        // Accept connection from client
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) {
            perror("ERROR on accept");
            exit(1);
        }

        if(fork()==0)
        {
            close(sockfd);
            memset(buffer,0,sizeof(buffer));
            sprintf(buffer,"+OK POP3 server ready\r\n");
            send(newsockfd,buffer,strlen(buffer),0);
            // printf("hello child\n");
            handleClient(&newsockfd);
            close(newsockfd);
            exit(0);
        }
        close(newsockfd);

        // // Create a new thread for the client
        // if (pthread_create(&tid, NULL, handleClient, &newsockfd) != 0) {
        //     perror("Failed to create thread");
        // }
    }

    
    return 0;
}





