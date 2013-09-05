CC = gcc
LDFLAGS :=
CFLAGS := -c -Wall -Werror
INCLUDES := -I.


.PHONY : all
all : libuflow.so


libuflow.o :
	$(CC) $(CFLAGS) $(INCLUDES) -fpic libuflow.c


libuflow.so : libuflow.o
	$(CC) -shared -o libuflow.so libuflow.o


.PHONY : install
install : libuflow.so
	sudo cp ${PWD}/libuflow.so /usr/lib
	sudo cp ${PWD}/libuflow.h /usr/include
	sudo chmod 0755 /usr/lib/libuflow.so
	sudo ldconfig


.PHONY : uninstall
uninstall :
	sudo rm /usr/lib/libuflow.so
	sudo rm /usr/include/libuflow.h
	sudo ldconfig


.PHONY : clean
clean :
	rm -f *.o libuflow.so
