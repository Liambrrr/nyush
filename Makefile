CC = gcc
CFLAGS = -Wall -Wextra -O2

OBJS = main.o command.o signal_handling.o job.o execution.o error_handling.o

nyush: $(OBJS)
	$(CC) $(CFLAGS) -o nyush $(OBJS)

main.o: main.c command.h signal_handling.h error_handling.h job.h execution.h
	$(CC) $(CFLAGS) -c main.c

command.o: command.c command.h
	$(CC) $(CFLAGS) -c command.c

signal_handling.o: signal_handling.c signal_handling.h
	$(CC) $(CFLAGS) -c signal_handling.c

job.o: job.c job.h
	$(CC) $(CFLAGS) -c job.c

execution.o: execution.c execution.h
	$(CC) $(CFLAGS) -c execution.c

error_handling.o: error_handling.c error_handling.h
	$(CC) $(CFLAGS) -c error_handling.c

clean:
	rm -f nyush $(OBJS)

