#include "./mylib.h"

static pthread_mutex_t mtx_h=PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond_h=PTHREAD_COND_INITIALIZER;

conf* myconf (char* conffile){
    int i=0;
    char* bu;
    char* b;
    char* nomesock;
    char* nomelog;
    FILE* f=NULL;
    conf* c;
    
    ec_null(c=malloc(sizeof(conf)), "Errore malloc in myconf: c");

    ec_null(b=malloc(256*sizeof(char)), "Errore malloc in myconf: b");

    ec_null(nomelog=malloc(256*sizeof(char)), "Errore malloc in myconf: nomelog");

    ec_null(nomesock=malloc(256*sizeof(char)), "Errore malloc in myconf: nomesock");

    ec_null_err(f=fopen(conffile, "r"), "Errore fopen in myconf");
    
    c->NFILE=0;
    c->DIM=0;
    c->NT=0;

    while(i<3){
        ec_null(fgets(b, 256, f), "Errore fgets in myconf");
        bu=b;
        while(*b!='=')
            b++;
        b++;

        switch(i){
            case 0:
                c->NFILE=atoi(b);
                break;
            case 1:
                c->DIM=atoi(b);
                break;
            case 2:
                c->NT=atoi(b);
                break;
            }
        i++;
        b=bu;
    }
    
    ec_null(fgets(nomelog, 256, f), "Errore fgets in myconf");
    if (nomelog[strlen(nomelog)-1] =='\n') nomelog[strlen(nomelog)-1] = '\0';
    else if (nomelog[strlen(nomelog)-1] ==' ') nomelog[strlen(nomelog)-1] = '\0';
    int l=strlen(nomelog);
    ec_null(c->fileLog=malloc((l+1)*sizeof(char)), "Errore malloc in myconf: c->fileLog");
    strncpy(c->fileLog, nomelog, l+1);
    fflush(stdout);
    
    ec_null(fgets(nomesock, 256, f), "Errore fgets in myconf");
    if (nomesock[strlen(nomesock)-1] =='\n') nomesock[strlen(nomesock)-1] = '\0';
    else if (nomesock[strlen(nomesock)-1] ==' ') nomesock[strlen(nomesock)-1] = '\0';
    l=strlen(nomesock);
    ec_null(c->sock=malloc((l+1)*sizeof(char)), "Errore malloc in myconf: c->sock");
    strncpy(c->sock, nomesock, l+1);
    fflush(stdout);

    free(b);
    free(nomelog);
    free(nomesock);


    ec_div_zero(fclose(f), "Errore fclose in myconf");
    
    return c;
}




int max_fd(fd_set set, int fd_max){
    int fd, max=-1;
    for(int i=0, fd=0; i<fd_max+1; i++, fd++){
        if(FD_ISSET(fd, &set))
            if(fd>max) max=fd;
    }
    return max;
}




//inserisce in coda
void push(coda **q, int fd){
    if(empty==1){ //sono il primo

        coda* nuovo;
        ec_null(*q=malloc(sizeof(coda)), "S-Master: errore malloc: nuovo");
        (*q)->r_fd=fd;
        (*q)->next=NULL;
        empty=0;
    }
    else{
        coda* nuovo;
        coda* curr;
        ec_null(nuovo=malloc(sizeof(coda)), "S-Master: errore malloc: nuovo");
        nuovo->r_fd=fd;
        nuovo->next=NULL;
        curr=*q;
        while(curr->next!=NULL)
            curr=curr->next;
        curr->next=nuovo;
    }
}


//inserisce in testa;
void push_in_testa(coda **q, int fd){
    if(empty==1){ //sono il primo

        coda* nuovo;
        ec_null(*q=malloc(sizeof(coda)), "S-Master: errore malloc: nuovo");
        (*q)->r_fd=fd;
        (*q)->next=NULL;
        empty=0;
    }
    else{
        coda* nuovo;
        coda* curr;
        curr=*q;
        ec_null(nuovo=malloc(sizeof(coda)), "S-Master: errore malloc: nuovo");
        nuovo->r_fd=fd;
        nuovo->next=curr;
        *q=nuovo;
    }
}




//preleva in testa
int pop(coda **q){
    int fd;
    coda* curr;
    curr=*q;
    fd=curr->r_fd;
    if((fd==-1) || (fd==-2)){
        return fd;
    }
    curr=*q;
    *q=curr->next;
    if(curr->next==NULL) empty=1;
    curr->next=NULL;
    free(curr);
    return fd;

}


void clean_coda(coda **q){
    coda* curr;
    coda* next;
    curr=*q;
    while(curr!=NULL){
        next=curr->next;
        free(curr);
        curr=next;
    }
    *q=NULL;
    
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




void openF(int fd, int fd_w, int i, myhash *ht){
    int l;
    char *com;
    int presente;
    int msg;
    ec_meno1_err(writen(fd, (void*)"OK", 3), "Worker: writen");
    //ricevo lunghezza stringa nome file
    ec_meno1_err(readn(fd, &l, sizeof(int)), "Worker: readn");
    ec_null(com=malloc((l+1)*sizeof(char)), "Worker: malloc com");
    //ricevo stringa nome file
    ec_meno1_err(readn(fd, (void*)com, l+1), "Worker: readn");
    //controllo, visto che è valido solo il flag O_CREATE, se il file esiste già
    Lock(&mtx_h, "openF: lock");
    if(hash_find(ht, com)!=NULL){
        presente=1;
        }
    else{
        presente=0;
    }
    ec_meno1_err(writen(fd, &presente, sizeof(int)), "Worker: writen");
    ec_meno1_err(readn(fd, &msg, sizeof(int)), "Worker: readn");
    if(msg==1){
        if(hash_insert(ht, com, NULL)==NULL){
            Unlock(&mtx_h, "writeF: unlock");
            perror("Worker: hash_insert");
            ec_meno1_err(writen(fd, (void*)"NO", 3), "Worker: writen");
            ec_meno1_err(writen(fd_w, &fd, sizeof(int)), "Worker: writen");
            return;}
        ec_meno1_err(writen(fd, (void*)"OK", 3), "Worker: writen");
    }
    
    Unlock(&mtx_h, "openF: unlock");
    free(com);
    ec_meno1_err(writen(fd_w, &fd, sizeof(int)), "Worker: writen");
}



void readFi(int fd, int fd_w, int i, myhash *ht){

    int l, g;
    char *com;
    void* data;
    ec_meno1(writen(fd, (void*)"OK", 3), "Worker: writen");
    ec_meno1_err(readn(fd, &l, sizeof(int)), "Worker: readn");
    ec_null(com=malloc((l+1)*sizeof(char)), "Worker: malloc com");
    ec_meno1_err(readn(fd, (void*)com, l+1), "Worker: readn");
    Lock(&mtx_h, "readFi: lock");
    if((data=hash_find(ht, com))==NULL){
        Unlock(&mtx_h, "readFi: unlock");
        free(com);
        ec_meno1_err(writen(fd, (void*)"NO", 3), "Worker: writen");
        ec_meno1_err(writen(fd_w, &fd, sizeof(int)), "Worker: writen");
        return;
    }
    else{
        ec_meno1(writen(fd, (void*)"OK", 3), "Worker: writen");
        char* d=(char*)data;
        //calcolo la lunghezza del cobtenuto che dovrò mandare e gliela mando
        g=strlen(d);
        ec_meno1(writen(fd, &g, sizeof(int)), "Worker: writen");
        ec_meno1(writen(fd, data, g+1), "Worker: writen");
        if(hash_set_open(ht, com)!=0){
                Unlock(&mtx_h, "writeF: unlock");
                perror("Worker: hash_insert");
                ec_meno1_err(writen(fd, (void*)"NO", 3), "Worker: writen");
                ec_meno1_err(writen(fd_w, &fd, sizeof(int)), "Worker: writen");
                return;}
        Unlock(&mtx_h, "openF: unlock");
        ec_meno1_err(writen(fd, (void*)"OK", 3), "Worker: writen");
    }
    
    free(com);       
    ec_meno1_err(writen(fd_w, &fd, sizeof(int)), "Worker: writen");
}

void readNF(int fd, int fd_w, int i, myhash *ht){
    int n; 
    int p;
    int r;
    p=0;
    ec_meno1(writen(fd, (void*)"OK", 3), "Worker: writen");
    ec_meno1_err(readn(fd, &n, sizeof(int)), "Worker: readn");
    
    if(n<=0 || n>ht->nentries) p=ht->nentries;
    else p=n;
    ec_meno1_err(writen(fd, (void*)&p, sizeof(int)), "Worker: writen");
    
    for(int i=0, j=0; j<p, i<ht->nbuckets; i++){
        entry *curr;
        for(curr=ht->buckets[i]; j<p, curr!=NULL;curr=curr->next){
            if(j<p){
                int l_n, l_c;
                l_n=strlen(curr->k);
                ec_meno1(writen(fd, &l_n, sizeof(int)), "Worker: writen");
                l_c=0;
                if(curr->data!=NULL)l_c=strlen(curr->data);
                ec_meno1_err(writen(fd, &l_c, sizeof(int)), "Worker: writen");
                ec_meno1_err(writen(fd, curr->k, l_n+1), "Worker: writen");
                if(curr->data!=NULL) ec_meno1_err(writen(fd, curr->data, l_c+1), "Worker: writen");
                j++;
            }
        }
    }

    ec_meno1_err(writen(fd_w, &fd, sizeof(int)), "Worker: writen");
}



void writeF(int fd, int fd_w, int i, myhash *ht){
    int l, r=0;
    char *com;
    char *cont;
    entry *e;
    ec_meno1_err(writen(fd, (void*)"OK", 3), "Worker: writen");
    
    ec_meno1_err(readn(fd, &l, sizeof(int)), "Worker: readn");
    ec_null(com=malloc((l+1)*sizeof(char)), "Worker: malloc com");
    ec_meno1_err(readn(fd, com, l+1), "Worker: readn");
    ec_meno1_err(readn(fd, &l, sizeof(int)), "Worker: readn");
    printf("com=%s\n", com);
    printf("l=%d\n", l);
    if(l!=0){
        ec_null(cont=malloc((l+1)*sizeof(char)), "Worker: malloc com");
        ec_meno1_err(readn(fd, cont, l+1), "Worker: readn");
        printf("cont=%s\n", cont);
    }
    Lock(&mtx_h, "writeF: lock");
    if(hash_find(ht, com)==NULL){
        Unlock(&mtx_h, "writeF: unlock");
        free(com);
        if(l!=0) free(cont);
        ec_meno1_err(writen(fd, (void*)"NO", 3), "Worker: writen");
        ec_meno1_err(writen(fd_w, &fd, sizeof(int)), "Worker: writen");
        return;
    }
    if(hash_check_w(ht, com)!=1){
        Unlock(&mtx_h, "writeF: unlock");
        free(com);
        if(l!=0) free(cont);
        ec_meno1_err(writen(fd, (void*)"NO", 3), "Worker: writen");
        ec_meno1_err(writen(fd_w, &fd, sizeof(int)), "Worker: writen");
        return;
    }
        if(l==0){
            if(hash_update_insert(ht, com, NULL)==NULL){
                Unlock(&mtx_h, "writeF: unlock");
                perror("Worker: hash_insert");
                free(com);
                if(l!=0) free(cont);
                ec_meno1_err(writen(fd, (void*)"NO", 3), "Worker: writen");
                ec_meno1_err(writen(fd_w, &fd, sizeof(int)), "Worker: writen");
                return;}
        }
        else{
            printf("ramo else\n");
            if(hash_update_insert(ht, com, cont)==NULL){
                free(com);
                if(l!=0) free(cont);
                Unlock(&mtx_h, "writeF: unlock");
                perror("Worker: hash_insert");
                ec_meno1_err(writen(fd, (void*)"NO", 3), "Worker: writen");
                ec_meno1_err(writen(fd_w, &fd, sizeof(int)), "Worker: writen");
                return;}
        }
        if(hash_set_open(ht, com)!=0){
                Unlock(&mtx_h, "writeF: unlock");
                perror("Worker: hash_insert");
                free(com);
                if(l!=0) free(cont);
                ec_meno1_err(writen(fd, (void*)"NO", 3), "Worker: writen");
                ec_meno1_err(writen(fd_w, &fd, sizeof(int)), "Worker: writen");
                return;}
        ec_meno1_err(writen(fd, (void*)"OK", 3), "Worker: writen");
    free(com);
    if(l!=0) free(cont);
    
	if((ht->nfile)>(ht->max_file)) {r=1;}
	if((ht->dim)>(ht->max_dim)) {r=1;}
    
    ec_meno1(writen(fd, &r, sizeof(int)), "Worker: writen");
    while(r==1){
    	int l_n, l_c;
    	char *d;
        l_n=strlen(ht->key_min);
        ec_meno1(writen(fd, &l_n, sizeof(int)), "Worker: writen");
        d=hash_ret_data(ht, ht->key_min);
        if(d==NULL) l_c=0;
        else l_c=strlen(d);
        ec_meno1_err(writen(fd, &l_c, sizeof(int)), "Worker: writen");
        ec_meno1_err(writen(fd, ht->key_min, l_n+1), "Worker: writen");
        if (l_c!=0)ec_meno1_err(writen(fd, d, l_c+1), "Worker: writen");
        hash_delete(ht, ht->key_min, free, free);

        if(ht->nfile>ht->max_file) r=1;
	    else if(ht->dim>ht->max_dim) r=1;
        else r=0;
        ec_meno1(writen(fd, &r, sizeof(int)), "Worker: writen");
        }
            

    Unlock(&mtx_h, "openF: unlock");
    ec_meno1_err(writen(fd_w, &fd, sizeof(int)), "Worker: writen");
}

void append(int fd, int fd_w, int i, myhash *ht){
    size_t s;
    int l;
    char *com;
    char *buf;
    int r=0;
    ec_meno1(writen(fd, (void*)"OK", 3), "Worker: writen");

    ec_meno1_err(readn(fd, &l, sizeof(int)), "Worker: readn");
    ec_null(com=malloc((l+1)*sizeof(char)), "Worker: malloc com");
    ec_meno1_err(readn(fd, com, l+1), "Worker: readn");
    //ec_meno1_err(writen(fd, (void*)"OK", 3), "Worker: writen");
    
    Lock(&mtx_h, "append: lock");
    if(hash_find(ht, com)==NULL){
        Unlock(&mtx_h, "append: unlock");
        free(com);
        perror("append:file inesistente");
        ec_meno1_err(writen(fd, (void*)"NO", 3), "Worker: writen");
        ec_meno1_err(writen(fd_w, &fd, sizeof(int)), "Worker: writen");
        ec_meno1_err(writen(fd_w, &fd, sizeof(int)), "Worker: writen");
        return;
    }
    
        ec_meno1_err(writen(fd, (void*)"OK", 3), "Worker: writen");
        ec_meno1_err(readn(fd, &s, sizeof(size_t)), "Worker: readn");
        ec_null(buf=malloc((s+1)*sizeof(char)), "Worker: malloc com");
        ec_meno1_err(readn(fd, buf, s+1), "Worker: readn");
        if(hash_update_insert(ht, com, buf)==NULL){
            Unlock(&mtx_h, "append: unlock");
            free(com);
            free(buf);
            perror("Worker: hash_update_insert");
            ec_meno1_err(writen(fd, (void*)"NO", 3), "Worker: writen");
            ec_meno1_err(writen(fd_w, &fd, sizeof(int)), "Worker: writen");
            ec_meno1_err(writen(fd_w, &fd, sizeof(int)), "Worker: writen");
            return;}
        if(hash_set_open(ht, com)!=0){
                Unlock(&mtx_h, "writeF: unlock");
                perror("Worker: hash_insert");
                ec_meno1_err(writen(fd, (void*)"NO", 3), "Worker: writen");
                ec_meno1_err(writen(fd_w, &fd, sizeof(int)), "Worker: writen");
                return;}
        free (buf);
        ec_meno1_err(writen(fd, (void*)"OK", 3), "Worker: writen");
     

    if(ht->nfile>ht->max_file) {r=1;}
	if(ht->dim>ht->max_dim) {r=1;}
    ec_meno1(writen(fd, &r, sizeof(int)), "Worker: writen");

    while(r==1){
    	int l_n, l_c;
    	char *d;
        l_n=strlen(ht->key_min);
        ec_meno1(writen(fd, &l_n, sizeof(int)), "Worker: writen");
        d=hash_ret_data(ht, ht->key_min);
        if(d==NULL) l_c=0;
        else l_c=strlen(d);
        ec_meno1_err(writen(fd, &l_c, sizeof(int)), "Worker: writen");
        ec_meno1_err(writen(fd, ht->key_min, l_n+1), "Worker: writen");
        if (l_c!=0)ec_meno1_err(writen(fd, d, l_c+1), "Worker: writen");
        hash_delete(ht, ht->key_min, free, free);
        if(ht->nfile>ht->max_file) r=1;
	    else if(ht->dim>ht->max_dim) r=1;
        else r=0;
        ec_meno1(writen(fd, &r, sizeof(int)), "Worker: writen");
        }
    Unlock(&mtx_h, "append: unlock");
    free(com);
    ec_meno1_err(writen(fd_w, &fd, sizeof(int)), "Worker: writen");
}

/*void closeF(int fd, int fd_w, int i, myhash *ht){
    int l;
    char *com;
    ec_meno1(writen(fd, (void*)"OK", 3), "Worker: writen");

    ec_meno1_err(readn(fd, &l, sizeof(int)), "Worker: readn");
    ec_null(com=malloc((l+1)*sizeof(char)), "Worker: malloc com");
    ec_meno1_err(readn(fd, com, l+1), "Worker: readn");

    ec_meno1(writen(fd, (void*)"OK", 3), "Worker: writen");

    free(com);
    ec_meno1_err(writen(fd_w, &fd, sizeof(int)), "Worker: writen");
    
    printf("Worker-%d: closeF fatta\n", i);
}*/
