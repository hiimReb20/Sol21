#include "./myAPI.h"
#define M 30
static int fd_sk=-1;
static int empty=1;
c_f *q;


int openConnection(const char* sockname, int msec, const struct timespec abstime){
    if(sockname==NULL || msec<0){
        perror("myAPI: openConnection: parametri non validi");
        errno=EINVAL;
        return -1;
    }
    char buf[M];
    struct sockaddr_un ad;
    strncpy(ad.sun_path, sockname, PMAX);
    ad.sun_family=AF_UNIX;
    
    if((fd_sk=socket(AF_UNIX, SOCK_STREAM, 0))==-1){
        perror("myAPI: openConnection: errore socket");
        return -1;
    }
    int x=1;
    while(connect(fd_sk, (struct sockaddr*)&ad, sizeof(ad))==-1){
        
        if(errno=ENOENT){
            if(timeout(abstime)){
                unsigned int w=msec;
                struct timespec wait_for_connection={0, w};
                nanosleep(&wait_for_connection, NULL);
                x++;}
            else{
                perror("myAPI: openConnection: timeout");
                errno=ETIMEDOUT;
                return -1;
            }
            }else{
            perror("myAPI: openConnection: errore connect");
            errno=ENOTSOCK;
            return -1;
        }
    }
    //attenddere richiesta
    printf("Socket creata dopo %d prove con socket=%d\n", x, fd_sk);
    return 0;
}

int closeConnection(const char* sockname){
    if(sockname==NULL ){
        perror("myAPI: closeConnection: parametri non validi");
        errno=EINVAL;
        return -1;
    }
    /*if(strcmp(sockname, "./mysock1")!=0){
        perror("myAPI: closeConnection: socket inesistente");
        errno=ENOENT;
        return -1;
    }*/
    if(fd_sk==-1){
        perror("myAPI: closeConnection: fd_sk non inizializzato");
        errno=EINVAL;
        return -1;
    }
    char* write_buf;
    char* read_buf;
    if((write_buf=malloc(7*sizeof(char)))==NULL){
       perror( "myAPI: closeConnection: malloc write_buf");
       errno=ENOMEM;
       return -1;
    }
    if((read_buf=malloc(3*sizeof(char)))==NULL){
       perror("myAPI: closeConnection: malloc read_buf");
       errno=ENOMEM;
       free(write_buf);
       return -1; 
    }
    strncpy(write_buf, "closeC", 7);

    if(writen(fd_sk, (void*) write_buf, 7)==-1){
        perror("myAPI: closeConnection: writen");
        free(read_buf);
        free(write_buf);
        return -1;
    }

    if(writen(fd_sk, (void*) write_buf, 7)==-1){
        perror("myAPI: closeConnection: writen");
        free(read_buf);
        free(write_buf);
        return -1;
    }
    
    if(readn(fd_sk, (void*) read_buf, 3)==-1){
        perror("myAPI: closeConnection: readn");
        free(read_buf);
        free(write_buf);
        return -1;

    }
    //riferire al server e attebdere richiesta
    if(close(fd_sk)==-1){
        perror("myAPI: closeConnection: errore close");
        free(read_buf);
        free(write_buf);
        return -1;        
    }
    free(write_buf);
    free(read_buf);
    if(clean_q(&q)==-1){
        perror("myAPI: closeConnection: clean_q non andata a buon fine");
        errno=EINTR;
        return -1;
    }
    printf("CloseC ok\n");
    return 0;
}
int openFile(const char* pathname, int flags){
    if(pathname==NULL){
        perror("myAPI: pathname è NULL");
        errno=EINVAL;
        return -1;
    }
    if(check_p(pathname)==-1){
        perror("myAPI: openFile: pathname non è un path assoluto");
        errno=EINVAL;
        return -1;
    }

    /*struct stat info;
    if(stat(pathname, &info)==-1){
        perror("openFile: pathname non esiste");
        errno=ENOENT;
        return -1;
    }
    if(!S_ISREG(info.st_mode)){
        perror("openFile: pathname non è un file");
        errno=ENOTDIR;
        return -1;
    }*/
    
    if(check_o(&q, pathname)!=-1){
        perror("myAPI: openFile: File già aperto");
        errno=EBADF;
        return -1;
    }

    char* write_buf;
    char* read_buf;
    int presente;
    int msg;
    if((write_buf=malloc(100*sizeof(char)))==NULL){
        perror("myAPI: openFile: malloc write_buf");
        errno=ENOMEM;
        return -1;
    }
    
    if((read_buf=malloc(100*sizeof(char)))==NULL){
        perror("myAPI: openFile: malloc read_buf");
        errno=ENOMEM;
        free(write_buf);
        return -1;
    }
    strncpy(write_buf, "openF", 7);
    
    if(flags==O_LOCK || flags==(O_CREATE | O_LOCK)){
        free(write_buf);
        free(read_buf);
        perror("myAPI: openFile: operazione momentaneamente non disponibile");
        errno=EPERM;
        return -1;
    }

    else if(flags==O_CREATE){
        if(writen(fd_sk, (void*) write_buf, 7)==-1){
            perror("myAPI: openFile: writen");
            free(write_buf);
            free(read_buf);
            return -1;

        }
        if(readn(fd_sk, (void*) read_buf, 3)==-1){
            perror("myAPI: openFile: readn");
            free(write_buf);
            free(read_buf);
            return -1;
        }
        if(strcmp(read_buf, "OK")!=0){
            free(write_buf);
            free(read_buf);
            perror("myAPI: Server ha detto no");
            errno=EEXIST;
            return -1;
            }
        
        int l=strlen(pathname);
        if(writen(fd_sk, &l, sizeof(int))==-1){
            perror("myAPI: openFile: writen");
            free(write_buf);
            free(read_buf);
            return -1;
            }
        strncpy(write_buf, pathname, strlen(pathname)+1);
        
        if(writen(fd_sk, (void*) write_buf, l+1)==-1){
            perror("myAPI: openFile: writen");
            free(write_buf);
            free(read_buf);
            return -1;

        }
        if(readn(fd_sk, &presente, sizeof(int))==-1){
            perror("myAPI: openFile: readn");
            free(write_buf);
            free(read_buf);
            return -1;
        }
        if(presente!=0){
            free(write_buf);
            free(read_buf);
            msg=0;
            if(writen(fd_sk, &msg, sizeof(int))==-1){
                perror("myAPI: openFile: writen");
                return -1;}
            perror("myAPI: openFIle: O_CREATE  e file è già presente nello Storage");
            errno=EBADMSG;
            return -1;
            }
        msg=1;
        if(writen(fd_sk, &msg, sizeof(int))==-1){
            perror("myAPI: openFile: writen");
            free(write_buf);
            free(read_buf);
            return -1;}
        if(readn(fd_sk, (void*) read_buf, 3)==-1){
            perror("myAPI: openFile: readn");
            free(write_buf);
            free(read_buf);
            return -1;
        }
        if(strcmp(read_buf, "OK")!=0){
            free(write_buf);
            free(read_buf);
            perror("myAPI: Server ha detto no");
            errno=EEXIST;
            return -1;
            }
        push(&q, pathname);
    }
    else if(flags==NOFLAG){
        if(writen(fd_sk, (void*) write_buf, 7)==-1){
            perror("myAPI: openFile: writen");
            free(write_buf);
            free(read_buf);
            return -1;

        }
        if(readn(fd_sk, (void*) read_buf, 3)==-1){
            perror("myAPI: openFile: readn");
            free(write_buf);
            free(read_buf);
            return -1;
        }
        if(strcmp(read_buf, "OK")!=0){
            free(write_buf);
            free(read_buf);
            perror("myAPI: openFIle: Server ha detto no");
            errno=EEXIST;
            return -1;
            }
        
        int l=strlen(pathname);
        if(writen(fd_sk, &l, sizeof(int))==-1){
            perror("myAPI: openFile: writen");
            free(write_buf);
            free(read_buf);
            return -1;
            }
        strncpy(write_buf, pathname, strlen(pathname)+1);
        
        if(writen(fd_sk, (void*) write_buf, l+1)==-1){
            perror("myAPI: openFile: writen");
            free(write_buf);
            free(read_buf);
            return -1;

        }
        if(readn(fd_sk, &presente, sizeof(int))==-1){
            perror("myAPI: openFile: readn");
            free(write_buf);
            free(read_buf);
            return -1;
        }
        if(presente!=1){
            free(write_buf);
            free(read_buf);
            msg=0;
            if(writen(fd_sk, &msg, sizeof(int))==-1){
                perror("myAPI: openFile: writen");
                return -1;}
            perror("myAPI: openFIle: O_CREATE  e file è già presente nello Storage");
            errno=EBADMSG;
            return -1;
            }
        msg=1;
        if(writen(fd_sk, &msg, sizeof(int))==-1){
            perror("myAPI: openFile: writen");
            free(write_buf);
            free(read_buf);
            return -1;}
        if(readn(fd_sk, (void*) read_buf, 3)==-1){
            perror("myAPI: openFile: readn");
            free(write_buf);
            free(read_buf);
            return -1;
        }
        if(strcmp(read_buf, "OK")!=0){
            free(write_buf);
            free(read_buf);
            perror("myAPI: Server ha detto no");
            errno=EEXIST;
            return -1;
            }
        push(&q, pathname);
    }
    else{
        perror("myAPI: openFile: flag non valido");
        errno=EINVAL;
        free(write_buf);
        free(read_buf);
        errno=EPERM;
        return -1;
    }


    
    free(write_buf);
    free(read_buf);
    printf("openfile ok\n");
    return 0;
}
int readFile(const char* pathname, void** buf, size_t* size){
    if(pathname==NULL || buf==NULL || size<0){
        perror("myAPI: readFile: parametri non validi");
        errno=EINVAL;
        return -1;
    }
    if(check_o(&q, pathname)==-1){
        perror("readFile: File non aperto");
        errno=EBADF;
        return -1;
    }
    char* b;
    char* read_buf;
    int lung, l;
    if((read_buf=malloc(100*sizeof(char)))==NULL){
        perror("myAPI: readFile: malloc read_buf");
        errno=ENOMEM;
        return -1;
    }
    
    if(writen(fd_sk, (void*) "readFi", 7)==-1){
        perror("myAPI: readFile: writen");
        free(read_buf);
        return -1;
    }
    if(readn(fd_sk, (void*) read_buf, 3)==-1){
        perror( "myAPI: readFile: readn");
        free(read_buf);
        return -1;

    }
    if(strcmp(read_buf, "OK")!=0){
        perror("myAPI: readFIle: operazione non andata a buon fine");
        errno=EBADMSG;
        free(read_buf);
        return -1;
        }
    l=strlen(pathname);
    if(writen(fd_sk, &l, sizeof(int))==-1){
        perror("myAPI: readFile: writen");
        free(read_buf);
        return -1;
    }
     
    if(writen(fd_sk, (void*) pathname, strlen(pathname)+1)==-1){
        perror("myAPI: readFile: writen");
        free(read_buf);
        return -1;
    }
     
    
    if(readn(fd_sk, (void*) read_buf, 3)==-1){
        perror("myAPI: readFile: readn");
        free(read_buf);
        return -1;
    } 
    if(strcmp(read_buf, "OK")!=0){
        free(read_buf);
        perror("myAPI: readFile: operazione non andata a buon fine");
        errno=EBADMSG;
        return -1;
        }
    

    if(readn(fd_sk, &lung, sizeof(int))==-1){
        perror("myAPI: readFile: readn");
        free(read_buf);
        return -1;
    }

    if((b=malloc((lung+1)*sizeof(char)))==NULL){
        perror("myAPI: readFile: malloc b");
        free(read_buf);
        errno=ENOMEM;
        return -1;
    }
    
    if(readn(fd_sk, b, lung+1)==-1){
        perror("myAPI: readFile: readn");
        free(read_buf);
        return -1;
    }
    *(size)=lung+1;
    *buf=(void*)b;
    if(readn(fd_sk, (void*) read_buf, 3)==-1){
        perror("myAPI: readFile: readn");
        free(read_buf);
        return -1;
    }
    if(strcmp(read_buf, "OK")!=0){
        free(read_buf);
        perror("myAPI: readFile: operazione non andata a buon fine");
        errno=EBADMSG;
        return -1;
        }
    //free(b);
    //qui andrà scritto e poi cancellato il buffer
    free(read_buf);
    int r=reset_o(&q, pathname);
    if(r==1){
        perror("myAPI: readFile: reset_o");
        errno=EBADMSG;
        return -1;
    }
    else if(r==-1){
        perror("myAPI: readFile: file non nella coda");
        errno=ESPIPE;
        return -1;
    }
    printf("readfile ok\n");
    return 0;

}

int readNFiles(int N, const char* dirname){
    if(dirname==NULL ){
        perror("myAPI: readNFiles: parametri non validi");
        errno=EINVAL;
        return -1;
    }
    if(check_d(dirname)==-1){
        perror("myAPI: readNFiles: dirname non è una directory");
        errno=EINVAL;
        return -1;
    }
    char* read_buf;
    int p;

    if((read_buf=malloc(3*sizeof(char)))==NULL){
        perror("myAPI: readNFiles: malloc read_buf");
        errno=ENOMEM;
        return -1;
    }
     


    if(writen(fd_sk, (void*)"readNF", 7)==-1){
        perror("myAPI: readNFiles: writen");
        free(read_buf);
        return -1;
    }

    if(readn(fd_sk, (void*) read_buf, 3)==-1){
        perror("myAPI: readNFiles: readn");
        free(read_buf);
        return -1;
        }
    
    if(strcmp(read_buf, "OK")!=0){
        free(read_buf);
        perror("myAPI: readNFIles: operazione non andata a buon fine");
        errno=EBADMSG;
        return -1;
            }
          
    if(writen(fd_sk, &N, sizeof(int))==-1){
        perror("myAPI: readNFiles: writen");
        free(read_buf);
        return -1;
    }


    
    if(readn(fd_sk, (void*)&p, sizeof(int))==-1){
        perror("myAPI: readNFiles: readn");
        free(read_buf);
        return -1;
    }
    
    for(int i=0; i<p; i++){
        char* nome;
        char* buf;
        int lung_cont, lung_nome;
        //mi dice lunghezza nome
        if(readn(fd_sk, &lung_nome, sizeof(int))==-1){
            perror("myAPI: readNFiles: readn");
            free(read_buf);
            return -1;
        }

    
        //mi dice lunghezza contenuto
        if(readn(fd_sk, &lung_cont, sizeof(int))==-1){
            perror("myAPI: readNFiles: readn");
            free(read_buf);
            return -1;
        }

        if(lung_cont!=0){
            if((buf=malloc((lung_cont+1)*sizeof(char)))==NULL){
                perror("myAPI: readNFiles: malloc buf");
                free(read_buf);
                return -1;
            }
        }
        if((nome=malloc((lung_nome+1)*sizeof(char)))==NULL){
            perror("myAPI: readNFiles: malloc buf\n");
            free(read_buf);
            free(buf);
            return -1;
            }
        
        //mi dice nome file
        if(readn(fd_sk, (void*) nome, lung_nome+1)==-1){
            perror("myAPI: readNFiles: readn");
            free(read_buf);
            free(buf);
            free(nome);
            return -1;
        }
        //mi dice contenuto file
        if(lung_cont!=0) ec_meno1_err(readn(fd_sk, (void*) buf, lung_cont+1), "myAPI: readNFiles: readn");
        
        int ret;

        if(lung_cont!=0) ret=saveFile(nome, buf, dirname, "readN");
        else ret=saveFile(nome, NULL, dirname, "readN");

        if(lung_cont!=0) free(buf);
        free(nome);
    }
    //free(write_buf);
    free(read_buf);
    printf("readnfile ok\n");
    return 0;

}

int writeFile(const char* pathname, const char* dirname){
    if(pathname==NULL){
        perror("myAPI: writeFile: parametri non validi");
        errno=EINVAL;
        return -1;
    }

    //poi dovrà essere eliminata perchè già in openFile
    struct stat info;
    if(stat(pathname, &info)==-1){
        perror("writeFile: pathname non esiste");
        errno=ENOENT;
        return -1;
    }
    if(!S_ISREG(info.st_mode)){
        perror("writeFile: pathname non è un file");
        errno=ENOTDIR;
        return -1;
    }
    int d_n=0;
    if(dirname==NULL)
        d_n=1;
    if(check_p(pathname)==-1){
        perror("myAPI: writeFile: pathname non è un path assoluto");
        errno=EINVAL;
        return -1;
    }
    if(d_n==0 && check_d(dirname)==-1){
        perror("myAPI: writeFile: dirname non è una directory");
        return -1;
    }

    int res_o;
    if((res_o=check_o(&q, pathname))==-1){
        perror("myAPI: writeFile: operazione non valida: l'ultima operazione effettuata non è openFile");
        errno=EINVAL;
        return -1;
    }
    if(res_o==0){
        perror("myAPI: writeFile: file non aperto");
        errno=EINVAL;
        return -1;
    }
    char* write_buf;
    char* read_buf;
    char* cont;
    int l;
    FILE *f;
    ec_null(write_buf=malloc(100*sizeof(char)), "myAPI: writeFile: malloc write_buf");
    ec_null(read_buf=malloc(100*sizeof(char)), "myAPI: writeFile: malloc read_buf");

    strncpy(write_buf, "writeF", 7);
    
    //mando al server l'operazione e il serve risponde
    ec_meno1_err(writen(fd_sk, (void*) write_buf, 7), "myAPI: writeFile: writen");
    ec_meno1_err(readn(fd_sk, (void*) read_buf, 3), "myAPI: writeFile: readn");
    if(strcmp(read_buf, "OK")!=0){
        free(write_buf);
        free(read_buf);
        perror("myAPI: writeFile: operazione non andata a buon fine");
        errno=EBADMSG;
        return -1;
        }

    l=strlen(pathname);
    strncpy(write_buf, pathname, l+1);
    
    ec_meno1_err(writen(fd_sk, &l, sizeof(int)), "myAPI: writeFile: writen");
    ec_meno1_err(writen(fd_sk, (void*) write_buf, l+1), "myAPI: writeFile: writen");
    
    ec_null_err(f=fopen(pathname,"r"), "myAPI: writeFile: fopen");
    ec_null(cont=malloc(256*sizeof(char)), "myAPI: writeFile: malloc cont");
    
    ec_null(fgets(cont, 256, f), "myAPI: writeFile: fgets");
    printf("\n\n\ncont=%s\n\n\n", cont);
    ec_div_zero(fclose(f), "saveFile: fclose");
    if(cont!=NULL) l=strlen(cont);
    else l=0;

    ec_meno1_err(writen(fd_sk, &l, sizeof(int)), "myAPI: writeFile: writen");
    if(l!=0) ec_meno1_err(writen(fd_sk, (void*) cont, l+1), "myAPI: writeFile: writen");
    
    ec_meno1_err(readn(fd_sk, (void*) read_buf, 3), "myAPI: writeFile: readn");
    if(strcmp(read_buf, "OK")!=0){
        free(write_buf);
        free(read_buf);
        perror("myAPI: writeFile: il Server ha detto no");
        errno=EBADMSG;
        return -1;
        }
    //voglio sapere se mi manderà file o no
    int rimpiazzo;
    ec_meno1_err(readn(fd_sk, &rimpiazzo, sizeof(int)), "myAPI: writeFile: readn");
    while(rimpiazzo){
        int lung_cont, lung_nome;
        char* buf;
        char* nome;
            
        ec_meno1_err(readn(fd_sk, &lung_nome, sizeof(int)), "myAPI: writeFile: readn");
        ec_meno1_err(readn(fd_sk, &lung_cont, sizeof(int)), "myAPI: writeFile: readn");
            
        if(lung_cont!=0) ec_null(buf=malloc((lung_cont+1)*sizeof(char)), "myAPI: writeFile: malloc buf");
        ec_null(nome=malloc((lung_nome+1)*sizeof(char)), "myAPI: writeFile: malloc buf");
   
        ec_meno1_err(readn(fd_sk, (void*) nome, lung_nome+1), "myAPI: writeFile: readn");
        if(lung_cont!=0)ec_meno1_err(readn(fd_sk, (void*) buf, lung_cont+1), "myAPI: writeFile: readn");
        int ret;
        if(!d_n){
            int ret;
            if(lung_cont==0) ret=saveFile(nome, NULL, dirname, "append");
            else ret=saveFile(nome, buf, dirname, "append");
        }
        if(check_o(&q, nome)!=-1) pop(&q, nome);
        if(lung_cont!=0) free(buf);
        free(nome);
        ec_meno1_err(readn(fd_sk, &rimpiazzo, sizeof(int)), "myAPI: writeFile: readn");
        }
    
    free(write_buf);
    free(read_buf);
    int r=reset_o(&q, pathname);
    if(r==1){
        perror("myAPI: writeFile: reset_o");
        errno=EBADMSG;
        return -1;
    }
    else if(r==-1){
        perror("myAPI: writeFile: file non nella coda");
        errno=ESPIPE;
        return -1;
    }
    printf("writefile ok\n");
    return 0;
}
int appendToFile(const char* pathname, void* buf, size_t size, const char* dirname){
    if(pathname==NULL || buf==NULL || size<0){
        perror("myAPI: appendToFile: parametri non validi");
        errno=EINVAL;
        return -1;
    }
    int d_n=0;
    if(dirname==NULL)
        d_n=1;
    if(d_n==0 && check_d(dirname)==-1){
        perror("myAPI: appendToFile: dirname non è una directory");
        errno=EINVAL;
        return -1;
    }
    if(size!=(strlen(buf))){
        perror("myAPI: appendToFile: size e dimensione buffer non sono coerenti");
        errno=EINVAL;
        return -1;
    }

    if(check_o(&q, pathname)==-1){
        perror("myAPI: appendToFile: file non aperto");
        errno=EBADF;
        return -1;
    }
    
    char* write_buf;
    char* read_buf;
    int rimpiazzo;
    int l;
    ec_null(write_buf=malloc(100*sizeof(char)), "myAPI: appendToFile: malloc write_buf");
    ec_null(read_buf=malloc(100*sizeof(char)), "myAPI: appendToFile: malloc read_buf");

    strncpy(write_buf, "append", 7);
    
    //mando al server l'operazione e il serve risponde
    ec_meno1_err(writen(fd_sk, (void*) write_buf, 7), "myAPI: appendToFile: writen");
    ec_meno1_err(readn(fd_sk, (void*) read_buf, 3), "myAPI: appendToFile: readn");
    if(strcmp(read_buf, "OK")!=0){
        free(write_buf);
        free(read_buf);
        perror("myAPI: appendToFile: operazione non andata a buon fine");
        errno=EBADMSG;
        return -1;
        }
    //mando dimensione nome path
    l=strlen(pathname);
    ec_meno1_err(writen(fd_sk, &l, sizeof(int)), "myAPI: appendToFile: writen");
    ec_meno1_err(writen(fd_sk, (void*) pathname, l+1), "myAPI: appendToFile: writen");
    ec_meno1_err(readn(fd_sk, (void*) read_buf, 3), "myAPI: appendToFile: readn");
    if(strcmp(read_buf, "OK")!=0){
        free(write_buf);
        free(read_buf);
        perror("myAPI: appendToFile: operazione non andata a buon fine");
        errno=EBADMSG;
        return -1;
        }
    //mando al server la dimensione del buffer da scrivere
    ec_meno1_err(writen(fd_sk, &size, sizeof(size_t)), "myAPI: appendToFile: writen");
    
    //mando stringa
    strncpy(write_buf, buf, strlen(buf)+1);
    ec_meno1_err(writen(fd_sk, (void*) write_buf, strlen(buf)+1), "myAPI: appendToFile: writen");
    ec_meno1_err(readn(fd_sk, (void*) read_buf, 3), "myAPI: appendToFile: readn");
    if(strcmp(read_buf, "OK")!=0){
        free(write_buf);
        free(read_buf);
        perror("myAPI: appendToFile: operazione non andata a buon fine");
        errno=EBADMSG;
        return -1;
        }
    //voglio sapere se mi manderà file o no
    ec_meno1_err(readn(fd_sk, &rimpiazzo, sizeof(int)), "myAPI: appendToFile: readn");
    
    while(rimpiazzo){
        int lung_cont, lung_nome;
        char* buf;
        char* nome;
            
        ec_meno1_err(readn(fd_sk, &lung_nome, sizeof(int)), "myAPI: appendToFile: readn");
        ec_meno1_err(readn(fd_sk, &lung_cont, sizeof(int)), "myAPI: appendToFile: readn");
            
        if(lung_cont!=0) ec_null(buf=malloc((lung_cont+1)*sizeof(char)), "myAPI: appendToFile: malloc buf");
        ec_null(nome=malloc((lung_nome+1)*sizeof(char)), "myAPI: appendToFile: malloc buf");
           
        ec_meno1_err(readn(fd_sk, (void*) nome, lung_nome+1), "myAPI: appendToFile: readn");
        if(lung_cont!=0) ec_meno1_err(readn(fd_sk, (void*) buf, lung_cont+1), "myAPI: appendToFile: readn");
        if(!d_n){
            int ret;
            if(lung_cont==0) ret=saveFile(nome, NULL, dirname, "append");
            else ret=saveFile(nome, buf, dirname, "append");
        }
        if(check_o(&q, nome)!=-1) pop(&q, nome);
        if(lung_cont!=0) free(buf);
        free(nome);
        ec_meno1_err(readn(fd_sk, &rimpiazzo, sizeof(int)), "myAPI: appendToFile: readn");
        }
    
    int r=reset_o(&q, pathname);
    if(r==1){
        perror("myAPI: appendToFile: reset_o");
        errno=EINVAL;
        free(write_buf);
        free(read_buf);
        return -1;
    }
    else if(r==-1){
        perror("myAPI: appendToFile: file non nella coda");
        errno=ESPIPE;
        free(write_buf);
        free(read_buf);
        return -1;
    }
    printf("append ok\n");
    free(write_buf);
    free(read_buf);
    return 0;
}
int lockFile(const char* pathname){
    perror("lockFile:funzionalità momentaneamente non disponibile");
    errno=EPERM;
    return -1;
}
int unlockFile(const char* pathname){
    perror("unlockFile:funzionalità momentaneamente non disponibile");
    errno=EPERM;
    return -1;
}
int closeFile(const char* pathname){
    /*
    char* write_buf;
    char* read_buf;
    int l;
    ec_null(write_buf=malloc(7*sizeof(char)), "myAPI: closeFile: malloc write_buf");
    ec_null(read_buf=malloc(100*sizeof(char)), "myAPI: closeFile: malloc read_buf");

    strncpy(write_buf, "closeF", 7);
    
    //mando al server l'operazione e il serve risponde
    ec_meno1_err(writen(fd_sk, (void*) write_buf, 7), "myAPI: closeFile: writen");
    ec_meno1_err(readn(fd_sk, (void*) read_buf, 3), "myAPI: closeFile: readn");
    if(strcmp(read_buf, "OK")!=0){
        free(write_buf);
        free(read_buf);
        perror("myAPI: closeFile: operazione non andata a buon fine");
        errno=EBADMSG;
        return -1;
        }

    l=strlen(pathname);
    strncpy(write_buf, pathname, l+1);
    
    ec_meno1_err(writen(fd_sk, &l, sizeof(int)), "myAPI: closeFile: writen");
    ec_meno1_err(writen(fd_sk, (void*) write_buf, strlen(pathname)+1), "myAPI: closeFile: writen");
    
    ec_meno1_err(readn(fd_sk, (void*) read_buf, 3), "myAPI: closeFile: readn");
    if(strcmp(read_buf, "OK")!=0){
        free(write_buf);
        free(read_buf);
        perror("myAPI: closeFile: operazione non andata a buon fine");
        errno=EBADMSG;
        return -1;
        }

    free(write_buf);
    free(read_buf);
    */
    if(check_o(&q, pathname)==-1){
        perror("closeFile: File non aperto");
        errno=EBADF;
        return -1;
    }
    pop(&q, pathname);
    printf("closefile ok\n");
    return 0;
}

int removeFile(const char* pathname){
    perror("removeFile:funzionalità momentaneamente non disponibile");
    errno=EPERM;
    return -1;
}




//Funzioni Ausiliarie
int timeout(const struct timespec abstime){
    int timeout=0;
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    //printf("ab-tvsec=%ld, now-tvsec=%ld\n", abstime.tv_sec, now.tv_sec);
    if(abstime.tv_sec>=now.tv_sec){
        //printf("ab-tvnsec=%ld, now-tvnsec=%ld\n", abstime.tv_nsec, now.tv_nsec);
        if(abstime.tv_nsec>=now.tv_nsec){
            //printf("not timeout\n");
            return timeout;}
    }
    //printf("timeout\n");
    return !timeout;
}

ssize_t readn(int fd, void *ptr, size_t n) {  
   size_t   nleft;
   ssize_t  nread;
   nleft = n;
   while (nleft > 0) {
     if((nread = read(fd, ptr, nleft)) < 0) {
        if (nleft == n) return -1; /* error, return -1 */
        else break; /* error, return amount read so far */
     } else if (nread == 0) break; /* EOF */
     nleft -= nread;
     ptr   += nread;
   }
   return(n - nleft); /* return >= 0 */
}
 
ssize_t writen(int fd, void *ptr, size_t n) {  
   size_t   nleft;
   ssize_t  nwritten;
   nleft = n;
   while (nleft > 0) {
     if((nwritten = write(fd, ptr, nleft)) < 0) {
        if (nleft == n) return -1; /* error, return -1 */
        else break; /* error, return amount written so far */
     } else if (nwritten == 0) break; 
     nleft -= nwritten;
     ptr   += nwritten;
   }
   return(n - nleft); /* return >= 0 */
}

void push(c_f **q, const char* pathname){
    if(empty==1){ //sono il primo

        c_f* nuovo;
        //ec_null(*q=malloc(sizeof(c_f)), "S-Master: errore malloc: nuovo");
        *q=malloc(sizeof(c_f));
        //ec_null((*q)->nome=malloc((strlen(pathname)+1)*sizeof(char)), "S-Master: errore malloc: nuovo");
        (*q)->nome=malloc((strlen(pathname)+1)*sizeof(char));
        strncpy((*q)->nome, pathname, strlen(pathname)+1);
        (*q)->op=1;
        (*q)->next=NULL;
        empty=0;
    }
    else{
        c_f* nuovo;
        c_f* curr;
        //ec_null(nuovo=malloc(sizeof(c_f)), "S-Master: errore malloc: nuovo");
        nuovo=malloc(sizeof(c_f));
        //ec_null(nuovo->nome=malloc((strlen(pathname)+1)*sizeof(char)), "S-Master: errore malloc: nuovo");
        nuovo->nome=malloc((strlen(pathname)+1)*sizeof(char));
        strncpy(nuovo->nome, pathname, strlen(pathname)+1);
        nuovo->next=NULL;
        nuovo->op=1;
        curr=*q;
        while(curr->next!=NULL)
            curr=curr->next;
        curr->next=nuovo;
    }
}

//preleva in testa, restituisce 0 se la coda è vuota, -1 se non l'ha trovato, 1 se tutto è andato bene
int pop(c_f **q, const char* pathname){
    c_f* curr;
    c_f* prec;
    prec=NULL;
    curr=*q;
    if(curr==NULL) return 0;
    while((curr!=NULL) && (strcmp(curr->nome, pathname)!=0)){
        prec=curr;
        curr=curr->next;
    }
    if(curr==NULL) return -1;
    //curr non è nè primo nè ultimo
    if((prec!=NULL) && (curr->next!=NULL)) {
        prec->next=curr->next;}
    //curr non è primo ma è ultimo
    else if((prec!=NULL) && (curr->next=NULL)){
        prec->next=NULL;
    }
    //curr è primo ma non ultimo
    else if((prec==NULL) && (curr->next!=NULL)){
        *q=curr->next;
    }
    //curr è primo e ultimo
    else{
        *q=NULL;
        empty=1;
    }
    free(curr->nome);
    free(curr);
    return 1;

}

//-1 se non lo ha trovato, 0 altrimenti
int reset_o(c_f **q, const char* pathname){
    c_f* curr;
    curr=*q;
    if(curr==NULL) return -1;
    while((curr!=NULL) && (strcmp(curr->nome, pathname)!=0)){
        curr=curr->next;
    }
    if(curr==NULL) return -1;
    curr->op=0;
    return curr->op;

}

//-1 se non lo ha trovato, il valore di op altrimenti
int check_o(c_f **q, const char* pathname){
    c_f* curr;
    curr=*q;
    if(curr==NULL) return -1;
    while((curr!=NULL) && (strcmp(curr->nome, pathname)!=0)){
        curr=curr->next;
    }
    if(curr==NULL) return -1;
    return curr->op;
}

int clean_q(c_f **q){
    c_f* curr;
    c_f *next;
    curr=*q;
    
    while(curr!=NULL){
        next=curr->next;
        free(curr->nome);
        free(curr);
        curr=next;
    }

    return 1;

}

//0 se è il path è assoluto, -1 altrimenti
int check_p(const char* pathname){
    if(pathname[0]=='/') return 0;
    return -1;
}

int check_d(const char* dirname){
    //if(dirname[0]!='/') return -1;
    int l;
    l=strlen(dirname);
    if(dirname[l]=='/') return -1;

    return 0;
}

int saveFile(char* no, const char* buf, const char* dirname, const char* op){
    printf("\n\nentro in savefile: ");
    FILE *newfile;
    char *cwd;
    struct stat info;
    char *nome;
    if(stat(dirname, &info)==-1){
        perror("saveFile: dirname non esiste");
        errno=ENOENT;
        return -1;
    }
    if(!S_ISDIR(info.st_mode)){
        perror("saveFile: non è una directory");
        errno=ENOTDIR;
        return -1;
    }
    if(no[0]=='/'){
        int l_n=strlen(no);
        if((nome=malloc(l_n*sizeof(char)))==NULL){
            perror("saveFile: malloc n");
            errno=ENOMEM;
            return -1;
        }
        for(int i=0; i<l_n-1; i++){
            nome[i]=no[i+1];
            
    }
        nome[l_n-1]='\0';
    }
    else{
        int l_n=strlen(no)+1;
        if((nome=malloc(l_n*sizeof(char)))==NULL){
            perror("saveFile: malloc n");
            errno=ENOMEM;
            return -1;
        }
        strncpy(nome, no, l_n);
    }
    if((cwd=malloc(20*sizeof(char)))==NULL){
        perror("saveFile: malloc: cwd");
        errno=ENOMEM;
        free(nome);
        return -1;
    }

    if((cwd=getcwd(cwd, 20))==NULL){
        if(errno!=ERANGE){
            perror("saveFile: getcwd");
            free(cwd);
            free(nome);
            errno=EINTR;
            return -1;
        }
        int l=40;
        free(cwd);
        if((cwd=malloc(l*sizeof(char)))==NULL){
                perror("saveFile: realloc cwd");
                errno=ENOMEM;
                free(cwd);
                free(nome);
                return -1;
            }
        while((cwd=getcwd(cwd, l))==NULL){
            if(errno!=ERANGE){
                perror("saveFile: getcwd");
                free(cwd);
                free(nome);
                errno=EINTR;
                return -1;
            }
            l=l*2;
            free(cwd);
            if((cwd=malloc(l*sizeof(char)))==NULL){
                perror("saveFile: realloc cwd");
                free(cwd);
                free(nome);
                errno=ENOMEM;
                return -1;
            }
        }
    }
    
    if(chdir(dirname)==-1){
        perror("saveFile: chdir(dirname)");
        free(cwd);
        free(nome);
        errno=EINTR;
        return -1;
    }
    ec_null_err(newfile=fopen(nome,"a"), "saveFile: fopen");
    
    if(buf!=NULL) fwrite(buf, sizeof(char), strlen(buf)+1, newfile);
    if(buf!=NULL) fwrite("\n", sizeof(char), 1, newfile);
    ec_div_zero(fclose(newfile), "saveFile: fclose");

    if(chdir(cwd)==-1){
        perror("saveFile: chdir(cwd)");
        free(cwd);
        free(nome);
        errno=EINTR;
        return -1;
    }
    //printf(" qui ci arrivo: ");
    free(cwd);
    free(nome);
    //printf("cwd=%s\n", cwd);
    //printf("e pure qui\n");
}
