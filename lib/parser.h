
#if !defined(PARSER_H_)
#define PARSER_H_

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
#include <getopt.h>
#include "./myAPI.h"


#define ec_meno1(r,s)       if((r)==-1){perror(s); return -1;}
#define ec_null(r,s)        if((r)==NULL){perror(s); return -1;}
#define ec_div_zero(r,s)    if((r)!=0){perror(s); return -1;}
#define ec_div_true(r,s)    if((r)!=true) {perror(s); return -1;}
#define ec_meno1_err(r,s) if(r==-1){perror(s); return -1;}
#define ec_null_err(r,s) if((r)==NULL){perror(s); return -1;}
//#define ec_connect(r,s,e)   if(r==-1){if(e==ENOENT) sleep(1); else exit(EXIT_FAILURE);}
#define ec_neg(r,s)       if((r)<0){perror(s); return -1;}

typedef struct coda_richieste{
    int opt;
    char* str;
    char* app;
    int n;
    struct coda_richieste* next;
}c_r;

void option_h();
void option_f(char* optarg);
void option_w(char* optarg);
void option_W(char* optarg);
void option_r(char* optarg);
void option_R(char* optarg);
void option_d(char* optarg);
void option_D(char* optarg);
void option_t(char* optarg);
void option_a(char *optarg);
void option_o(char *optarg);

void option_p();

void push_opt(c_r **q, int op, char *rich, char *s, int n);

c_r *pop_opt(c_r **q);

void clean_coda_opt(c_r **q);
void stampa(c_r**q);

void run_client();
#endif
