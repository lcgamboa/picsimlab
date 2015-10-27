

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
}rtc_t;


void rtc_rst(rtc_t *rtc);
void rtc_init(rtc_t *rtc);
void rtc_update(rtc_t *rtc);
void rtc_end(rtc_t *rtc);

unsigned char rtc_io(rtc_t *rtc, unsigned char scl, unsigned char sda);
