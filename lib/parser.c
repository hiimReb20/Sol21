#include "./parser.h"
static int empty=1;
static char *sock=NULL;
static char *dir_lettura=NULL;
static char *dir_scrittura=NULL;
static int tempo;
static int flag_p=0;
c_r *q_opt;
void option_h(){
    printf("File Storage Server: lista delle opzioni valide e relativi argomenti:\n\n");
    printf("-o: n, filename1[,filename2,...]\n\tn, intero uguale a 1 o 2\n\t\tcon n=1 si intende flag=O_CREATE\n");
    printf("\t\tcon n=2 si intende flag=O_LOCK (non supportato)\n\t\tcon n!=(1 V 2) si intende flag=NOFLAG, cioè come non specificato\n\n");
    printf("\tfilename: lista di file\n\trichiesta di aprire filename con flag n\n\n");
    printf("-f:filename\n\tfilename:nome del socket a cui il client può connettersi\n\n");
    printf("-w:dirname[,n]\n\tdirname:nome di una directory\n\tn: intero>=0, opzionale\n");
    printf("\tinvia al server i file contenuti in dirname\n");
    printf("\tse dirname contiene altre directories, queste vengono visitate ricorsivamente fino ad aver scritto n files\n");
    printf("\tse n=0 o assente, non c'è limite superiore ai files da inviare\n\n");
    printf("-W:file1[,file2,...]\n\tfile:lista di file da scrivere nel server\n\ti file devono essere separati solo e soltanto da ','\n\n");
    printf("-D: dirname\n\tdirname: cartella in cui salvare i file inviati dal server a causa di capacity misses\n\n");
    printf("-r: file1[,file2]\n\tfile: lista di file da leggere\n\ti file devono essere separati solo e soltanto da ','\n\n");
    printf("-R: [n]\n\tn: intero>=0, opzionale\n\tlegge n file qualsiasi che siano memorizzati nel server\n");
    printf("\tse n=0 o n assente, richiede di leggere tutti i file memorizzati nel server\n\n");
    printf("-d: dirname\n\tdirname: cartella\n\tspecifica la cartella in cui memorizzare i file letti\n\n");
    printf("-t: time\n\ttime:intero>=0\n\tspecifica il tempo (in ms) che deve intercorrere tra due richieste successive del client al server\n\tse l'opzione non è speciifcata questo tempo va inteso =0\n\n");
    printf("-a: filename,string\n\tfilename: nome file\n\ts: stringa\n");
    printf("\tscrive in append la stringa s al file filename\n\n");
    printf("-p: non accetta argomenti\n\tabilita le stampe sullo stdout per ogni operazione\n\ti dettagli delle operazioni da stampare comprendono\n");
    printf("\t\ttipo di operazione\n\t\tfile di riferimento\n\t\tesito\n\t\tbytes scritti\n");
    
}

void option_f(char* optarg){
           int l=strlen(optarg)+1;
           sock=malloc(l*sizeof(char));
           if(sock==NULL){
               perror("malloc sock");
               exit(EXIT_FAILURE);
           }
           strncpy(sock, optarg, l);
           printf("sock=%s\n", sock);
}

void option_w(char *optarg){
    printf("opzione w\n");
    char b[256];
    int n;
    char *state;
    char *token, *dir;
    token=strtok_r(optarg, ",", &state);
    dir=token;
    token = strtok_r(NULL, ",", &state);
    printf("n=%s\n", token);
    if(token!=NULL) n=atoi(token);
    else n=0;
    printf("n=%d\n", n);
    push_opt(&q_opt, 1, dir, NULL, n);

    stampa(&q_opt);
}

void option_W(char* optarg){
    printf("opzione W\n");
    char b[256];
    char *state;
    char *token = strtok_r(optarg, ",", &state);
    while(token){
        push_opt(&q_opt, 1, token, NULL, -1);
        token = strtok_r(NULL, ",", &state); 
    }
    stampa(&q_opt);
}

void option_r(char* optarg){
    printf("opzione r\n");
    char b[256];
    char *state;
    char *token = strtok_r(optarg, ",", &state);
    while(token){
        push_opt(&q_opt, 2, token, NULL, -1);
        token = strtok_r(NULL, ",", &state); 
    }
    stampa(&q_opt);
}

void option_R(char* optarg){
    printf("opzione R\n");
    char b[256];
    int n;
    char *state;
    char *token;
    token=strtok_r(optarg, ",", &state);
    if(token!=NULL){
        n=atoi(token);
        if(n<0) n=0;
    }
    else n=0;
    push_opt(&q_opt, 2, NULL, NULL, n);

    stampa(&q_opt);
}

void option_d(char* optarg){
    printf("opzione d\n");
    if(dir_lettura!=NULL) free(dir_lettura);
    int l=strlen(optarg)+1;
    dir_lettura=malloc(l*sizeof(char));
    if(dir_lettura==NULL){
        perror("malloc sock");
        exit(EXIT_FAILURE);
        }
        strncpy(dir_lettura, optarg, l);
        printf("dir_lettura=%s\n", dir_lettura);
}

void option_D(char* optarg){
    printf("opzione D\n");
    if(dir_scrittura!=NULL) free(dir_scrittura);
    int l=strlen(optarg)+1;
    dir_scrittura=malloc(l*sizeof(char));
    if(dir_scrittura==NULL){
        perror("malloc sock");
        exit(EXIT_FAILURE);
        }
        strncpy(dir_scrittura, optarg, l);
        printf("dir_scrittura=%s\n", dir_scrittura);
}

void option_t(char* optarg){
    printf("opzione t\n");
    int n;
    char *state;
    char *token;
    token=strtok_r(optarg, ",", &state);
    if(token!=NULL){
        n=atoi(token);
        if(n>0) tempo=n;
    }
    printf("tempo=%d\n", tempo);
}

void option_a(char *optarg){
    printf("opzione a\n");
    char b[256];
    int n;
    char *state;
    char *token, *s, *file;
    token=strtok_r(optarg, ",", &state);
    file=token;
    token = strtok_r(NULL, ",", &state);
    s=token;
    push_opt(&q_opt, 3, file, s, -1);
    stampa(&q_opt);
}

void option_o(char *optarg){
    printf("opzione o\n");
    char *state;
    char *token, *file;
    int n;
    token=strtok_r(optarg, ",", &state);
    n=atoi(token);
    if((n!=1) && (n!=2)) n=0;
    token = strtok_r(NULL, ",", &state);
    while(token){
        push_opt(&q_opt, 0, token, NULL, n);
        token = strtok_r(NULL, ",", &state);
    }

    stampa(&q_opt);
}

void option_p(){
    printf("opzione p\n");
    flag_p=1;
}

void push_opt(c_r **q, int op, char *rich, char* s, int n){
    if(empty==1){ //sono il primo
        //ec_null(*q=malloc(sizeof(c_f)), "S-Master: errore malloc: nuovo");
        *q=malloc(sizeof(c_r));
        //ec_null((*q)->nome=malloc((strlen(pathname)+1)*sizeof(char)), "S-Master: errore malloc: nuovo");
        (*q)->opt=op;
        (*q)->n=n;
        if(rich==NULL) (*q)->str=NULL;
        else {
            (*q)->str=malloc((strlen(rich)+1)*sizeof(char));
            strncpy((*q)->str, rich, strlen(rich)+1);
            }
        if(s==NULL) (*q)->app=NULL;
        else {
            (*q)->app=malloc((strlen(s)+1)*sizeof(char));
            strncpy((*q)->app, s, strlen(s)+1);
            }
        (*q)->next=NULL;
        empty=0;
    }
    else{
        c_r* nuovo;
        c_r* prec;
        c_r* curr;
        c_r* next;

        //ec_null(nuovo=malloc(sizeof(c_f)), "S-Master: errore malloc: nuovo");
        nuovo=malloc(sizeof(c_r));
        //ec_null(nuovo->nome=malloc((strlen(pathname)+1)*sizeof(char)), "S-Master: errore malloc: nuovo");
        nuovo->opt=op;
        nuovo->n=n;
        if(rich==NULL) nuovo->str=NULL;
        else{
            nuovo->str=malloc((strlen(rich)+1)*sizeof(char));
            strncpy(nuovo->str, rich, strlen(rich)+1);
            }
        if(s==NULL) nuovo->app=NULL;
        else{
            nuovo->app=malloc((strlen(s)+1)*sizeof(char));
            strncpy(nuovo->app, s, strlen(s)+1);
            }
        nuovo->next=NULL;
        curr=*q;
        next=curr->next;
        prec=NULL;
        while((curr!=NULL) && (nuovo->opt>=curr->opt)){
            prec=curr;
            curr=curr->next;
            //next=curr->next;
            }
        if(prec==NULL) *q=nuovo; 
        else prec->next=nuovo;
        nuovo->next=curr;
    }
}

//preleva in testa, restituisce 0 se la coda è vuota, -1 se non l'ha trovato, 1 se tutto è andato bene
c_r *pop_opt(c_r **q){
    c_r* curr;
    curr=*q;
    if(curr==NULL) return NULL;
    if(curr->next==NULL)
        *q=NULL;
    else *q=curr->next;
    return curr; 

}

void clean_coda_opt(c_r **q){
    c_r* curr;
    c_r* next;
    curr=*q;
    while(curr!=NULL){
        next=curr->next;
        free(curr->str);
        free(curr);
        curr=next;
    }
    *q=NULL;
    
}

void stampa(c_r**q){
    if(*q==NULL){
        printf("coda vuota\n");
        return;
    }
    c_r* curr;
    curr=*q;
    printf("stampa:\n");
    while(curr!=NULL){
        printf("opt=%d, str=%s, %s, s=n=%d\n", curr->opt, curr->str, curr->app, curr->n);
        curr=curr->next;
    }
    printf("\n\n");
}



void run_client(){
    c_r *richiesta;
    struct timespec abstime;
    struct timespec prevtime;
    unsigned int seme=1;
    int ran_sec, ran_nsec, msec, flags;
    ran_sec=rand_r(&seme);
    ++seme;
    ran_nsec=rand_r(&seme);
    ran_sec=(ran_sec%60)+60;
    ran_nsec=(ran_nsec%10000);
    clock_gettime(CLOCK_REALTIME, &prevtime);
    abstime.tv_sec=prevtime.tv_sec+ran_sec;
    abstime.tv_nsec=prevtime.tv_nsec+ran_nsec;
    openConnection(sock, tempo, abstime);

    //push_opt(&q, 3, NULL, -8);
    richiesta=pop_opt(&q_opt);
    while(richiesta!=NULL){
        switch (richiesta->opt)
        {
        case 0:{
            if(richiesta->n==1)openFile(richiesta->str, O_CREATE);
            else if(richiesta->n==2)openFile(richiesta->str, O_LOCK);
            else if(richiesta->n==0)openFile(richiesta->str, NOFLAG);
            
        }
            break;
        case 1:{
            if(richiesta->n<0){
                printf("w\n");
                //openFile(richiesta->str, O_CREATE);
            }
            else{
                printf("W\n");
                writeFile(richiesta->str, dir_scrittura);
            }
        }
            break;
        case 2:
            if(richiesta->n<0){
                char *b;
                size_t s;
                readFile(richiesta->str, (void**)&b, &s);
            }
            else{
                readNFiles(richiesta->n, dir_lettura);
            }
            break;
        case 3:
            
                appendToFile(richiesta->str, richiesta->app, strlen(richiesta->app), dir_scrittura);
            break;
        default:{
            perror("run_client: opzione non valida");
            }   
            break;
        }
        if(richiesta->str!=NULL) free(richiesta->str);
        if(richiesta->app!=NULL) free(richiesta->app);
        free(richiesta);
        richiesta=pop_opt(&q_opt);
    }
    closeConnection(sock);
}
