all: main.o scanner.o
	gcc -o t main.o scanner.o

scanner.cpp: scanner.re
	re2c -is scanner.re.cpp > scanner.cpp

clean:
	rm -rf *.o t
