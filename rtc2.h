

typedef struct
{

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
}rtc2_t;


void rtc2_rst(rtc2_t *rtc);
void rtc2_init(rtc2_t *rtc);
void rtc2_update(rtc2_t *rtc);
void rtc2_end(rtc2_t *rtc);

unsigned char rtc2_io(rtc2_t *rtc, unsigned char scl, unsigned char sda);
