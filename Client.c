//#include "./lib/myAPI.h"
#include "./lib/parser.h"


int main(int argc, char* argv[]){

    if(argc==1){
        perror("Client: manca opzione");
        exit(EXIT_FAILURE);
    }

    char* sockname;
    char* pathname;
    char* dirname;
    char* comando;
    char* comando_tok;
    char* com_pointer;
    void* buf;
    char b[6];
    int l_com=0;
    int opt;
    size_t siz;
    

    ec_null(sockname=malloc(10*sizeof(char)), "Client: malloc: sockname\n");
    ec_null(pathname=malloc(10*sizeof(char)), "Client: malloc: pathname\n");
    ec_null(dirname=malloc(10*sizeof(char)), "Client: malloc: dirname\n");
    ec_null(comando=malloc(100*sizeof(char)), "Client: malloc: comando\n");
    ec_null(comando_tok=malloc(100*sizeof(char)), "Client: malloc: comando_tok\n");


    sprintf(sockname, "./mysock");
    sprintf(pathname, "/path");
    sprintf(dirname, "/dir");

    while((opt=getopt(argc, argv, "hf:w:W:D:r:R:d:t:l:u:c:a:o:p"))!=-1){
        //printf("%s\n", comando);
        switch(opt){
            //fprintf(f, "%s\n\n", com_pointer);
                case 'h':{
                    option_h();
                    free(sockname);
                    free(pathname);
                    free(dirname);
                    free(comando);
                    free(comando_tok);
                    exit(EXIT_SUCCESS);
                } break;
                case 'f':{
                    option_f(optarg);
                } break;
                case 'w':{
                    option_w(optarg);
                } break;
                case 'W':{
                    option_W(optarg);
                } break;
                case 'D':{
                    option_D(optarg);
                } break;
                case 'r':{
                    option_r(optarg);
                } break;
                case 'R':{
                    option_R(optarg);
                } break;
                case 'd':{
                    option_d(optarg);
                } break;
                case 't':{
                    option_t(optarg);
                } break;
                case 'l':{
                    perror("-l: operazione non supportata");
                } break;
                case 'u':{
                    perror("-u: operazione non supportata");
                } break;
                case 'c':{
                    perror("-c: operazione non supportata");
                } break;
                case 'a':{
                    option_a(optarg);
                }break;
                case 'o':{
                    option_o(optarg);
                }break;
                case 'p':{
                    option_p();
                } break;
                case '?':{
                    printf("%s: comando non riconosciuto\n", argv[optind-1]);
                }
                default:
                    printf("Operazione non valida\n");
                    fflush(stdout);
                break;
        }
    }

    
    run_client();
    //printf("ran_sec=%d, ran_nsec=%d\n", ran_sec, ran_nsec);
    //printf("ab-tvsec=%ld, prev-tvsec=%ld\n", abstime.tv_sec, prevtime.tv_sec);
    //printf("ab-tvnsec=%ld, prev-tvnsec=%ld\n", abstime.tv_nsec, prevtime.tv_nsec);
    
    /*for(int j=0; j<20; j++){
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



    /*writeFile(pathname, dirname);
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
    }*/
    free(sockname);
    free(pathname);
    free(dirname);
    //free(buf);
    free(comando);
    free(comando_tok);
    return 0;
}
