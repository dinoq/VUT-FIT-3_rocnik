CC=g++
CFLAGS=
DEPS = ims.hpp


all: simulation 

run:
	./ims
	
%.o: %.cc $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

simulation: ims.o Simulace.o Obce.o
	$(CC) -o ims ims.o Simulace.o Obce.o -lsimlib -lm
