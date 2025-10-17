
#include "main.h"//������ͷ�ļ�

int main(void) 
{	
	SYS_Init();//ϵͳ��ʼ���ܺ���
	while(1)   //��ѭ��
	{
		MPU_Read();    //MPU6050���ݶ�ȡ
		DATA_Report(); //MPU6050�����ϱ�
	}
}
/**
  * @brief  ϵͳ��ʼ���ܺ���
  * @param  ��
  * @retval ��
  */
void SYS_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //�ж����ȼ����麯��
	delay_init();	    							 //��ʱ������ʼ��	  
	uart_init(115200);	 	                         //���ڳ�ʼ��Ϊ115200
	LED_Init();		                               	 //��ʼ����LED���ӵ�Ӳ���ӿ�
	
	MPU_Init();	                                     //��ʼ��MPU6050
	while(mpu_dmp_init())                            //��ʼ��mpu_dmp��
 	{
		LED1 = !LED1;delay_ms(50);LED1 = !LED1;     //LED��˸ָʾ
		printf("Initialization failed��\r\n");		//���ڳ�ʼ��ʧ���ϱ�
	}
	printf("Initialization succeed��\r\n");			//���ڳ�ʼ���ɹ��ϱ�
	delay_ms(999);									//��ʱ��������ʾ
	mpu6050.flag = 0;                               //�ɼ��ɹ���־λ��ʼ��
	mpu6050.speed = 0;								//�ϱ��ٶȳ�ʼ��
}
/**
  * @brief  MPU6050���ݶ�ȡ����
  * @param  ��
  * @retval ��
  */
void MPU_Read(void)
{
	
	if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)//dmp����õ����ݣ��Է���ֵ�����ж�
	{ 
		temp=MPU_Get_Temperature();	                //�õ��¶�ֵ
		MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//�õ����ٶȴ���������
		MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//�õ�����������
		mpu6050.speed++;                            //�ϱ��ٶ��Լ�
		if(mpu6050.speed == 4)						//�ϱ��ٶ���ֵ����
		{
			mpu6050.flag = 1;						//�ɼ��ɹ���־λ����Ϊ��Ч
			mpu6050.speed = 0;						//�ϱ��ٶȹ���
		}	
	}
	else 											//�ɼ����ɹ�										
	{
		mpu6050.flag = 0;							//�ɼ��ɹ���־λ����Ϊ��Ч
	}	
}
/**
  * @brief  MPU6050�����ϱ�
  * @param  ��
  * @retval ��
  */
void DATA_Report(void)
{
	if(mpu6050.flag == 1)						//�ɼ��ɹ�ʱ
	{ 
		if(temp<0)								//�����������жϣ��ж�Ϊ��ʱ
		{
			temp=-temp;							//�Ը�����ȡ��
		}
		else                                    //�ж�Ϊ��ʱ
		{
		}
		printf("temp:%d.%d,",temp/100,temp%10); //ͨ������1����¶�
		
		temp=pitch*10;							 //��tempΪpitch
		if(temp<0)								//�����������жϣ��ж�Ϊ��ʱ
		{
			temp=-temp;						    //�Ը�����ȡ��		
		}
		else                                    //�ж�Ϊ��ʱ 
		{
		}
		printf("pitch:%d.%d,",temp/10,temp%10); //ͨ������1���pitch	
		
		temp=roll*10;                            //��tempΪroll
		if(temp<0)								//�����������жϣ��ж�Ϊ��ʱ
		{
			temp=-temp;						    //�Ը�����ȡ��	
		}
		else                                    //�ж�Ϊ��ʱ
		{
		}
		printf("roll:%d.%d,",temp/10,temp%10);//ͨ������1���roll
		
		temp=yaw*10;                           //��tempΪyaw
		if(temp<0)								//�����������жϣ��ж�Ϊ��ʱ
		{
			temp=-temp;						    //�Ը�����ȡ��
		}
		else                                    //�ж�Ϊ��ʱ
		{
		}
		printf("yaw:%d.%d,",temp/10,temp%10);//ͨ������1���yaw	
		printf("gyrox:%d,gyroy:%d,gyroz:%d,aacx:%d,aacy:%d,aacz:%d\r\n",gyrox,gyroy,gyroz,aacx,aacy,aacz);//�ϱ����ٶ����ݣ��Ǽ��ٶ�����
		LED1=!LED1;								 			//LED��˸
		mpu6050.flag = 0;									//�ɼ��ɹ���־λ����Ϊ��Ч
	}
	else ;														//������
}
