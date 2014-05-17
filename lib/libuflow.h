#ifndef LIBUFLOW_H
#define LIBUFLOW_H

#ifdef __cplusplus
extern "C" {
#endif

	#define REGISTER_NB 32

	int config_init(const char *path);

	int config_read(unsigned int addr);

	void config_write(unsigned int addr, unsigned int value);

	void config_write_array(unsigned int addr,
				unsigned int *array, int length);

	void config_write_sequence(const unsigned int *addr,
				   const unsigned int *array, int length);

	void config_poll(unsigned int addr, unsigned int value);
#ifdef __cplusplus
}
#endif
#endif				/* LIBUFLOW_H */
