CC = gcc
LDFLAGS := -luflow
CFLAGS := -c -Wall -Werror
INCLUDES := -I.


.PHONY : all
all : test test-lib


test : uflow-test.o
	$(CC) $< -o $@ $(LDFLAGS)


test-lib : uflow-test-lib.o
	$(CC) $< -o $@ $(LDFLAGS)


.c.o:
	$(CC) $(INCLUDES) $(CFLAGS) $< -o $@


.PHONY : clean
clean :
	rm -f *.o test test-lib
