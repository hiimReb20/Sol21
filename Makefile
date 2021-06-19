CC = gcc
CFLAGS = -std=c99 -lm -lpthread -g
VALGRIND = valgrind --leak-check=full
CON1 = ./configurazioni/config1.txt
CON2 = ./configurazioni/config2.txt
NOMES = ./Server.c
NOMEC = ./Client.c

.PHONY: all server.o client.o ./lib/myAPI.o ./lib/mylib.o ./lib/icl_hash.o ./lib/parser.o test1 test2 client test2val test3 clean

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


client:
	@$(VALGRIND) ./client.o &
	@#$(VALGRIND) ./client.o &


test1:
	@$(VALGRIND) ./server.o $(CON1) & echo $$! > t1.PID &
	@sleep 15; \
	kill -1 $$(cat t1.PID); \


test2:
	@(./server.o $(CON2) 2 & echo $$! > t2.PID) &
	@sleep 15; \
	kill -1 $$(cat t2.PID); \

test2val:
	@$(VALGRIND) ./server.o $(CON2) & echo $$! > t2.PID &
	@sleep 10; \
	#kill -1 $$(cat t2.PID); 
	@kill -3 $$(cat t2.PID); \

test3:
	@$(VALGRIND) ./server.o $(CON2) & echo $$! > t2.PID &
	@$(VALGRIND) ./client.o &
	@sleep 3; \
	kill -1 $$(cat t2.PID); \

clean:
	@rm -f *.o libmyl.a libmyA.a libicl.a libp.a ./lib/mylib.o ./lib/myAPI.o ./lib/icl_hash.o ./lib/parser.o mysock* *.PID ./dir/*.txt output
