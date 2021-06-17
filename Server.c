#include "./lib/mylib.h"
#include "./lib/icl_hash.h"

#define N 30
#define NB 20 //NB=nbuckets

static conf *c;
volatile sig_atomic_t hup_flag=0;
volatile sig_atomic_t quint_flag=0;
static pthread_mutex_t mtx_fd=PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond_fd=PTHREAD_COND_INITIALIZER;



myhash *ht;
coda* q;

void cleanup();

//static void gestore(int signum);

static void* waiting_sig(void* argum);

static void* work(void* argum);

int main(int argc, char* argv[]){
    if(argc!=2){
        perror("Numero di argomenti non valido");
        exit(EXIT_FAILURE);
    }
    ec_null(c=myconf(argv[1]), "S-Master: errore apertura configurazione");
    int stop=0;
    int mypipe[2];
    arg* args;
    arg_sig a;
    //cleanup();
    atexit(cleanup);
    pthread_t* workers;
    pthread_t sig_th;
    
    printf("%d %d %d, sock=%s, log=%s\n", c->NFILE, c->DIM, c->NT, c->sock, c->fileLog);

    char* sockname;
    ec_null(sockname=malloc(20*sizeof(char)), "S-Master: malloc: sockname");

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGHUP);
    //sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGQUIT);
    ec_div_zero((pthread_sigmask(SIG_BLOCK, &mask, NULL)), "S-Master: errore pthread_sigmask");

    struct sigaction s;
    memset(&s, 0, sizeof(s));
    s.sa_handler=SIG_IGN;
    ec_meno1_err((sigaction(SIGPIPE, &s, NULL)), "S-Master: errore sigaction: SIGPIPE");

    struct sockaddr_un ad;
    strncpy(ad.sun_path, c->sock, PMAX);
    //strncpy(ad.sun_path, "./mysock", PMAX);
    ad.sun_family=AF_UNIX;
    int fd_sk, fd_c, fd_num=0, fd, fd_min;
    int nread;
    fd_set set, rdset;

    ec_meno1_err((fd_sk=socket(AF_UNIX, SOCK_STREAM, 0)), "S-Master: errore socket");
    ec_meno1_err(bind(fd_sk, (struct sockaddr*)&ad, sizeof(ad)), "S-Master: errore bind");
    ec_meno1_err(listen(fd_sk, SOMAXCONN), "S-Master: errore nella listen");
    
    ec_meno1_err(pipe(mypipe), "S-Master: errore pipe");

    ec_null(ht=hash_create(NB, NULL, NULL, c->NFILE, c->DIM), "S-Master_num);r: errore hash_create");
    ec_null(workers=malloc(c->NT*sizeof(pthread_t)), "S-Master: errore malloc: workers");
    ec_null(args=malloc(c->NT*sizeof(arg)), "S-Master: errore malloc: args");
    for(int i=0; i<c->NT; i++){
        args[i].i=i;
        args[i].fd_r=mypipe[0];
        args[i].fd_w=mypipe[1];
        ec_div_zero(pthread_create(&workers[i], (void*)NULL, &work, &args[i]), "S-Master: errore pthread_create");
    }
    a.fd_p=mypipe[1];
    a.m=&mask;
    ec_div_zero(pthread_create(&sig_th, (void*)NULL, &waiting_sig, &a), "S-Master: errore pthread_create");

    if(fd_sk>fd_num) fd_num=fd_sk;
    if(mypipe[0]>fd_num) fd_num=mypipe[0];
    FD_ZERO(&set);
    FD_SET(fd_sk, &set);
    FD_SET(mypipe[0], &set);
    fd_min=fd_num;

    while(!stop){
        
        rdset=set;
        //ec_meno1_err((select(fd_num+1, &rdset, NULL, NULL, NULL)), "S-Master: errore  select");
        int i;
        while(((i=select(fd_num+1, &rdset, NULL, NULL, NULL))==-1) && (errno==EINTR));
        for(fd=0; fd<=fd_num; fd++){
            if(FD_ISSET(fd, &rdset)){
                if(fd==fd_sk){
                    ec_meno1_err((fd_c=accept(fd_sk, NULL, 0)), "S-Master: errore accept");
                    FD_SET(fd_c, &set);
                    if(fd_c>fd_num) fd_num=fd_c;
                }
                else if(fd==mypipe[0]){
                    int thisfd;
                    ec_meno1_err(readn(mypipe[0], &thisfd, sizeof(int)), "Worker: writen");
                    if(thisfd==-1){
                        Lock(&mtx_fd, "S-Master: errore lock");
                        push(&q, -1);
                        Signal(&cond_fd, "S-Master: errore signal");
                        Unlock(&mtx_fd, "S-Master: errore unlock");
                        stop=1;
                    }
                    else if(thisfd==-2){
                        Lock(&mtx_fd, "S-Master: errore lock");
                        push_in_testa(&q, -2);
                        printf("fatto push -2\n");
                        Signal(&cond_fd, "S-Master: errore signal");
                        Unlock(&mtx_fd, "S-Master: errore unlock");
                        stop=1;
                    }
                    else{
                        FD_SET(thisfd, &set);
                        if(thisfd>fd_num) fd_num=thisfd;
                    }
                }
                else{
                    FD_CLR(fd, &set);
                    fd_num=max_fd(set, fd_num);
                    Lock(&mtx_fd, "S-Master: errore lock");
                    push(&q, fd);
                    Signal(&cond_fd, "S-Master: errore signal");
                    Unlock(&mtx_fd, "S-Master: errore unlock");
                }
            }
        }
    }
    
    for(int i=0; i<c->NT; i++){
        ec_div_zero(pthread_join(workers[i], NULL), "S-Master: errore join");
        //free(workers[i]);
    }
    ec_div_zero(pthread_join(sig_th, NULL), "S-Master: errore join");
    printf("fd_min=%d, fd_num=%d\n", fd_min, fd_num);
    for(int i=fd_min+1; i<=fd_num; i++){
        ec_meno1_err(writen(i, "NO", 3), "S-Master: writen");
        close(i);
    }
    close(fd_sk);
    hash_destroy(ht, free,free);
    if(empty!=1) clean_coda(&q);
    free(args);
    free(q);
    free(workers);
    free(sockname);
    //cleanup();
    
    close(mypipe[0]);
    close(mypipe[1]);
    return 0;
}


void cleanup(){
    unlink(c->sock);
    free(c->fileLog);
    free(c->sock);
    free(c);
    //unlink("mysock");
}
static void* waiting_sig(void* argum){
    arg_sig a=*(arg_sig*)argum;
    int fd=0;
    int sig;
    int r;
    
    ec_meno1_err(r=sigwait(a.m, &sig), "Waiting_sig: sigwait");
    
    if(sig==SIGHUP){
        fd=-1;
        ec_meno1_err(writen(a.fd_p, &fd, sizeof(int)), "Worker: writen");
    }
    else if(sig==SIGQUIT){
        printf("wait_sig: arrivato lui\n");
        fd=-2;
        ec_meno1_err(writen(a.fd_p, &fd, sizeof(int)), "Worker: writen");
    }
}

static void* work(void* argum){
    arg a=*(arg*)argum;
    int fd;
    char *write_buf;
    char *read_buf;
    ec_null(write_buf=malloc(3*sizeof(char)), "Worker: malloc write_buf");
    ec_null(read_buf=malloc(7*sizeof(char)), "Worker: malloc read_buf");
    while(1){
        Lock(&mtx_fd, "Worker: errore lock");
        while(q==NULL){
            Wait(&cond_fd, &mtx_fd, "Worker: errore wait");
        }
        fd=pop(&q);
        Signal(&cond_fd, "Worker: errore signal");
        Unlock(&mtx_fd, "Worker: errore unlock");
        if((fd==-1) || (fd==-2)){
            printf("Work-%d, visto %d\n", a.i, fd);
            free(write_buf);
            free(read_buf);
            close(a.fd_w);
            return (void*)0;
        }
        
        ec_meno1(readn(fd, (void*) read_buf, 7), "Worker: readn");
        
        
        if(strncmp(read_buf, "closeC", 7)==0){
            ec_meno1(writen(fd, (void*)"OK", 3), "Worker: writen");
            close(fd);
            printf("Worker-%d: %s fatta\n", a.i, read_buf);
            }
        
        else if(strncmp(read_buf, "openF", 7)==0){
            openF(fd, a.fd_w, a.i, ht);
            }

        else if(strncmp(read_buf, "readFi", 7)==0){
            readFi(fd, a.fd_w, a.i, ht);
            }
        
        else if(strncmp(read_buf, "readNF", 7)==0){
            readNF(fd, a.fd_w, a.i, ht);
            }

        else if(strncmp(read_buf, "writeF", 7)==0){
            writeF(fd, a.fd_w, a.i, ht);
            //printf("Write\n");
            //print(ht);
        }
        else if(strncmp(read_buf, "append", 7)==0){
            append(fd, a.fd_w, a.i, ht);
            //printf("Append\n");
            //print(ht);
        }
        /*else if(strncmp(read_buf, "closeF", 7)==0){
            closeF(fd, a.fd_w, a.i, ht);
            
        }*/
        else{
            ec_meno1(writen(fd, (void*)"NO", 3), "S-Master: writen");
        }      
    }
}
