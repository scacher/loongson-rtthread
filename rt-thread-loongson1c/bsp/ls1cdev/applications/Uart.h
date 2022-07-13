//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//作者： 安徽大学电子信息工程学院  王FN QQ2388309910
// 
////////////////////////////////////////////////////////////////////////////////// 
#define UART_BUFF_LEN  	16  	//定义发送缓冲区长度
extern u8 UART1_Get_OK;//
extern u8 UART1_RX_Buff[UART_BUFF_LEN];     //发送缓冲,最大USART_REC_LEN个字节.
extern rt_device_t serial1;

//void Uart1_Init(unsigned int baudrate );
void test_usart1(void);



