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
    printf("-o: n, filename1[,filename2,...]\n\tn, intero uguale a 1 o 2, NON OPZIONALE\n\t\tcon n=1 si intende flag=O_CREATE\n");
    printf("\t\tcon n=2 si intende flag=O_LOCK (non supportato)\n\t\tcon n!=(1 V 2) si intende flag=NOFLAG, cioè come non specificato\n\n");
    printf("\tfilename: lista di file\n\trichiesta di aprire filename con flag n\n\n");
    printf("-f:filename\n\tfilename:nome del socket a cui il client deve connettersi\n\n");
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
}

void option_w(char *optarg){
    char b[256];
    int n;
    char *state;
    char *token, *dir;
    token=strtok_r(optarg, ",", &state);
    dir=token;
    token = strtok_r(NULL, ",", &state);
    if(token!=NULL) {
        n=atoi(token);
        if(n<0) n=0;
        }
    else n=0;
    push_opt(&q_opt, 1, dir, NULL, n);
}

void option_W(char* optarg){
    char b[256];
    char *state;
    char *token = strtok_r(optarg, ",", &state);
    while(token){
        push_opt(&q_opt, 1, token, NULL, -1);
        token = strtok_r(NULL, ",", &state); 
    }
}

void option_r(char* optarg){
    char b[256];
    char *state;
    char *token = strtok_r(optarg, ",", &state);
    while(token){
        push_opt(&q_opt, 2, token, NULL, -1);
        token = strtok_r(NULL, ",", &state); 
    }
}

void option_R(char* optarg){
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
}

void option_d(char* optarg){
    if(dir_lettura!=NULL) free(dir_lettura);
    int l=strlen(optarg)+1;
    dir_lettura=malloc(l*sizeof(char));
    if(dir_lettura==NULL){
        perror("malloc dir_lettura");
        exit(EXIT_FAILURE);
        }
    strncpy(dir_lettura, optarg, l);   
}

void option_D(char* optarg){
    if(dir_scrittura!=NULL) free(dir_scrittura);
    int l=strlen(optarg)+1;
    dir_scrittura=malloc(l*sizeof(char));
    if(dir_scrittura==NULL){
        perror("malloc dir_scrittura");
        exit(EXIT_FAILURE);
        }
    strncpy(dir_scrittura, optarg, l);}

void option_t(char* optarg){
    int n;
    char *state;
    char *token;
    token=strtok_r(optarg, ",", &state);
    if(token!=NULL){
        n=atoi(token);
        if(n>0) tempo=n;
    }
}

void option_a(char *optarg){
    char b[256];
    int n;
    char *state;
    char *token, *s, *file;
    token=strtok_r(optarg, ",", &state);
    file=token;
    token = strtok_r(NULL, ",", &state);
    s=token;
    push_opt(&q_opt, 3, file, s, -1);
}

void option_o(char *optarg){
    char *state;
    char *token, *file;
    int n;
    token=strtok_r(optarg, ",", &state);
    n=atoi(token);
    if((n!=1) && (n!=2) && (n!=0)) n=1;
    token = strtok_r(NULL, ",", &state);
    while(token){
        push_opt(&q_opt, 0, token, NULL, n);
        token = strtok_r(NULL, ",", &state);
    }
}

void option_p(){
    flag_p=1;
}

void push_opt(c_r **q, int op, char *rich, char* s, int n){
    if(empty==1){ //sono il primo
        ec_null_v(*q=malloc(sizeof(c_r)), "parser.h: errore malloc: nuovo");
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

        ec_null_v(nuovo=malloc(sizeof(c_r)), "parser.h: errore malloc: nuovo");
        nuovo->opt=op;
        nuovo->n=n;
        if(rich==NULL) nuovo->str=NULL;
        else{
            ec_null_v(nuovo->str=malloc((strlen(rich)+1)*sizeof(char)), "parser.h: errore malloc: str");
            strncpy(nuovo->str, rich, strlen(rich)+1);
            }
        if(s==NULL) nuovo->app=NULL;
        else{
            ec_null_v(nuovo->app=malloc((strlen(s)+1)*sizeof(char)), "parser.h: malloc: app");
            strncpy(nuovo->app, s, strlen(s)+1);
            }
        nuovo->next=NULL;
        curr=*q;
        prec=NULL;
        while((curr!=NULL) && (nuovo->opt>=curr->opt)){
            prec=curr;
            curr=curr->next;
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


int findFile(char *dirname, int n, int tro){
    if(n==-1) return -1;
    if(n==tro) return 0;
    struct stat info;
    if(stat(dirname, &info) ==-1){
        perror("findFile: dirname non esiste");
        return -1;
    }
    if(!S_ISDIR(info.st_mode)){
        perror("findFile: dirname non è una directory");
        return -1;
    }
    DIR* thisdir;
    if((thisdir=opendir(dirname))==NULL){
        perror("findFile: opendir");
        return -1;
    }
    int t=0;
    int stop=0;
    struct dirent* thisread;
    errno=0;
    while(((errno=0, thisread=readdir(thisdir))!=NULL) &&(stop==0)){
        if(thisread->d_ino!=0){
            if((thisread->d_type==4) &&(strcmp(thisread->d_name, ".")!=0) && (strcmp(thisread->d_name, "..")!=0)){
                char *newdir;
                if((newdir=malloc((strlen(dirname)+strlen(thisread->d_name)+2)*sizeof(char)))==NULL){
                    perror("findFile: malloc newdir");
                    return -1;
                }
                strncpy(newdir, dirname, strlen(dirname)+1);
                strncat(newdir, "/", 1);
                strncat(newdir, thisread->d_name, strlen(thisread->d_name)+1);
                int ret=0;
                if((tro+t)!=n)
                    ret=findFile(newdir, n, t+tro);
                t+=ret;
                if((t+tro)==n) stop=1;
                free(newdir);
            }
            else if(thisread->d_type==8){
                int l=strlen(thisread->d_name);
                if((((thisread->d_name[l-1])=='t')&&(thisread->d_name[l-2]=='x')&&(thisread->d_name[l-3]=='t')&&(thisread->d_name[l-4]=='.'))){
                    char *newnome;
                    if((newnome=malloc((strlen(dirname)+strlen(thisread->d_name)+2)*sizeof(char)))==NULL){
                    perror("findFile: malloc newdir");
                    return -1;
                    }
                    strncpy(newnome, dirname, strlen(dirname)+1);
                    strncat(newnome, "/", 1);
                    strncat(newnome, thisread->d_name, strlen(thisread->d_name)+1);
                    openFile(newnome, O_CREATE);
                    writeFile(newnome, dir_scrittura);
                    free(newnome);
                    t++;
                    if((tro+t)==n) stop=1;
                }
            }
        }
    }
    if(errno!=0){
        perror("findFile: readdir");
        return -1;
    }
    
    if(closedir(thisdir)==-1){
        perror("findFile: closedir");
        return -1;
    }
    return t;
}

void run_client(){
    c_r *richiesta;
    struct timespec abstime;
    struct timespec prevtime;
    struct timespec intervallo;
    
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
    intervallo.tv_sec=tempo/1000;
    intervallo.tv_nsec=(tempo%1000)*1.0e6;
    
    int op=openConnection(sock, tempo, abstime);
    if(flag_p && (op!=-1)) printf("-f: Connessione con socket %s avvenuta con successo\n", sock);
    else if(flag_p && (op==-1)) printf("-f: Connessione con socket %s fallita\n", sock);

    //push_opt(&q, 3, NULL, -8);
    richiesta=pop_opt(&q_opt);
    while(richiesta!=NULL){
        switch (richiesta->opt)
        {
        case 0:{
            int o;
            if(richiesta->n==1) o=openFile(richiesta->str, O_CREATE);
            else if(richiesta->n==2)o=openFile(richiesta->str, O_LOCK);
            else if(richiesta->n==0)o=openFile(richiesta->str, NOFLAG);
            if(flag_p && (o!=-1)) printf("-o: Apertura di %s avvenuta con successo\n", richiesta->str);
            else if(flag_p && (o==-1)) printf("-o: Apertura di %s fallita\n", richiesta->str);
            
        }
            break;
        case 1:{
            if(richiesta->n>=0){
                int f=findFile(richiesta->str, richiesta->n, 0);
                if(flag_p && (f!=-1)) printf("-w: Scrittura di %d file presenti in %s avvenuta con successo\n", f, richiesta->str);
                else if(flag_p && (f==-1)) printf("-w: Scrittura dei file presenti in %s fallita\n", richiesta->str);
            }
            else{
                int w=writeFile(richiesta->str, dir_scrittura);
                if(flag_p && (w!=-1)) printf("-W: Scrittura di %s avvenuta con successo\n",richiesta->str);
                else if(flag_p && (w==-1)) printf("-W: Scrittura di %s fallita\n", richiesta->str);
            }
        }
            break;
        case 2:
            if(richiesta->n<0){
                char *b;
                b=NULL;
                size_t s;
                int r=readFile(richiesta->str, (void**)&b, &s);
                if(flag_p && (r==-1)) printf("-r: lettura di %s fallita\n", richiesta->str);
                if(b!=NULL && (r!=-1)){
                    int s=saveFile(richiesta->str, b, dir_lettura, "readF");
                    if(flag_p && (s==-1)) printf("-r: lettura di %s avvenuta con successo. Salvataggio del file con contenuto '%s' in %s fallito\n", richiesta->str, b, dir_lettura);
                    else if(flag_p && (s!=-1)) printf("-r: lettura di %s e salvataggio del file con contenuto '%s' in %s avvenuti con successo\n", richiesta->str, b, dir_lettura);
                }   
                else if(b==NULL && (r!=-1)){
                    int s=saveFile(richiesta->str, NULL, dir_lettura, "readF");
                    if(flag_p && (s==-1)) printf("-r: lettura di %s avvenuta con successo. Salvataggio del file vuoto in %s fallito\n", richiesta->str, dir_lettura);
                    else if(flag_p && (s!=-1)) printf("-r: lettura di %s e salvataggio del file vuoto in %s avvenuti con successo\n", richiesta->str, dir_lettura);
                }
            }
            else{
                int R=readNFiles(richiesta->n, dir_lettura);
                if(flag_p && (R!=-1)) printf("-R: Memorizzazione dei file del server in %s avvenuta con successo\n", dir_lettura);
                else if(flag_p && (R==-1)) printf("-R: Memorizzazione dei file del server in %s fallita\n", dir_lettura);
            }
            break;
        case 3:{
                int a=appendToFile(richiesta->str, richiesta->app, strlen(richiesta->app), dir_scrittura);
                if(flag_p && (a!=-1)) printf("-a: Append della stringa %s al file %s avvenuta con successo\n", richiesta->app, richiesta->str);
                else if(flag_p && (a==-1)) printf("-a: Append della stringa %s al file %s  fallita\n", richiesta->app, richiesta->str);
            }
           break;
        default:{
            perror("run_client: opzione non valida");
            }   
            break;
        }
        if(richiesta->str!=NULL) free(richiesta->str);
        if(richiesta->app!=NULL) free(richiesta->app);
        free(richiesta);
        nanosleep(&intervallo, NULL);
        richiesta=pop_opt(&q_opt);
    }
    closeConnection(sock);
}
