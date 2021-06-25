
#if !defined(MYLIB_H_)
#define MY_LIB_H_

#define _POSIX_C_SOURCE 200809L 
#include <stdbool.h>
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
#include <sys/un.h>
#include <sys/time.h>
#include <string.h>
#include <stdatomic.h>
#include <math.h>
#include <sys/wait.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "./icl_hash.h"


static int rimpiazzi_totali=0;

#define PMAX 108

#define ec_meno1(r,s)       if((r)==-1){perror(s); exit(EXIT_FAILURE);}
#define ec_null(r,s)        if((r)==NULL){perror(s); exit(EXIT_FAILURE);}
#define ec_div_zero(r,s)    if((r)!=0){perror(s); exit(EXIT_FAILURE);}
#define ec_div_true(r,s)    if((r)!=true) {perror(s); exit(EXIT_FAILURE);}
#define ec_meno1_err(r,s) if(r==-1){perror(s); exit(errno);}
#define ec_null_err(r,s) if((r)==NULL){perror(s); exit(errno);}
//#define ec_connect(r,s,e)   if(r==-1){if(e==ENOENT) sleep(1); else exit(EXIT_FAILURE);}
#define ec_neg(r,s)       if((r)<0){perror(s); exit(EXIT_FAILURE);}

#define Lock(l, s) if(pthread_mutex_lock(l)!=0){ \
    perror(s); \
    pthread_exit((void*)EXIT_FAILURE); }

#define Unlock(l, s) if(pthread_mutex_unlock(l)!=0){ \
    perror(s); \
    pthread_exit((void*)EXIT_FAILURE); }

#define Wait(c,l,s) if (pthread_cond_wait(c,l)!=0){ \
    perror(s);		    \
    pthread_exit((void*)EXIT_FAILURE);}

#define Twait(c,l,t,s) {							\
    int r=0;								\
    if ((r=pthread_cond_timedwait(c,l,t))!=0 && r!=ETIMEDOUT) {		\
      perror(s);			\
      pthread_exit((void*)EXIT_FAILURE);					\
    }}

#define Signal(c, s)  if (pthread_cond_signal(c)!=0)       {	\
    perror(s);			\
    pthread_exit((void*)EXIT_FAILURE);}

#define Bcast(c,s)     if (pthread_cond_broadcast(c)!=0)    {		\
    perror(s);			\
    pthread_exit((void*)EXIT_FAILURE);}

static int empty=1;

typedef struct conf{
    int NFILE;
    int DIM;
    int NT;
    char* fileLog;
    char* sock;

}conf;

typedef struct arg_sig{
    int fd_p;
    sigset_t *m;
}arg_sig;

typedef struct coda_s{
    int r_fd; //fd_richiedente
    struct coda_s* next;
}coda;

typedef struct arg_s{
    int i;
    int fd_w;
    int fd_r;
}arg;

//inizializza i campi della struct con con i valori letti nel file puntato da conffile
//ritorna c inizializzata se ha successo, NULL altrimenti
conf *myconf(char* conffile);

//restituisce il numero corrispondente al fd maggiore presente in set
int max_fd(fd_set set, int fd_max);

//inserisce in coda a q il valore fd
void push(coda **q, int fd);

//inserisce in testa a q il valore fd 
void push_in_testa(coda **q, int fd);

//preleva il valore in testa a q e lo restituisce
int pop(coda **q);

//elimina tutti gli elementi presenti in q
void clean_coda(coda **q);

ssize_t readn(int fd, void *ptr, size_t n);
ssize_t writen(int fd, void *ptr, size_t n);

//apre il file passato attraverso fd e lo inserisce in ht
//stampa su file di log un riassunto dell'operazione
//comunica al Master attraverso fd_w il valore fd
void openF(int fd, int fd_w, int i, myhash *ht);

//legge il file passato attraverso fd contenuto in ht e restituisce il contenuto al client
//stampa su file di log un riassunto dell'operazione
//comunica al Master attraverso fd_w il valore fd
void readFi(int fd, int fd_w, int i, myhash *ht);

//legge n file contenuti in ht e ne restituisce il contenuto al client
//n è passato dal client attraverso fd
//se n<=0 legge e restituisce tutti i file presenti in ht
//stampa su file di log un riassunto dell'operazione
//comunica al Master attraverso fd_w il valore fd
void readNF(int fd, int fd_w, int i, myhash *ht);

//aggiorna il file passato attraverso fd e contenuto in ht con il contenuto
//anch'esso passato dal client attraverso fd
//include il file nel conteggio dei file e della dimensione se il contenuto!=NULL
//stampa su file di log un riassunto dell'operazione
//comunica al Master attraverso fd_w il valore fd
void writeF(int fd, int fd_w, int i, myhash *ht);

//aggiorna il file passato attraverso fd e contenuto in ht con la stringa
//anch'essa passata dal client attraverso fd
//include il file nel conteggio dei file e della dimensione se il contenuto!=NULL
//se il file non rientrava già in questi calcoli
//stampa su file di log un riassunto dell'operazione
//comunica al Master attraverso fd_w il valore fd
void append(int fd, int fd_w, int i, myhash *ht);

//chiama la funzione hash_dump sul file di log e su ht
//stampa su file di log alcune statistiche dell'esecuzione del programma
//effettua la fclose sul file di log
void chiusura(myhash *ht);

//stampa sul file di log un riassunto dell'operazione di apertura connessione
void stampa_conn(int n);

//stampa sul file di log un riassunto dell'operazione di chiusura connessione
void stampa_close(int n);
//
#endif
