#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "BMP388.h"

extern struct bmp3_calib_data  calib_data;
extern struct bmp3_uncomp_data uncomp_data;
extern struct bmp3_data        comp_data;

void print_temperature_pressure(int64_t temperature, uint64_t pressure) {
    float temp = (float)temperature / 100.0f;
    float pres = (float)pressure / 100.0f;
    printf("温度是%.2f ℃，气压是%.2f Pa\r\n", temp, pres);
}
int main(void)
{	
	uart_init(115200);
	delay_init();	
	printf("连接中...\r\n");
	while(BMP388_Init())
		printf("连接失败,重试中...\r\n");
	printf("连接成功..\r\n");
	BMP388_ON();
	printf("采样中....\r\n");
	
	while(1)
	{
		delay_ms(3);
		BMP388_Get_PaT();
		print_temperature_pressure(comp_data.temperature, comp_data.pressure);
		delay_ms(1000);
	}
}




/*
	printf("修正系数如下\r\n");
	printf("T1=%ud\r\n",calib_data.par_t1);
	printf("T2=%ud\r\n",calib_data.par_t2);
	printf("T3=%d\r\n", calib_data.par_t3);
	printf("P1=%d\r\n", calib_data.par_p1);
	printf("P2=%d\r\n", calib_data.par_p2);
	printf("P3=%d\r\n", calib_data.par_p3);
	printf("P4=%d\r\n", calib_data.par_p4);
	printf("P5=%ud\r\n",calib_data.par_p5);
	printf("P6=%ud\r\n",calib_data.par_p6);
	printf("P7=%d\r\n", calib_data.par_p7);
	printf("P8=%d\r\n", calib_data.par_p8);
	printf("P9=%d\r\n", calib_data.par_p9);
	printf("P10=%d\r\n",calib_data.par_p10);
	printf("P11=%d\r\n",calib_data.par_p11);
	printf("TL=%lld\r\n",calib_data.t_lin);
*/
