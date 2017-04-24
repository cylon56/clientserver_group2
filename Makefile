SHELL = /bin/sh
FLAGS = -Wall
CC = g++

all:    echo_s log_s echo_c

echo_s:     echo_s.c
	$(CC) $(FLAGS) -g -o $@ echo_s.c
log_s:	    log_s.c
	$(CC) $(FLAGS) -g -o $@ log_s.c
echo_c:     echo_c.c
	$(CC) $(FLAGS) -g -o $@ echo_c.c
clean:
	rm echo_s log_s echo_c
