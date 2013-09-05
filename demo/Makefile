CC = gcc
LDFLAGS :=
CFLAGS := -c -Wall -Werror
INCLUDES := -I.


.PHONY : all
all : test


test : uflow-test.o
	$(CC) $< -o $@ $(LDFLAGS)


.c.o:
	$(CC) $(INCLUDES) $(CFLAGS) $< -o $@


.PHONY : clean
clean :
	rm -f *.o test
