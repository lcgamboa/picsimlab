

extern char SERIALDEVICE[100];

char * slist(void);
int sopen(const char * port);
int sclose(void);
int scfg(void);
unsigned long ssend(unsigned char c);
unsigned long srec(unsigned char * c);
unsigned long srec_tout(unsigned char * c);

