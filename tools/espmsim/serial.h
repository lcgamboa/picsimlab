

char * serial_list(void);
#ifdef LINUX
int serial_open(const char * sname);
int serial_cfg(int serialfd,int baud);
int serial_close(int serialfd);
unsigned long serial_send(int serialfd, const unsigned char * c, unsigned int  len);
unsigned long serial_rec(int serialfd, unsigned char * c, unsigned int len);
#else
HANDLE serial_open(const char * sname);
int serial_cfg(HANDLE serialfd,int baud);
int serial_close(HANDLE serialfd);
unsigned long serial_send(HANDLE serialfd, const unsigned char * c, unsigned int  len);
unsigned long serial_rec(HANDLE serialfd, unsigned char * c, unsigned int len);
#endif

