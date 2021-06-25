
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
#include <dirent.h>
#include "./myAPI.h"


#define ec_meno1(r,s)       if((r)==-1){perror(s); return -1;}
#define ec_null(r,s)        if((r)==NULL){perror(s); return -1;}
#define ec_div_zero(r,s)    if((r)!=0){perror(s); return -1;}
#define ec_div_true(r,s)    if((r)!=true) {perror(s); return -1;}
#define ec_meno1_err(r,s) if(r==-1){perror(s); return -1;}
#define ec_null_err(r,s) if((r)==NULL){perror(s); return -1;}
#define ec_null_v(r,s)        if((r)==NULL){perror(s); return;}
//#define ec_connect(r,s,e)   if(r==-1){if(e==ENOENT) sleep(1); else exit(EXIT_FAILURE);}
#define ec_neg(r,s)       if((r)<0){perror(s); return -1;}

typedef struct coda_richieste{
    int opt;
    char* str;
    char* app;
    int n;
    struct coda_richieste* next;
}c_r;

//stampa su stdout l'elenco dei comandi con relativi argomenti
void option_h();

//inizializza la variabile statica sock della libreria con l'argomento di -f
void option_f(char* optarg);

//inizializza la variabile statica tempo della libreria con l'argomento di -t
void option_t(char* optarg);

//inizializza la variabile statica dir_lettura della libreria con l'argomento di -d
void option_d(char* optarg);

//inizializza la variabile statica dir_scrittura della libreria con l'argomento di -D
void option_D(char* optarg);

//inizializza la variabile statica flag_p della libreria con 1
void option_p();

//parsa optarg per ottenere gli argomenti necessari
//invoca push_opt con op=2, n>=0
void option_R(char* optarg);

//parsa optarg per ottenere gli argomenti necessari
//invoca push_opt con op=1, n>=0
void option_w(char* optarg);

//parsa optarg per ottenere gli argomenti necessari
//invoca push_opt con op=1, n=-1
void option_W(char* optarg);

//parsa optarg per ottenere gli argomenti necessari
//invoca push_opt con op=2, n=-1
void option_r(char* optarg);

//parsa optarg per ottenere gli argomenti necessari
//invoca push_opt con op=3, n=-1
void option_a(char *optarg);

//parsa optarg per ottenere gli argomenti necessari
//invoca push_opt con op=0, n=1 V n=2 V n=3
void option_o(char *optarg);


//crea elemento richiesta e inserisce per ordine crescente di op nella coda q
void push_opt(c_r **q, int op, char *rich, char *s, int n);

//preleva in testa alla coda q
c_r *pop_opt(c_r **q);

//elimina gli elementi presenti in coda q
void clean_coda_opt(c_r **q);

//stampa gli elementi presente nella coda q
void stampa(c_r**q);

//visita ricorsivamente la cartella dirname finche n=t
//oppure, se n<0, fino a che non ha visitato tutta la cartella e tutte le sotto cartelle
int findFile(char *dirname, int n, int tro);

//ciclo while in cui scorre la coda q e chiama le funzioni di myAPI in modo appropriato
void run_client();
#endif
