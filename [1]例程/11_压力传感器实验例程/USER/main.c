#include "stm32f10x.h"
#include "led.h"
#include "usart.h"
#include "delay.h"
#include "oled.h"
#include "hx711.h"



int value;
float weight;
int32_t reset;
u8 buff[30];

float Weights=500.0;  //100g
int32_t Weights_100=8493860;  //100g

int main(void)
{ 
	
  SystemInit();//����ϵͳʱ��Ϊ72M	
	delay_init(72);

	HX711_Init();
	USART1_Config();//���ڳ�ʼ��
	

	printf("Start \n");
	delay_ms(1000);

	
	reset = HX711_GetData();

  while (1)
  {
//		LED_Toggle();
		value = HX711_GetData();
		printf("%d\r\n",value);
		
		weight=(float)(value-reset)*Weights/430;
		
		sprintf((char*)buff,"%.1f   ",weight);
		
		printf("����: %f\r\n",weight);
		


		delay_ms(500);
  }
}




