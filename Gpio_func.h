#ifndef __GPIO_FUNC_H__


 /****************************************************************
 * Constants
 ****************************************************************/
 
#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define MAX_BUF 64

/****************************************************************
 * Functions
 ****************************************************************/

int gpio_export(unsigned int gpio);
int gpio_unexport(unsigned int gpio);
int gpio_set_dir(unsigned int gpio, unsigned int out_flag);
int gpio_set_value(unsigned int gpio, unsigned int value);
int gpio_get_value(unsigned int gpio, unsigned int *value);
int gpio_set_edge(unsigned int gpio, char *edge);
int gpio_fd_open(unsigned int gpio);
int gpio_fd_open_read(unsigned int gpio);
int gpio_fd_open_edge(unsigned int gpio);
int gpio_fd_close(int fd);
int mux_gpio_set(unsigned int gpio, unsigned int value);

#endif /* __GPIO_FUNC_H__ */
