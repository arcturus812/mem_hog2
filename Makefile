CC = gcc
CFLAGS = -g
LDFLAGS = -static

mem_acc: mem_acc.o
	$(CC) $(LDFLAGS) -o $@ $^

mem_acc.o: mem_acc.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f mem_acc.o mem_acc
