all:
	clang -o LogisticRegression src/main.c -lm

clean:
	rm LogisticRegression
