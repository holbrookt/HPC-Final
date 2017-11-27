all: sgd

downpour:
	clang -c src/weights.c -o src/weights.o
	clang -c src/data.c -o src/data.o
	clang -c src/util.c -o src/util.o
	clang -c src/downpour.c -o src/downpout.o
	clang -o DownpourLogisticRegression src/downpour.o src/data.o src/util.o src/weights.o -lm 


sgd:
	clang -c src/weights.c -o src/weights.o
	clang -c src/data.c -o src/data.o
	clang -c src/util.c -o src/util.o
	clang -c src/main.c -o src/main.o
	clang -o LogisticRegression src/main.o src/data.o src/util.o src/weights.o -lm 

clean:
	rm src/*.o
	rm LogisticRegression
