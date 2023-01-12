#define PORT 8080 

// possible menus
#define START_MENU 0
#define SIGN_UP_OPTIONS 1
#define SIGN_IN_OPTIONS 2
#define USER_OPTIONS 3
#define ADMIN_OPTIONS 4

// possible options
#define SIGN_UP 5
#define SIGN_IN 6
#define USER_SIGN_UP 7
#define SIGN_UP_AS_JOINT 8
#define SIGN_UP_AS_ADMIN 9
#define USER_SIGN_IN 10
#define SIGN_IN_AS_JOINT 11
#define SIGN_IN_AS_ADMIN 12
#define DEPOSIT 13
#define WITHDRAW 14
#define BALANCE 15
#define PASSWORD 16
#define DETAILS 17
#define EXIT 18
#define ADD_USER 19
#define DEL_USER 20
#define MODIFY_USER 21
#define GET_USER_DETAILS 22
#define INVALID -1
#define BUFF_SIZE 1000

//struct user
struct user 
{
    char type[BUFF_SIZE];
    char password[BUFF_SIZE];
    char user_name[BUFF_SIZE];
};

//struct details
struct account {
    int balance;
};


