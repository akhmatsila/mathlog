.PHONY: all, run, clean
SOURCES=$(shell find -type f -name "*.cpp")
HEADERS=$(shell find -type f -name "*.h")

all:main.o operation.o axiom.o parser.o proof.o
	g++ main.o operation.o axiom.o parser.o proof.o --std=c++14 -o  program

main.o:main.cpp
	g++ -c main.cpp -o main.o

operation.o: operation.cpp operation.h
	g++ -c operation.cpp -o operation.o

axiom.o: axiom.cpp axiom.h
	g++ -c axiom.cpp -o axiom.o

parser.o: parser.cpp parser.h
	g++ -c parser.cpp -o parser.o

proof.o: proof.cpp proof.h
	g++ -c proof.cpp -o proof.o

run:
	./program

clean:
	rm ./program rm *.o


