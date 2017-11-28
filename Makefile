CC = gcc
MPICC = mpicc 

all: sgd

downpour:
	$(CC) -c src/weights.c -o src/weights.o
	$(CC) -c src/data.c -o src/data.o
	$(CC) -c src/util.c -o src/util.o
	$(MPICC) -c src/downpour.c -o src/downpour.o
	$(MPICC) -o DownpourLogisticRegression src/downpour.o src/data.o src/util.o src/weights.o -lm 


sgd:
	$(CC) -c src/weights.c -o src/weights.o -g
	$(CC) -c src/data.c -o src/data.o -g
	$(CC) -c src/util.c -o src/util.o -g
	$(CC) -c src/main.c -o src/main.o -g
	$(CC) -o LogisticRegression src/main.o src/data.o src/util.o src/weights.o -lm -lrt -g

clean:
	rm src/*.o
	rm LogisticRegression
