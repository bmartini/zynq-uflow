#include "libuflow.h"

#include <assert.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

void *config_init(const char *path)
{
	int fd;
	void *ptr;
	assert((fd = open(path, O_RDWR)) >= 0);

	ptr =
	    mmap(NULL, REGISTER_NB, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	close(fd);
	if (ptr == MAP_FAILED) {
		return NULL;
	} else {
		return ptr;
	}
}

void config_write(void *ptr, unsigned int addr, unsigned int value)
{
	volatile unsigned int *reg = ((volatile unsigned int *) ptr) + addr;

	*reg = value;
}

void config_write_array(void *ptr, unsigned int addr, unsigned int *array,
			int length)
{
	int xx;
	volatile unsigned int *reg = ((volatile unsigned int *) ptr) + addr;

	for (xx = 0; xx < length; xx++) {
		*reg = array[xx];
	}
}

int config_read(void *ptr, unsigned int addr)
{
	volatile unsigned int *reg = ((volatile unsigned int *) ptr) + addr;

	return *reg;
}
