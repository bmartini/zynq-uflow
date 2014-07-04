#include "libuflow.h"

#include <assert.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

static void *ptr;

int config_init(const char *path)
{
	int fd;
	assert((fd = open(path, O_RDWR)) >= 0);

	ptr =
	    mmap(NULL, REGISTER_NB, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	close(fd);
	if (ptr == MAP_FAILED) {
		return -1;	// failure
	} else {
		return 0;	// success
	}
}

void config_write(unsigned int addr, unsigned int value)
{
	volatile unsigned int *reg = ((volatile unsigned int *)ptr) + addr;

	*reg = value;
}

void config_write_array(unsigned int addr, const unsigned int *array, int length)
{
	int xx;
	volatile unsigned int *reg = ((volatile unsigned int *)ptr) + addr;

	for (xx = 0; xx < length; xx++) {
		*reg = array[xx];
	}
}

void config_write_sequence(const unsigned int *addr, const unsigned int *array,
			   int length)
{
	int xx;
	volatile unsigned int *reg = ((volatile unsigned int *)ptr);

	for (xx = 0; xx < length; xx++) {
		*(reg + addr[xx]) = array[xx];
	}
}

int config_read(unsigned int addr)
{
	volatile unsigned int *reg = ((volatile unsigned int *)ptr) + addr;

	return *reg;
}

void config_poll(unsigned int addr, unsigned int value)
{
	while (value > config_read(addr)) ;
}
