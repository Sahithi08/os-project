#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <pthread.h>
#include "constants.h"
extern int signin(int , char* ,char*);
extern int signup(int , char* ,char*); 
extern int deposit(char* ,int);
extern int withdraw(char* ,int);
extern char* details(char*);
extern int balance(char*);
extern int change_password(char* ,char*);
void *connection_handler(void *);
extern int del_user(char*);
extern int modify_user (char* ,char* ,char*);
int main(int agrc, char const *argv[])
{
    //first we have to establish the connection
    //server has authorization process
    //server is the backend
    int server_fd;
    int new_socket;
    int val;
    struct sockaddr_in add;
    int opt = 1;
    int addrlen = sizeof(add);
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	{ 
		perror("error:socket failed"); 
		exit(EXIT_FAILURE); 
	} 
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
	{ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 
	add.sin_family = AF_INET; 
	add.sin_addr.s_addr = INADDR_ANY; 
	add.sin_port = htons(PORT); 
	//bind() is a socket identification.
    if(bind(server_fd, (struct sockaddr *)&add, sizeof(add))<0) 
	{ 
		perror("error:bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	if(listen(server_fd, 3) < 0) 
	{ 
        //The listen() system call prepares a connection-oriented server to accept client connections.
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 
    printf("ready to listen!!\n");
    for(;;)
    {
        //The accept() system call is used with connection-based socket types (SOCK_STREAM, SOCK_SEQPACKET). It extracts the first connection request on the queue of pending connections, creates a new connected socket, and returns a new file descriptor referring to that socket.
        if((new_socket = accept(server_fd, (struct sockaddr *)&add, (socklen_t*)&addrlen))<0) 
        { 
            perror("accept"); 
            exit(EXIT_FAILURE); 
        } 
        pthread_t thread_id;
        if(pthread_create( &thread_id , NULL ,  connection_handler , (void*) &new_socket) < 0)
        {
            perror("can not create thread");
            return 1;
        }
        puts("Handler assigned");
    }
	return 0; 
}
//server takes inputs from client and checks whether it is in database or not.
//it then returns it to client 
void *connection_handler(void *socket_desc)
{
	int sock = *(int*)socket_desc , option, deposit_amt, withdraw_amount, ret ,balance_amount;
	char* user_name = malloc(BUFF_SIZE*sizeof(char));
	char* pwd = malloc(BUFF_SIZE*sizeof(char));
    for(;;)
    {
		char* type = malloc(BUFF_SIZE*sizeof(char));
		char* message = malloc(BUFF_SIZE*sizeof(char));
		char* option_string = malloc(BUFF_SIZE*sizeof(char));
        char* new_username = malloc(BUFF_SIZE*sizeof(char));
		char* amount_string = malloc(BUFF_SIZE*sizeof(char));
		printf("sock = %d\n",sock);
        //It reads inputs from the client.
		read(sock , option_string, sizeof(option_string)); 
        //the atoi function converts a string to an integer. 
		option = atoi(option_string);
		if(option == USER_SIGN_UP || option == SIGN_UP_AS_ADMIN || option == SIGN_UP_AS_JOINT)
		{
			read( sock , user_name, sizeof(user_name));
			read( sock , pwd, sizeof(pwd));
			int ret = signup(option,user_name,pwd);
			if(ret == -1) message = "\n";
			else message = "User added successfully!\n";
		}
		else if(option == USER_SIGN_IN || option == SIGN_IN_AS_ADMIN || option == SIGN_IN_AS_JOINT)
		{
			read(sock , user_name, sizeof(user_name));
			read(sock , pwd, sizeof(pwd));
			ret = signin(option,user_name,pwd);
			if(ret == -1) message = "error:sign in failed\n";
			else message = "signed in!\n";
		}
		else if(option == DEPOSIT)
		{
			read(sock , amount_string, sizeof(amount_string));
			deposit_amt = atoi(amount_string);
			ret = deposit(user_name,deposit_amt);
			if(ret == 0) message = "amount deposited\n";
		}
		else if(option == WITHDRAW) 
		{
			read(sock , amount_string , sizeof(amount_string));
			withdraw_amount = atoi(amount_string);
			ret = withdraw(user_name , withdraw_amount);
			if (ret == -1) message = "error:unable to withdraw\n";
			else message = "withdrew successfully\n";
		}
		else if(option == BALANCE)
		{
			balance_amount = balance(user_name);
			sprintf(message,"%d",balance_amount);
		}
		else if(option == PASSWORD) 
		{
			read(sock, pwd, sizeof(pwd));
			ret = change_password (user_name, pwd);
			if (ret == -1) message = "error:can't change the password\n";
			else message = "successfully changed the password\n";
		}
		else if(option == DEL_USER) 
		{
			char* pwd = malloc(BUFF_SIZE*sizeof(char));
            char* user_name = malloc(BUFF_SIZE*sizeof(char));
			read(sock , user_name, sizeof(user_name));
			ret = del_user(user_name);
			printf("unlink returned %d\n",ret);
			if (ret == -1) message = "error:can't delete user\n";
			else message = "successfully deleted the user\n";
		}
		else if(option == GET_USER_DETAILS) 
		{
			char* username = malloc(BUFF_SIZE*sizeof(char));
			read( sock , username, sizeof(username));
			printf("username = %s\n",username);
			message = details(username);
		}
        else if(option == DETAILS) 
		{
			message = details(user_name);
		}	
		else if(option == MODIFY_USER) 
		{
			char* pwd = malloc(BUFF_SIZE*sizeof(char));
            char* user_name = malloc(BUFF_SIZE*sizeof(char));
			read(sock , user_name, sizeof(user_name));
			read(sock , new_username, sizeof(new_username));
			read(sock , pwd, sizeof(pwd));
			ret = modify_user (user_name, new_username,pwd);
			if (ret == -1) message = "error:can't change the user\n";
			else message = "changed user successfully\n";
		}			
		else if (option == ADD_USER) 
        {
			char* username = malloc(BUFF_SIZE*sizeof(char));
			char* password = malloc(BUFF_SIZE*sizeof(char));
			read( sock , type, sizeof(type));
			read( sock , username, sizeof(username));
			read( sock , password, sizeof(password));
			printf("type = %s username = %s pwd = %s\n",type,username,password);
			if(!strcmp(type ,"1")) option = USER_SIGN_UP;
			else option = SIGN_UP_AS_JOINT;
			ret = signup(option ,username, password);
			if(ret == -1) message = "account could not be added\n";
			else message = "account is added!\n";
		}
		send(sock , message , BUFF_SIZE * sizeof(char) , 0 ); 
		//It will again send the msg to client which then displays the msg.
	}
    return 0;
} 
