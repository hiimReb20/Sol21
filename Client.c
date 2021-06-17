#include "./lib/myAPI.h"
#define DELIM " -"
#define BELLO "./bello"

int main(int argc, char* argv[]){
    char* sockname;
    char* pathname;
    char* dirname;
    char* comando;
    char* comando_tok;
    char* com_pointer;
    void* buf;
    char b[6];
    int l_com=0;
    size_t siz;
    struct timespec abstime;
    struct timespec prevtime;
    unsigned int seme=1;
    int ran_sec, ran_nsec, msec, flags;
    

    ec_null(sockname=malloc(10*sizeof(char)), "Client: malloc: sockname\n");
    ec_null(pathname=malloc(10*sizeof(char)), "Client: malloc: pathname\n");
    ec_null(dirname=malloc(10*sizeof(char)), "Client: malloc: dirname\n");
    ec_null(comando=malloc(100*sizeof(char)), "Client: malloc: comando\n");
    ec_null(comando_tok=malloc(100*sizeof(char)), "Client: malloc: comando_tok\n");


    sprintf(sockname, "./mysock");
    sprintf(pathname, "/path");
    sprintf(dirname, "/dir");
    msec=1;
    flags=1;
    /*scanf("%s", comando);
    while(strncmp(comando, "-C", strlen(comando))!=0){
        //printf("%s\n", comando);
        fflush(stdout);
        com_pointer=strtok(comando, DELIM);
        while(com_pointer){
            //fprintf(f, "%s\n\n", com_pointer);
            switch(com_pointer[0]){
                case 'h':{
                    //printf("%s\n", com_pointer[0]);
                    printf("operazione: h\n");
                    fflush(stdout);
                } break;
                case 'f':{
                    //printf("%s\n", com_pointer[0]);
                    printf("operazione: f\n");
                    fflush(stdout);
                } break;
                case 'w':{
                    //printf("%s\n", com_pointer[0]);
                    printf("operazione: w\n");
                    fflush(stdout);
                } break;
                case 'W':{
                    //printf("%s\n", com_pointer[0]);
                    printf("operazione: W\n");
                    fflush(stdout);
                } break;
                case 'D':{
                    //printf("%s\n", com_pointer[0]);
                    printf("operazione: D\n");
                    fflush(stdout);
                } break;
                case 'r':{
                    //printf("%s\n", com_pointer[0]);
                    printf("operazione: r\n");
                    fflush(stdout);
                } break;
                case 'R':{
                    //printf("%s\n", com_pointer[0]);
                    printf("operazione: R\n");
                    fflush(stdout);
                } break;
                case 'd':{
                    //printf("%s\n", com_pointer[0]);
                    printf("operazione: d\n");
                    fflush(stdout);
                } break;
                case 't':{
                    //printf("%s\n", com_pointer[0]);
                    printf("operazione: t\n");
                    fflush(stdout);
                } break;
                case 'l':{
                    //printf("%s\n", com_pointer[0]);
                    printf("operazione: l\n");
                    fflush(stdout);
                } break;
                case 'u':{
                    //printf("%s\n", com_pointer[0]);
                    printf("operazione: u\n");
                    fflush(stdout);
                } break;
                case 'c':{
                    //printf("%s\n", com_pointer[0]);
                    printf("operazione: c\n");
                    fflush(stdout);
                } break;
                case 'p':{
                    //printf("%s\n", com_pointer[0]);
                    printf("operazione: p\n");
                    fflush(stdout);
                } break;
                default:
                    printf("Operazione non valida\n");
                    fflush(stdout);
                break;
            }
           com_pointer=strtok(NULL, DELIM); 
        }
        scanf("%s", comando);
    }*/

    
    
    ran_sec=rand_r(&seme);
    ++seme;
    ran_nsec=rand_r(&seme);
    ran_sec=(ran_sec%60)+60;
    ran_nsec=(ran_nsec%10000);
    clock_gettime(CLOCK_REALTIME, &prevtime);
    abstime.tv_sec=prevtime.tv_sec+ran_sec;
    abstime.tv_nsec=prevtime.tv_nsec+ran_nsec;
    //printf("ran_sec=%d, ran_nsec=%d\n", ran_sec, ran_nsec);
    //printf("ab-tvsec=%ld, prev-tvsec=%ld\n", abstime.tv_sec, prevtime.tv_sec);
    //printf("ab-tvnsec=%ld, prev-tvnsec=%ld\n", abstime.tv_nsec, prevtime.tv_nsec);
    
    //for(int j=0; j<20; j++){
    openConnection(sockname, msec, abstime);
    
    openFile("/prova1.txt", O_CREATE);
    openFile("/bellissimo.txt", O_CREATE);
    openFile("/prova2.txt", O_CREATE);
    openFile("/prova2.txt", O_CREATE);
    openFile("/prova3.txt", O_LOCK);
    openFile("/prova3.txt", O_CREATE);
    openFile("/prova4.txt", O_CREATE);
    openFile("/prova5.txt", O_CREATE);
    openFile("/prova6.txt", O_CREATE);
    openFile("/prova7.txt", O_CREATE);
    openFile("/prova8.txt", O_CREATE);
    openFile("/prova9.txt", O_CREATE);
    openFile("/prova10.txt", O_CREATE);
    openFile("/prova11.txt", O_CREATE);
    openFile("bellissimo", O_CREATE);
    openFile("/home/reb/Scrivania/Sol21/prova15.txt", O_CREATE);
    
    /*writeFile(pathname, dirname);
    writeFile("/prova1.txt", "/home/reb/back_sol/dir");
    writeFile("/bellissimo.txt", "/home/reb/back_sol/dir");
    writeFile("/prova2.txt", "/home/reb/back_sol/dir");
    writeFile("/prova2.txt", "/home/reb/back_sol/dir");
    writeFile("/prova3.txt", "/home/reb/back_sol/dir");
    writeFile("/prova3.txt", "/home/reb/back_sol/dir");
    writeFile("/prova4.txt", NULL);
    writeFile("/prova5.txt", "bellaaaa");
    writeFile("/prova6.txt", "/home/reb/back_sol/dir");
    writeFile("/prova7.txt", "/home/reb/back_sol/dir");
    writeFile("/prova8.txt", "/home/reb/back_sol/dir");
    writeFile("/prova9.txt", "/home/reb/back_sol/dir");
    writeFile("/prova10.txt", "/home/reb/back_sol/dir");
    writeFile("/prova11.txt", "/home/reb/back_sol/dir");
    writeFile("bellissimo", NULL);
    writeFile("ciao", "ciao");
    writeFile("/bellissimo", "/home/reb/prsol21");
    
    snprintf(b, 6, "bello");
    siz=strlen(b);

    appendToFile("/prova1.txt", b, siz, dirname);
    appendToFile("bello", "ciao", 3, dirname);
    appendToFile("/prova2.txt", "la rebi", 7, "/home/reb/back_sol/dir");
    appendToFile("/prova3.txt", "è molto", 8, "/home/reb/back_sol/dir");
    appendToFile("/prova4.txt", "ma molto", 8, "/home/reb/back_sol/dir");
    appendToFile("/prova5.txt", "davvero", 7, "/home/reb/back_sol/dir");
    appendToFile("/prova6.txt", "molto", 5, "/home/reb/back_sol/dir");
    appendToFile("/prova7.txt", "simpy", 5, "/home/reb/back_sol/dir");
    appendToFile("/prova8.txt", "la rebi", 7, "/home/reb/back_sol/dir");
    for(int i=0; i<3; i++)
       appendToFile("/prova9.txt", "èèèèèèèèèèèèèèè", 30, "/home/reb/back_sol/dir"); 
    
    readFile(pathname, &buf, &siz);
    
    readNFiles(2, "/home/reb/back_sol/dir");*/



    writeFile(pathname, dirname);
    writeFile("/prova1.txt", "./dir");
    writeFile("/bellissimo.txt", "./dir");
    writeFile("/prova2.txt", "./dir");
    writeFile("/prova2.txt", "./dir");
    writeFile("/prova3.txt", "./dir");
    writeFile("/prova3.txt", "./dir");
    writeFile("/prova4.txt", NULL);
    writeFile("/prova5.txt", "bellaaaa");
    writeFile("/prova6.txt", "./dir");
    writeFile("/prova7.txt", "./dir");
    writeFile("/prova8.txt", "./dir");
    writeFile("/prova9.txt", "./dir");
    writeFile("/prova10.txt", "./dir");
    writeFile("/prova11.txt", "./dir");
    writeFile("bellissimo", NULL);
    writeFile("ciao", "ciao");
    writeFile("/home/reb/Scrivania/Sol21/prova15.txt", "./dir");
    
    snprintf(b, 6, "bello");
    siz=strlen(b);

    appendToFile("/prova1.txt", b, siz, dirname);
    appendToFile("bello", "ciao", 3, dirname);
    appendToFile("/prova2.txt", "la rebi", 7, "./dir");
    appendToFile("/prova3.txt", "è molto", 8, "./dir");
    appendToFile("/prova4.txt", "ma molto", 8, "./dir");
    appendToFile("/prova5.txt", "davvero", 7, "./dir");
    appendToFile("/prova6.txt", "molto", 5, "./dir");
    appendToFile("/prova7.txt", "simpy", 5, "./dir");
    appendToFile("/prova8.txt", "la rebi", 7, "./dir");
    for(int i=0; i<3; i++)
       appendToFile("/prova9.txt", "èèèèèèèèèèèèèèè", 30, "./dir"); 
    
    readFile(pathname, &buf, &siz);
    
    readNFiles(2, "./dir");
    
    
    lockFile(pathname);
    unlockFile(pathname);
    closeFile(pathname);
    removeFile(pathname);
    
    closeConnection(sockname);
    //}
    free(sockname);
    free(pathname);
    free(dirname);
    //free(buf);
    free(comando);
    free(comando_tok);
    return 0;
}
