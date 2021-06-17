
#if !defined(MYAPI_H_)
#define MY_API_H_

#define _POSIX_C_SOURCE 200809L 
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/un.h>
#include <sys/time.h>
#include <string.h>
#include <stdatomic.h>
#include <math.h>
#include <sys/wait.h>
#include <stddef.h>
#include <stdint.h>


#define PMAX 108
#define NOFLAG 00
#define O_CREATE 01
#define O_LOCK 02

#define ec_meno1(r,s)       if((r)==-1){perror(s); return -1;}
#define ec_null(r,s)        if((r)==NULL){perror(s); return -1;}
#define ec_div_zero(r,s)    if((r)!=0){perror(s); return -1;}
#define ec_div_true(r,s)    if((r)!=true) {perror(s); return -1;}
#define ec_meno1_err(r,s) if(r==-1){perror(s); return -1;}
#define ec_null_err(r,s) if((r)==NULL){perror(s); return -1;}
//#define ec_connect(r,s,e)   if(r==-1){if(e==ENOENT) sleep(1); else exit(EXIT_FAILURE);}
#define ec_neg(r,s)       if((r)<0){perror(s); return -1;}

typedef struct coda_file{
    char *nome;
    int op;
    struct coda_file* next;
}c_f;


int openConnection(const char* sockname, int msec, const struct timespec abstime);

int closeConnection(const char* sockname);

int openFile(const char* pathname, int flags);

int readFile(const char* pathname, void** buf, size_t* size);

int readNFiles(int N, const char* dirname);

int writeFile(const char* pathname, const char* dirname);

int appendToFile(const char* pathname, void* buf, size_t size, const char* dirname);

int lockFile(const char* pathname);

int unlockFile(const char* pathname);

int closeFile(const char* pathname);

int removeFile(const char* pathname);



//Funzioni Ausiliarie
int timeout(const struct timespec abstime);

ssize_t readn(int fd, void *ptr, size_t n);

ssize_t writen(int fd, void *ptr, size_t n);

void push(c_f **q, const char* pathname);

int pop(c_f **q, const char* pathname);

int reset_o(c_f **q, const char* pathname);

int check_o(c_f **q, const char* pathname);

int clean_q(c_f **q);

int check_p(const char* pathname);

int check_d(const char* dirname);

int saveFile(char* nome, const char* buf, const char* dirname, const char* op);


#endif
