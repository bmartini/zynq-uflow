#include <libuflow.h>

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int i;
	int value;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <uio-dev-file>\n", argv[0]);
		exit(1);
	}

	if (config_init(argv[1]) != 0) {
		return (1);
	}

	for (i = 0; i < REGISTER_NB; i++) {
		config_write(i, i * 2);
	}

	for (i = 0; i < REGISTER_NB; i++) {
		value = config_read(i);
		printf("reg %d: %d: %x\n", i, value, value);
	}

	return (0);
}
