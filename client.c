#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include "constants.h"
void interface(int);
int get_authorization_details(int,int);
void user_interface(int);
void admin_interface();
int printOptions(int);
int main(int argc, char const *argv[]) 
{ 
    //connection establish
    //to send messages
    //interface(sock) is for displaying choosing from these goes to user_interface
    //server has authorization process backend
    //sending msgs is in client
    //signals from client to server
    //client frontend
    //send is giving input client to server
    //in server inputs are taken checks in database whether it is there are not
    //return msg is read in client
    //if success we go into user_interface
	int sock = 0;
    int val_read; 
	struct sockaddr_in server_add; 
	char *hello = "Hello"; 
	char buffer[1024] = {0}; 
    //AF_INET refers to Address from the Internet and it requires a pair of (host, port) where the host can either be a URL of some particular website or its address and the port number is an integer.
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\nerror,socket can't be created \n"); 
		return -1; 
	} 
	server_add.sin_family = AF_INET; 
    //The htons() function translates a short integer from host byte order to network byte order. 
	server_add.sin_port = htons(PORT); 
	if(inet_pton(AF_INET, "127.0.0.1", &server_add.sin_addr)<=0) 
	{ 
		printf("\nInvalid address\n"); 
		return -1; 
	} 
	if(connect(sock, (struct sockaddr *)&server_add, sizeof(server_add)) < 0) 
	{ 
		printf("\nerror : connection failed\n"); 
		return -1; 
	} 
	////interface(sock) is for displaying choosing from these goes to user_interface.
	interface(sock);
	return 0; 
}
//joint users many users using one account
void interface(int sock)
{
	int option , opt;
	char* option_string = malloc(10*sizeof(char));
	char* message = malloc(BUFF_SIZE*sizeof(char));
	option = printOptions(START_MENU);
	switch (option) 
    {
		case SIGN_UP:
			opt = get_authorization_details(sock,SIGN_UP_OPTIONS);
			while(1) 
            {
				switch (opt) 
                {
					case USER_SIGN_UP: 
						user_interface(sock);
						break;
					case SIGN_UP_AS_JOINT :
						user_interface(sock);
						break;
					case SIGN_UP_AS_ADMIN :
						admin_interface(sock);
						break;
				}
			}
			break;
		case SIGN_IN:
			opt = get_authorization_details(sock,SIGN_IN_OPTIONS);
			switch (opt) 
            {
				case USER_SIGN_IN :
					//while(1)
					for(;;)
                    {
						user_interface(sock);
					}
					break;
				case SIGN_IN_AS_ADMIN :
					while(1) 
                    {
						admin_interface(sock);
					}
					break;
				case SIGN_IN_AS_JOINT :
					while(1)
                    {
						user_interface(sock);
					}
			}
			break;
		default:
			printf("Invalid Option\n");
			exit(1);
	}	
}
int get_authorization_details(int sock, int option)
{
	char* username = malloc(BUFF_SIZE*sizeof(char));
	char* password = malloc(BUFF_SIZE*sizeof(char));
	char* msg = malloc(BUFF_SIZE*sizeof(char));
	char* option_string = malloc(10*sizeof(char));
	printf("option = %d\n",option);
	int ret_option = printOptions(option);
	printf("username: ");
	scanf("%s",username);
	printf("password: ");
	scanf("%s",password);
	sprintf(option_string,"%d",option);
	//send takes input of username password and send to server from client.
	//messages are sent via a socket.
	send(sock , option_string , sizeof(option_string) , 0 ); 
	send(sock , username , sizeof(username) , 0 ); 
	send(sock , password , sizeof(password) , 0 ); 
	read( sock , msg, BUFF_SIZE * sizeof(char)); 
	printf("%s\n",msg); 
	if(!strcmp(msg,"error occured during sign in\n")) exit(1);
	return ret_option;
}
//return msg is read in client.
//if success we go into user_interface.
void user_interface(int sock)
{
	int option = printOptions(USER_OPTIONS),deposit_amount,withdraw_amount;
	char* option_string = malloc(20*sizeof(char));
	char* msg = malloc(BUFF_SIZE*sizeof(char));
	char* amount_string = malloc(BUFF_SIZE*sizeof(char));
	char* pwd = malloc(BUFF_SIZE*sizeof(char));
	sprintf(option_string,"%d",option);
	send(sock , option_string , sizeof(option_string) , 0 ); 
	switch(option) 
	{
		case WITHDRAW :
			printf("amount to be withdrawn: ");
			scanf("%d",&withdraw_amount);
			sprintf(amount_string,"%d",withdraw_amount);
			send(sock, amount_string, sizeof(amount_string), 0);
			break;
		case DEPOSIT : 
			printf("amount to be deposited: ");
			scanf("%d",&deposit_amount);
			sprintf(amount_string,"%d",deposit_amount);	
			printf("%s\n",amount_string);		
			send(sock, amount_string, sizeof(amount_string), 0);
			break;
		case BALANCE : 
			break;
		case PASSWORD : 
			printf("new password\n");
			scanf("%s",pwd);
			send(sock, pwd, sizeof(pwd), 0);
			break;
		case DETAILS : 
			break;
		case EXIT : 
			exit(0);
	}
	read(sock , msg, BUFF_SIZE * sizeof(char)); 
	printf("%s\n",msg); 
}

void admin_interface(int sock)
{
	int option = printOptions(ADMIN_OPTIONS), type;
	char* option_string = malloc(20*sizeof(char));
	char* pwd = malloc(BUFF_SIZE*sizeof(char));
	char* new_username = malloc(BUFF_SIZE*sizeof(char));
	char* user_name = malloc(BUFF_SIZE*sizeof(char));
	char* message = malloc(BUFF_SIZE*sizeof(char));
	sprintf(option_string,"%d",option);
	send(sock , option_string , sizeof(option_string) , 0 ); 
	switch (option) 
	{
		case ADD_USER :
			printf("Enter User Type\n");
			printf("1 : Normal\n");
			printf("2 : Joint\n");
			scanf("%d",&type);
			printf("username : ");
			scanf("%s",user_name);
			printf("password : ");
			scanf("%s",pwd);
			switch (type) 
			{
				case 1 : 
					send(sock , "1" , sizeof("1") , 0 ); 
					break;
				case 2 : 
					send(sock , "2" , sizeof("2") , 0 ); 
					break;
				default :
					printf("Invalid Type\n");
					exit(1);
			}
			printf("username = %s\n",user_name);
			printf("password = %s\n",pwd);
			send(sock , user_name , sizeof(user_name) , 0 ); 
			send(sock , pwd , sizeof(pwd) , 0 ); 
			break;
		case DEL_USER : 
			printf("username : ");
			scanf("%s",user_name);
			send(sock , user_name , sizeof(user_name) , 0 ); 
			break;
		case GET_USER_DETAILS : 
			printf("username : ");
			scanf("%s",user_name);
			send(sock , user_name , sizeof(user_name) , 0 ); 
			break;
		case MODIFY_USER : 
			printf("old username : " );
			scanf("%s",user_name);
			send(sock , user_name , sizeof(user_name) , 0 ); 
			printf("new username : ");
			scanf("%s",new_username);
			send(sock , new_username , sizeof(new_username) , 0 ); 
			printf("new password : ");
			scanf("%s",pwd);
			send(sock , pwd , sizeof(pwd) , 0 ); 
			break;
		case EXIT : 
			exit(0);
		default :
			printf("Invalid input\n");
			exit(0);
	}
	read( sock , message, BUFF_SIZE * sizeof(char)); 
	printf("%s\n",message); 
}
int printOptions(int menu)
{
	int option;
	switch (menu) 
	{
		case START_MENU:
			printf("Hello enter your choice\n");
			printf("1 : Sign Up\n");
			printf("2 : Sign In\n");
			scanf("%d",&option);
			switch(option) 
			{
				case 1 :
					return SIGN_UP;
				case 2 :
					return SIGN_IN;
				default : 
					return INVALID ;
			} 
		case SIGN_UP_OPTIONS:
			printf("Hello,How would you like to sign up?\n");
			printf("1 : User\n");
			printf("2 : Joint Account User\n");
			printf("3 : Administrator\n");
			scanf("%d",&option);
			switch(option) {
				case 1 :
					return USER_SIGN_UP;
				case 2 :
					return SIGN_UP_AS_JOINT;
				case 3 : 
					return SIGN_UP_AS_ADMIN;
				default :
					return INVALID;
			}
		case SIGN_IN_OPTIONS : 
			printf("How would you like to sign in?\n");
			printf("1 : User\n");
			printf("2 : Joint Account User\n");
			printf("3 : Administrator\n");
			scanf("%d",&option);
			switch(option)
			{
				case 1 :
					return USER_SIGN_IN;
				case 2 :
					return SIGN_IN_AS_JOINT;
				case 3 : 
					return SIGN_IN_AS_ADMIN;
				default :
					return -1;
			}
		case USER_OPTIONS : 
			printf("What would you like us to do for you?\n");
			printf("1 : Deposit\n");
			printf("2 : Withdraw\n");
			printf("3 : Check Balance\n");
			printf("4 : Change Pwd\n");
			printf("5 : View Details\n");
			printf("6 : Exit\n");
			scanf("%d",&option);
			switch(option) {
				case 1 : return DEPOSIT;
				case 2 : return WITHDRAW;
				case 3 : return BALANCE;
				case 4 : return PASSWORD;
				case 5 : return DETAILS;
				case 6 : return EXIT;
				default : return -1;
			}
		case ADMIN_OPTIONS :
			printf("1 : Add User\n");
			printf("2 : Delete User\n");
			printf("3 : Modify User\n");
			printf("4 : Search for Account Details\n");
			printf("5 : Exit\n") ;
			scanf("%d",&option);
			switch (option) 
			{
				case 1 : return ADD_USER;
				case 2 : return DEL_USER;
				case 3 : return MODIFY_USER;
				case 4 : return GET_USER_DETAILS;
				case 5 : return EXIT;
				default : return -1;
			}
		default:
			break;
	}
}