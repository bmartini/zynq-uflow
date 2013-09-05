#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>

#define REGISTER_NB	8

int main(int argc, char *argv[])
{
	int fd;
	void *map_addr;
	volatile unsigned int *mapped;
	int i;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <uio-dev-file>\n", argv[0]);
		exit(1);
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		perror("Failed to open <uio-dev-file>");
		return 1;
	}

	map_addr =
	    mmap(NULL, REGISTER_NB, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (map_addr == MAP_FAILED) {
		perror("Failed to mmap");
		return 1;
	}

	mapped = map_addr;

	/*
	 * write and read to all AXI Lite registers
	 */

	for (i = 0; i < REGISTER_NB; i++) {
		mapped[i] = i;
	}

	for (i = 0; i < REGISTER_NB; i++) {
		printf("%d ", mapped[i]);
	}

	printf("\n");

	munmap(map_addr, REGISTER_NB);

	close(fd);
	return 0;
}
