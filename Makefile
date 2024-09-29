CFLAGS:=-I/usr/src/linux/include -DMODULE

all: sesam-mod.o

sesam-mod.o: sesam-mod.c
	$(CC) $(CFLAGS) -o $@ -c $^

clean:
	rm -f sesam-mod.o
