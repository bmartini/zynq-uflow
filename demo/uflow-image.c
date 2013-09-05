#include "image_data_100.h"

#include <libxdma.h>
#include <libuflow.h>

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	uint32_t *src;
	uint32_t *dst;
	void *config_ptr;
	int i;

	if (xdma_init() < 0) {
		exit(EXIT_FAILURE);
	}

	config_ptr = config_init("/dev/uflow-accel");
	if (NULL == config_ptr) {
		exit(EXIT_FAILURE);
	}

	dst = (uint32_t *) xdma_alloc(DATA_SIZE, sizeof(uint32_t));
	src = (uint32_t *) xdma_alloc(DATA_SIZE, sizeof(uint32_t));

	// fill src lena
	for (i = 0; i < DATA_SIZE; i++) {
		src[i] = data[i];
	}

	// fill dst with zero
	for (i = 0; i < DATA_SIZE; i++) {
		dst[i] = 1;
	}

	// reset
	config_write(config_ptr, 0, 0x00000001);
	while (1 != config_read(config_ptr, 0)) ;
	config_write(config_ptr, 0, 0x00000000);
	while (0 != config_read(config_ptr, 0)) ;
	// reset end

	// config bus values
	config_write(config_ptr, 1, ((IMAGE_SIZE << 16) | (IMAGE_SIZE & 0xffff)));	// image height/width
	config_write(config_ptr, 1, 0x000a000a);	// kernel height/width
	config_write(config_ptr, 1, 0x00000001);	// mode
	config_write(config_ptr, 1, 0x00000000);	// sub x/y

	// config kernel values
	for (i = 0; i < 100; i++) {
		config_write(config_ptr, 2, 0x00000100);
	}

	if (0 < xdma_num_of_devices()) {
		xdma_perform_transaction(0, XDMA_WAIT_NONE, src, DATA_SIZE, dst,
					 DATA_SIZE);
	}

	printf("P2\n");
	printf("# LENA test pgm image\n");
	printf("%d %d\n", IMAGE_SIZE, IMAGE_SIZE);
	printf("255\n");
	for (i = 0; i < DATA_SIZE; i++) {
		printf("%d\n", dst[i] / 255);
	}

	xdma_exit();

	return (0);
}
