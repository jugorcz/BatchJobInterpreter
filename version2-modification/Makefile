CC=gcc
CFLAGS= -Wall
%.o: %.c
	$(CC) -c -o $@ $^ $(CFLAGS)

all: main clean

main: main.o
	$(CC) -o $@ $< $(CFLAGS)

clean:
	rm *.o
	
cleanAll:
	rm main
