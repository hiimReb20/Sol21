
#include "./lib/parser.h"


int main(int argc, char* argv[]){

    if(argc==1){
        perror("Client: manca opzione");
        exit(EXIT_FAILURE);
    }

    int opt;
    while((opt=getopt(argc, argv, "hf:w:W:D:r:R:d:t:l:u:c:a:o:p"))!=-1){
        switch(opt){
                case 'h':{
                    option_h();
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
                    perror("Operazione non valida");
                    fflush(stdout);
                break;
        }
    }

    
    run_client();

    return 0;
}
