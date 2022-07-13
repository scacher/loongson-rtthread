#define trig_gpio 74
#define echo_gpio 76

#define WAITING_HIGH   0
#define WAITING_LOW    1
#define CATCHING       2
#define TIME_OUT       3

extern unsigned char IRQ_flag;
extern float distance2;
extern u8    dc_update;


void test_Srf05(void);

