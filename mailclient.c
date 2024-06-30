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
#include <time.h>
#include <ctype.h>

char *strstrip(char *s)
{
    size_t size;
    char *end;

    size = strlen(s);

    if (!size)
        return s;

    end = s + size - 1;
    while (end >= s && isspace(*end))
        end--;
    *(end + 1) = '\0';

    while (*s && isspace(*s))
        s++;

    return s;
}

int isEmailFormat(const char *email) {
    int atCount = 0;
    int dotCount = 0;
    int atIndex = -1;

    // Iterate through the characters in the email string
    for (int i = 0; email[i] != '\0'; i++) {
        // Check for '@' symbol
        char c = email[i];
        if (c == '@') {
            atCount++;
            atIndex = i;
           
        }
        // Check for '.' symbol
        else if (c == '.') {
            dotCount++;
            
        }
        else if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || ((c >= '0') && (c <= '9')) || (c == ' ')){
            continue;
        }
        else{
            
            return 1;
        }
    }

    // Check the conditions for a valid email
    if (atCount == 1 && dotCount > 0 && atIndex > 0 && atIndex < strlen(email) - 1 && email[atIndex + 1] != '.') {
        return 0; // Valid email
    } else {
        return 1; // Invalid email
    }
    
}

int isValidMessageFormat(const char *message) {
    // Use strtok to extract fields from the message
    char *token;
    char *messageCopy = strdup(message);  // Create a copy of the message

    // Check the From field
    token = strtok(messageCopy, ":");
    if (token == NULL || strcmp(token, "From") != 0) {
        free(messageCopy);
        return 0; // Invalid format
    }
    
    token = strtok(NULL, "\n");
    if (token == NULL || isEmailFormat(token)) {
        free(messageCopy);
        return 0; // Invalid email format for From field
    }
   
    // Check the To field
    token = strtok(NULL, ":");
    if (token == NULL || strcmp(token, "To") != 0) {
        free(messageCopy);
        return 0; // Invalid format
    }
    
    token = strtok(NULL, "\n");
    if (token == NULL || isEmailFormat(token)) {
        free(messageCopy);
        return 0; // Invalid email format for To field
    }
    

    // Check the Subject field
    token = strtok(NULL, ":");
    if (token == NULL || strcmp(token, "Subject") != 0) {
        free(messageCopy);
        return 0; // Invalid format
    }
    

    // token = strtok(NULL, ".");


    // The message body can be empty or contain content (not checked here)

    free(messageCopy);
    return 1; // Valid format
}

int main(int argc, char *argv[]) {
    // Create a socket
    int sockfd1 = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd1 < 0) {
        perror("Error creating socket");
        return 1;
    }

    int sockfd2 = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd2 < 0) {
        perror("Error creating socket");
        return 1;
    }
    if(argc!=4)
    {
        printf("Add ip address , smtp port and pop3 port\n");
        return 1;
    }
   
    const char* ip = argv[1];
    int port1 = atoi(argv[2]);
    int port2 = atoi(argv[3]);

    // Connect to the server
    struct sockaddr_in server_addr1, server_addr2;
    server_addr1.sin_family = AF_INET;
    server_addr1.sin_addr.s_addr = inet_addr(ip); // Server IP address
    server_addr1.sin_port = htons(port1); // SMTP port

    server_addr2.sin_family = AF_INET;
    server_addr2.sin_addr.s_addr = inet_addr(ip); // Server IP address
    server_addr2.sin_port = htons(port2); // SMTP port
    
    if (connect(sockfd1, (struct sockaddr*)&server_addr1, sizeof(server_addr1)) < 0) {
        perror("Error connecting to server");
        return 1;
    }

    if (connect(sockfd2, (struct sockaddr*)&server_addr2, sizeof(server_addr2)) < 0) {
        perror("Error connecting to server");
        return 1;
    }

    // Send SMTP commands
    char buffer[1024];
    char input[5000] ;
    
    size_t input_size ;
    char user[100];
    char password[100];

    memset(buffer, 0, sizeof(buffer));
    recv(sockfd2, buffer, sizeof(buffer), 0);
    if(strncmp(buffer,"+OK",3)!=0)
    {
        printf("Error in connection\n");
        return 1;
    }

    printf("Enter your username: ");
    scanf("%s",user);
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "USER %s\r\n", user);
    send(sockfd2, buffer, sizeof(buffer), 0);
    memset(buffer, 0, sizeof(buffer));
    recv(sockfd2, buffer, sizeof(buffer), 0);
    // printf("%s\n",buffer);
    if(strncmp(buffer,"-ERR",4)==0)
    {
        printf("Wrong username\n");
        return 1;
    }
    else if(strncmp(buffer,"+OK",3)==0)
    {
        printf("Correct username\n");
        // return 1;
    }
    printf("Enter your password: ");
    scanf("%s",password);
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "PASS %s\r\n", password);
    send(sockfd2, buffer, sizeof(buffer), 0);
    memset(buffer, 0, sizeof(buffer));
    recv(sockfd2, buffer, sizeof(buffer), 0);
    // printf("%s\n",buffer);
    if(strncmp(buffer,"-ERR",4)==0)
    {
        printf("Wrong password\n");
        return 1;
    }
    else if(strncmp(buffer,"+OK",3)==0)
    {
        printf("Correct password\n");
        // return 1;
    }

    // int file = open("user.txt", O_WRONLY | O_CREAT | O_APPEND, 0666);
    // write(file, user, strlen(user));
    // write(file, "\t", 1);
    // write(file, password, strlen(password));
    // write(file, "\n", 1);


    
    while(1)
    {   fflush(stdin);
        memset(buffer, 0, sizeof(buffer));
        memset(input,0,sizeof(input));
        int flag =0;
        
        int f=0;
        // input_size=0;
        printf("Choose any one:\n1.Manage mail\n2. Send mail\n3. QUIT\n");
        scanf("%s",buffer);
        fflush(stdout);
        switch(buffer[0])
        {
            case '1':{
                int num;
                int mail_octets;
                int n;
                while(1)
                {
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"STAT\r\n");
                send(sockfd2, buffer, sizeof(buffer), 0);
                memset(buffer, 0, sizeof(buffer));
                recv(sockfd2, buffer, sizeof(buffer), 0);
                if(strncmp(buffer,"+OK",3)==0)
                {
                    //read integer from a string
                    // printf("%s\n",buffer);
                    
                    sscanf(buffer, "+OK %d %d\r\n", &num, &mail_octets);
                    printf("Number of mails: %d\n",num);
                    printf("Mail octets: %d\n",mail_octets);
                    memset(buffer, 0, sizeof(buffer));
                }
                else{
                    
                        printf("Error in connection1\n");
                        return 1;
                    
                }
               
                
                // int num = atoi(buffer);
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer, "LIST\r\n");
                send(sockfd2, buffer, sizeof(buffer), 0);
                memset(buffer, 0, sizeof(buffer));
                recv(sockfd2, buffer, sizeof(buffer), 0);
                if(strncmp(buffer,"-ERR",4)==0)
                {
                    printf("Error in connection2\n");
                    return 1;
                }
                else if(strncmp(buffer,"+OK",3)==0)
                {
                    memset(buffer, 0, sizeof(buffer));
                    while(recv(sockfd2, buffer, sizeof(buffer), 0)>0)
                    {
                        printf("%s ",buffer);
                        if(strncmp(buffer+strlen(buffer)-5,"\r\n.\r\n",5)==0)
                        {
                            memset(buffer, 0, sizeof(buffer));
                            break;
                        
                        } 
                        printf("%s ",buffer);
                        memset(buffer, 0, sizeof(buffer));
                    }
                    printf("\n");
                }
                
                
                // printf("outside\n");


                // printf("\n");
               
                while(1)
                {
                    printf("Enter the mail number to read: ");
                    scanf("%d",&n);
                    if(n>num)
                    {
                        printf("Out of range, enter again\n");
                        continue;
                    }
                    else if(n==-1)
                    {
                        flag=1;
                        break;
                    }
                    else
                    {
                        break;
                    }
                    
                

                }
                if(flag==1) 
                {
                    memset(buffer, 0, sizeof(buffer));
                    sprintf(buffer, "RETR %d\r\n",n);
                    send(sockfd2, buffer, sizeof(buffer), 0);
                    memset(buffer, 0, sizeof(buffer));
                    break;
                }
                
                
                // printf("hello\n");
                
                sprintf(buffer, "RETR %d\r\n",n);
                send(sockfd2, buffer, sizeof(buffer), 0);
                memset(buffer, 0, sizeof(buffer));
                recv(sockfd2, buffer, sizeof(buffer), 0);
                if(strncmp(buffer,"-ERR",4)==0)
                {
                    printf("Error in reading mail\n");
                    return 1;
                }
                else if(strncmp(buffer,"+OK",3)==0)
                {
                    // printf("%s\n",buffer);
                    memset(buffer, 0, sizeof(buffer));
                    while(recv(sockfd2, buffer, sizeof(buffer), 0)>0)
                    {
                        // printf("%s ",buffer);
                        // printf("%d\n",strlen(buffer));
                        
                        // printf("%s ",buffer);
                        if(strncmp(buffer+strlen(buffer)-5,"\r\n.\r\n",5)!=0)
                        {
                            printf("%s ",buffer);
                            // break;
                        
                        }
                        else if(strncmp(buffer+strlen(buffer)-5,"\r\n.\r\n",5)==0)
                        {
                            printf("%s\n",buffer);
                            // printf("%d\n",strlen(buffer));
                            memset(buffer, 0, sizeof(buffer));
                            break;
                        
                        }
                        else if(strncmp(buffer,"\r\n.\r\n",5)==0)
                        {
                            // printf("%s\n",buffer);
                            // printf("%d\n",strlen(buffer));
                            memset(buffer, 0, sizeof(buffer));
                            break;
                        
                        }
                        // printf("%d\n",strlen(buffer));
                        memset(buffer, 0, sizeof(buffer));
                    }
                    
                    // return 1;
                }
                // printf("out of loop\n");
                char ch = getchar();
                // printf("get\n");
                
                if(ch == 'd')
                {
                    printf("deletion\n");
                    memset(buffer, 0, sizeof(buffer));
                    sprintf(buffer, "DELE %d\r\n",n);
                    send(sockfd2, buffer, sizeof(buffer), 0);
                    memset(buffer, 0, sizeof(buffer));
                    recv(sockfd2, buffer, sizeof(buffer), 0);
                    if(strncmp(buffer,"-ERR",4)==0)
                    {
                        printf("Error in deleting mail\n");
                        return 1;
                    }
                    else if(strncmp(buffer,"+OK",3)==0)
                    {
                        printf("Mail deleted\n");
                        memset(buffer, 0, sizeof(buffer));
                    }
                    memset(buffer, 0, sizeof(buffer));
                    sprintf(buffer, "QUIT\r\n");
                    send(sockfd2, buffer, sizeof(buffer), 0);
                    memset(buffer, 0, sizeof(buffer));
                    break;
                }
                else
                {
                    continue;
                }

                }
                
                break;


                
                
            }
            
            case '2':{
                fflush(stdin);
                memset(buffer, 0, sizeof(buffer));
                printf("Enter your mail content :\n");
                int k=0;
                
                while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
                   
                    fflush(stdout);
                    
                    strcat(input, buffer);
                
                    if(strncmp(buffer, ".", 1)==0) break;
                }

                if (isValidMessageFormat(input)) {
                    printf("The message format is valid.\n");
                    f=1;
                } else {
                    printf("The message format is invalid.\n");
                    
                }
                break;
            }
            break;
            case '3':
            {
                memset(buffer,0,sizeof(buffer));
                sprintf(buffer, "QUIT\r\n");
                send(sockfd1, buffer, strlen(buffer), 0);
                memset(buffer, 0, sizeof(buffer));
                recv(sockfd1, buffer, sizeof(buffer), 0);
                if(strncmp(buffer,"221",3)!=0)
                    close(sockfd1);
                return 0;
            }
            default:
            {
                printf("Wrong choice\n");
                break;
            }
        }
        if(f==1) break;
        if(flag == 1) continue;
        

    }
    if(buffer[0]==2){
    char sender[100];
    char recipient[100];
    char domain[50];

    // Extract the sender and recipient from the message
    char *token;
    char *inputCopy = strdup(input);  // Create a copy of the message
    token = strtok(inputCopy, "\n");
    while (token != NULL) {
        if (strncmp(token, "From:", 5) == 0) {
            strcpy(sender, token + 6);
        } else if (strncmp(token, "To:", 3) == 0) {
            strcpy(recipient, token + 4);
        }
        token = strtok(NULL, "\n");
    }

    for(int i=0;sender[i]!='\0';i++)
    {
        if(sender[i]=='@')
        {
            strcpy(domain,sender+i+1);
            break;
        }
    }

    memset(buffer, 0, sizeof(buffer));

    // Send HELO command
    recv(sockfd1, buffer, sizeof(buffer), 0);
    if(strncmp(buffer,"220",3)!=0)
    {
        printf("Error in connection: 220\n");
        return 1;
    }


    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "HELO %s\r\n",domain);
    int s_err = send(sockfd1, buffer, strlen(buffer), 0);
    send(sockfd1, buffer, strlen(buffer), 0);
    // send(sockfd, buffer, strlen(buffer), 0);

    memset(buffer, 0, sizeof(buffer));

    recv(sockfd1, buffer, sizeof(buffer), 0);
    if(strncmp(buffer,"250",3)!=0)
    {
        printf("Error in connection: 250 \n");
        return 1;
    }

    memset(buffer, 0, sizeof(buffer));
    
    // Send MAIL FROM command
    sprintf(buffer, "MAIL FROM: <%s>\r\n",sender);
    send(sockfd1, buffer, strlen(buffer), 0);
    memset(buffer, 0, sizeof(buffer));

    recv(sockfd1, buffer, sizeof(buffer), 0);
    if(strncmp(buffer,"250",3)!=0)
    {
        printf("Wrong sender\n");
        return 1;
    }
    memset(buffer, 0, sizeof(buffer));

    // Send RCPT TO command
    sprintf(buffer, "RCPT TO: <%s>\r\n",recipient);
    send(sockfd1, buffer, strlen(buffer), 0);
    memset(buffer, 0, sizeof(buffer));

    recv(sockfd1, buffer, sizeof(buffer), 0);
    if(strncmp(buffer,"250",3)!=0)
    {
        printf("Wrong recipient\n");
        return 1;
    }
    memset(buffer, 0, sizeof(buffer));

    // Send DATA command
    sprintf(buffer, "DATA\r\n");
    send(sockfd1, buffer, strlen(buffer), 0);
    memset(buffer, 0, sizeof(buffer));
    recv(sockfd1, buffer, sizeof(buffer), 0);
    if(strncmp(buffer,"354",3)!=0)
    {
        printf("Error\n");
        return 1;
    }
    memset(buffer, 0, sizeof(buffer));

    // Send email content

    token = strtok(input, "\n");

    // Copy each line to the buffer
    while (token != NULL) {
        
        if(strncmp(token,".",1)==0)
        {
            
            sprintf(buffer, "\r\n.\r\n");
            int byte=send(sockfd1, buffer, strlen(buffer), 0);
            

            memset(buffer, 0, sizeof(buffer));
            break;
        }
        
        strcpy(buffer, token);
        strncat(buffer, "\r\n", 2);
        send(sockfd1, buffer, strlen(buffer), 0);
        memset(buffer, 0, sizeof(buffer));
        if(strncmp(token,"To:",3)==0)
        {
            time_t t;
            time(&t);

            sprintf(buffer, "Received: %s\r\n",ctime(&t));
            send(sockfd1, buffer, strlen(buffer), 0);
            // printf("%s\n",buffer);
            memset(buffer, 0, sizeof(buffer));
            token = strtok(NULL, "\n");
            // continue;
        }

        // Process the line in the buffer (you can perform further actions here)

        // Get the next token
        token = strtok(NULL, "\n");
    }
   

    recv(sockfd1, buffer, sizeof(buffer), 0);
    if(strncmp(buffer,"250",3)!=0)
    {
        printf("Message not delivered successfully\n");
        return 1;
    }
    memset(buffer, 0, sizeof(buffer));
    // printf("msg delivered\n");

    // Send QUIT command
    sprintf(buffer, "QUIT\r\n");
    send(sockfd1, buffer, strlen(buffer), 0);
    // printf("%s\n",buffer);
    memset(buffer, 0, sizeof(buffer));

    // Close the socket
    recv(sockfd1, buffer, sizeof(buffer), 0);
    if(strncmp(buffer,"221",3)!=0)
        close(sockfd1);
    }

    return 0;
}
