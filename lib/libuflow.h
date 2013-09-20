#ifndef LIBUFLOW_H
#define LIBUFLOW_H

#ifdef __cplusplus
extern "C" {
#endif

	const int REGISTER_NB = 32;

	void *config_init(const char *path);

	void config_write(void *ptr, unsigned int addr, unsigned int value);

	void config_write_array(void *ptr, unsigned int addr,
				unsigned int *array, int length);

	int config_read(void *ptr, unsigned int addr);

#ifdef __cplusplus
}
#endif
#endif				/* LIBUFLOW_H */
