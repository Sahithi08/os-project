#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "constants.h"
//we also use file locking to avoid joint users.
int signup(int option, char* user_name, char* password)
{
    char file_name[BUFF_SIZE];
    strcpy(file_name,user_name);
    char extension[5] = ".txt";
    //The strncat() function appends the first count characters of string2 to string1 and ends the resulting string with a null character (\0).
    strncat(file_name ,extension ,sizeof(extension));
    int fd = open(file_name,O_WRONLY);
    if(fd != -1)
    { 
        return -1;
    }
    else 
    {
        close(fd);
    }
    fd = open(file_name,O_WRONLY | O_CREAT,0644);
    if(fd == -1)
    {
        perror("signup"); 
        return -1;
    }
    //it requires username and password to sign up.
    struct user u;
    strcpy(u.user_name,user_name);
    strcpy(u.password,password);
    switch(option)
    {
        //we can sign up as normal user or joint user. 
        case USER_SIGN_UP:
            strcpy(u.type,"normal");
            break;
        case ADD_USER :
            strcpy(u.type,"normal");
            break;
        case SIGN_UP_AS_JOINT :
            strcpy(u.type,"joint");
            break;
        case SIGN_UP_AS_ADMIN :
            strcpy(u.type,"admin");
            break;
    }
    write(fd ,&u ,sizeof(struct user));
    struct account a;
    a.balance = 0;
    write(fd ,&a , sizeof(struct account));
    close(fd);
    return 0;
}
int signin(int option, char* user_name, char* password)
{
    static struct flock lock;
    lock.l_start = 0;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct user);
    lock.l_pid = getpid();
    char file_name[BUFF_SIZE];
    strcpy(file_name,user_name);
    char extension[5] = ".txt";
    int fd;
    strncat(file_name,extension,sizeof(extension));
    fd = open(file_name,O_RDONLY,0644);
    if(fd == -1)
    {
        perror("signin"); return -1;
    }
    struct user u;
    if(fcntl(fd, F_SETLKW, &lock)==-1) 
    {
        perror("fcntl"); 
        return -1;
    }
    //entering critical section
    lseek(fd,0,SEEK_SET);
    read(fd,&u,sizeof(struct user));
    //The function strncmp() is used to compare left string to right string up to a number.
    if((strcmp(u.password,password) != 0) 
        || (option == USER_SIGN_IN && (strcmp(u.type,"normal") != 0))
        || (option == SIGN_IN_AS_ADMIN && (strcmp(u.type,"admin") != 0))
        || (option == SIGN_IN_AS_JOINT && (strcmp(u.type,"joint") != 0))) 
    {    
        return -1;
    }
    //critical section ended
    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
    close(fd);
    return 0;
}
int withdraw(char* user_name, int amount)
{
    //to withdraw money from the account
    static struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = sizeof(struct user);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct account);
    lock.l_pid = getpid();
    char file_name[BUFF_SIZE];
    strcpy(file_name,user_name);
    char extension[5] = ".txt";
    int fd;
    strncat(file_name , extension , sizeof(extension));
    fd = open(file_name,O_RDWR,0644);
    if(fd == -1)
    {
        perror("signin"); return -1;
    }
    struct account a;
    if(fcntl(fd, F_SETLKW, &lock) == -1) 
    {
        perror("fcntl"); 
        return -1;
    }
    //entering critical section
    lseek(fd,sizeof(struct user),SEEK_SET);
    if(read(fd,&a,sizeof(struct account)) == -1)
    {
        perror("read"); 
        return -1;
    }
    printf("balance = %d\n",a.balance);
    a.balance -= amount;
    if(a.balance < 0) 
    {
        return -1;
    }
    lseek(fd,sizeof(struct user),SEEK_SET);
    if(write(fd,&a,sizeof(struct account)) == -1) 
    {
        perror("write"); 
        return -1;
    }
    //critical section ended
    lock.l_type = F_UNLCK;
    fcntl(fd ,F_SETLKW ,&lock);
    close(fd);
    return 0;
}
int balance(char* user_name)
{
    //to check the balance
    static struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_start = sizeof(struct user);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct account);
    lock.l_pid = getpid();
    char file_name[BUFF_SIZE];
    strcpy(file_name , user_name);
    char extension[5] = ".txt";
    strncat(file_name ,extension ,sizeof(extension));
    int fd = open(file_name,O_RDONLY,0644);
    if(fd == -1)
    {
        perror("signin"); 
        return -1;
    }
    struct account a;
    if(fcntl(fd, F_SETLKW, &lock) == -1) 
    {
        perror("fcntl"); 
        return -1;
    }
    //entering critical section
    lseek(fd, sizeof(struct user), SEEK_SET);
    if(read(fd , &a, sizeof(struct account)) == -1) 
    {    
        perror("read");
    }
    //critical section ended
    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
    close(fd);
    return a.balance;
}
int change_password(char* user_name, char* password)
{
    //it is used to change password to login.
    static struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct user);
    lock.l_pid = getpid();
    char file_name[BUFF_SIZE];
    strcpy(file_name,user_name);
    char extension[5] = ".txt";
    int fd;
    strncat(file_name , extension , sizeof(extension));
    fd = open(file_name , O_RDWR ,0644);
    if(fd == -1)
    {
        perror("change pwd"); return -1;
    }
    struct user u;
    lseek(fd , 0 ,SEEK_SET);
    if(fcntl(fd, F_SETLKW, &lock)==-1) 
    {
        perror("fcntl"); 
        return -1;
    }
    //entering critical section
    if(read(fd,&u,sizeof(struct user)) == -1) 
    {
        perror("read"); 
        return -1; 
    }
    strcpy(u.password,password);
    lseek(fd,0,SEEK_SET);
    if(write(fd,&u,sizeof(struct user))==-1) { perror("write"); return -1; }
    //critical section ended
    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
    close(fd);
    return 0;
}
int deposit(char* user_name, int amt)
{
    static struct flock lock;
    lock.l_start = sizeof(struct user);
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct account);
    lock.l_pid = getpid();
    char filename[BUFF_SIZE];
    strcpy(filename,user_name);
    char extension[5] = ".txt";
    strncat(filename,extension,sizeof(extension));
    int fd = open(filename,O_RDWR,0644);
    if(fd == -1)
    {
        perror("signin"); return -1;
    }
    struct account acc;
    if(fcntl(fd, F_SETLKW, &lock)==-1) 
    {
        perror("fcntl"); 
        return -1;
    }
    //entering critical critical section
    lseek(fd,sizeof(struct user),SEEK_SET);
    if(read(fd,&acc,sizeof(struct account))==-1) 
    {
        perror("read"); 
        return -1;
    }
    acc.balance += amt;
    lseek(fd,sizeof(struct user),SEEK_SET);
    if(write(fd,&acc,sizeof(struct account))==-1) {perror("write"); return -1;}
    //critical section ended
    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
    close(fd);
    return 0;
}
char* details(char* user_name)
{
    //will return details like username,password , balance.
    static struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_pid = getpid();
    char file_name[BUFF_SIZE];
    strcpy(file_name , user_name);
    char extension[5] = ".txt";
    strncat(file_name,extension,sizeof(extension));
    int fd = open(file_name , O_RDWR , 0644);
    if(fd == -1)
    {
        perror("open"); 
        return "user does not exist\n";
    }
    struct account a;
    struct user u;
    if(fcntl(fd, F_SETLKW, &lock) == -1) 
    {
        perror("fcntl"); 
        return "locked\n";
    }
    //entering the critical section
    lseek(fd , 0 , SEEK_SET);
    if(read(fd,&u,sizeof(struct user)) == -1)
    {
        perror("read"); 
        return "can't read the file\n";
    }
    if(read(fd,&a,sizeof(struct account)) == -1)
    {
        perror("read"); 
        return "can't read the file\n";
    }
    //critical section ended
    lock.l_type = F_UNLCK;
    fcntl(fd , F_SETLKW , &lock);
    close(fd);
    char* string = (char*)malloc(BUFF_SIZE * sizeof(char));
    sprintf(string,"username : %s \npassword : %s \ntype : %s\nbalance : %d\n",
    u.user_name , u.password , u.type ,a.balance);
    return string;
}