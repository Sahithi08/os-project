#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "constants.h"
extern int del_user(char*);
extern int signup(int , char* , char*);
int del_user(char* user_name)
{
    static struct flock lock;
    int fd;
    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_pid = getpid();
    char file_name[BUFF_SIZE];
    strcpy(file_name , user_name);
    char extension[5] = ".txt";
    strncat(file_name , extension , sizeof(extension));
    fd = open(file_name , O_RDWR , 0644);
    if(fd == -1)
    {
        perror("open");
    }
    if(fcntl(fd , F_SETLKW , &lock) == -1)
    {
        perror("open");
    }
    return unlink(file_name);
}
int modify_user(char* user_name, char* new_user,char* password)
{
    int fd,option;
    static struct flock lock;
    lock.l_start = 0;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_pid = getpid();
    char file_name[BUFF_SIZE];
    strcpy(file_name,user_name);
    char extension[5] = ".txt";
    strncat(file_name,extension,sizeof(extension));
    fd = open(file_name,O_RDWR,0644);
    if(fd == -1)
    {
        perror("mod user"); 
        return -1;
    }
    struct user u;
        /* data */
        if(fcntl(fd, F_SETLKW, &lock) == -1)
        {
            perror("fcntl");
        }
        //entering critical section
        lseek(fd,0,SEEK_SET);
        if(read(fd , &u , sizeof(struct user)) == -1)
        {
            perror("read"); 
            return -1; 
        }
        del_user(user_name);
        if(strcmp(u.type,"normal") == 0) option = USER_SIGN_UP;
        else option = SIGN_UP_AS_JOINT;
        strcpy(u.user_name,new_user);
        signup(option,new_user,password);
        //critical section has ended
        lock.l_type = F_UNLCK;
        fcntl(fd,F_SETLKW,&lock);
        close(fd);
        return 0;
}