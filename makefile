# Make INYEONG Shell

CC = gcc
TARGET = myshell

$(TARGET) : myshell.o
	$(CC) -o $(TARGET) myshell.o

myshell.o : myshell.c
	$(CC) -c -o myshell.o myshell.c

clean:
	rm *.o myshell
