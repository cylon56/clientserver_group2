SHELL = /bin/sh
FLAGS = -Wall -g -c
CC = g++
PROG = echo_s log_s echo_c
OBJS = echo_s.o log_s.o echo_c.o

all : echo_s log_s echo_c

echo_s : echo_s.o
	$(CC) -o echo_s echo_s.o
log_s : log_s.o
	$(CC) -o log_s log_s.o
echo_c : echo_c.o
	$(CC) -o echo_c echo_c.o
echo_s.o : echo_s.h
	$(CC) $(FLAGS) echo_s.c
log_s.o :
	$(CC) $(FLAGS) log_s.c
echo_c.o :
	$(CC) $(FLAGS) echo_c.c
clean: 
	rm -f core $(PROG) $(OBJS)
