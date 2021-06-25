CC = gcc
CFLAGS = -std=c99 -lm -lpthread -g
VALGRIND = valgrind --leak-check=full
CON1 = ./configurazioni/config1.txt
CON2 = ./configurazioni/config2.txt
NOMES = ./Server.c
NOMEC = ./Client.c

.PHONY: all server.o client.o ./lib/myAPI.o ./lib/mylib.o ./lib/icl_hash.o ./lib/parser.o test1 test2 clean1 clean2

all: server.o client.o
	
#compilazione Server	
server.o: ./lib/mylib.o ./lib/icl_hash.o
	@$(CC) $(NOMES) $(CFLAGS) -o server.o -L. -lmyl -licl

client.o: ./lib/myAPI.o ./lib/parser.o
	@$(CC) $(NOMEC) $(CFLAGS) -o client.o -L. -lp -lmyA

#compilazione e linking libreria	
./lib/mylib.o: ./lib/mylib.c 
	@$(CC) $(CFLAGS) $< -c -o ./lib/mylib.o
	@ar rvs libmyl.a ./lib/mylib.o

./lib/myAPI.o: ./lib/myAPI.c 
	@$(CC) $(CFLAGS) $< -c -o ./lib/myAPI.o
	@ar rvs libmyA.a ./lib/myAPI.o

./lib/icl_hash.o: ./lib/icl_hash.c
	@$(CC) $(CFLAGS) $< -c -o ./lib/icl_hash.o
	@ar rvs libicl.a ./lib/icl_hash.o

./lib/parser.o: ./lib/parser.c
	@$(CC) $(CFLAGS) $< -c -o ./lib/parser.o
	@ar rvs libp.a ./lib/parser.o


test1:
	@$(VALGRIND) ./server.o $(CON1) & echo $$! > t1.PID &\
	sleep 1;
	@bash ./script1.sh;
	@kill -1 $$(cat t1.PID); \


test2:
	@(./server.o $(CON2) & echo $$! > t2.PID) &\
	sleep 1;
	@bash ./script2.sh;
	@kill -1 $$(cat t2.PID); \

clean1:
	@rm -f *.o libmyl.a libmyA.a libicl.a libp.a ./lib/mylib.o ./lib/myAPI.o ./lib/icl_hash.o ./lib/parser.o mysock* *.PID  logFile1.log
	@rm -f ./dir_l/*.txt ./dir_s/*.txt ./file/*.txt ./file_o/*.txt 
	@rm -d  ./dir_l ./dir_s ./file ./file_o   

clean2:
	@rm -f *.o libmyl.a libmyA.a libicl.a libp.a ./lib/mylib.o ./lib/myAPI.o ./lib/icl_hash.o ./lib/parser.o mysock* *.PID  logFile2.log
	@rm -f ./dir_l1/*.txt ./dir_s1/*.txt ./file1/*.txt ./file_o1/*.txt ./dir_l2/*.txt ./dir_s2/*.txt ./file2/*.txt ./file_o2/*.txt
	@rm -f ./dir_l3/*.txt ./dir_s3/*.txt ./file3/*.txt ./file_o3/*.txt
	@rm -d ./dir_l1 ./dir_s1 ./file1 ./file_o1   ./dir_l2 ./dir_s2 ./file2 ./file_o2   ./dir_l3 ./dir_s3 ./file3 ./file_o3

