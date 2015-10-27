

typedef struct
{
unsigned int SIZE;
unsigned char ADDRB;

unsigned char *data;
unsigned short addr;

unsigned char datab;
unsigned char datas;

unsigned char ctrl;

unsigned char sclo;
unsigned char sdao;

unsigned char ret;

unsigned char bit;
unsigned char byte;
}mi2c_t;


void mi2c_init_null(mi2c_t *mem);
void mi2c_rst(mi2c_t *mem);
void mi2c_init(mi2c_t *mem,int sizekbits);
void mi2c_end(mi2c_t *mem);

unsigned char mi2c_io(mi2c_t *mem, unsigned char scl, unsigned char sda);
