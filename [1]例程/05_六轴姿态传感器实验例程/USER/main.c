
#include "main.h"//主函数头文件

int main(void) 
{	
	SYS_Init();//系统初始化总函数
	while(1)   //主循环
	{
		MPU_Read();    //MPU6050数据读取
		DATA_Report(); //MPU6050数据上报
	}
}
/**
  * @brief  系统初始化总函数
  * @param  无
  * @retval 无
  */
void SYS_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //中断优先级分组函数
	delay_init();	    							 //延时函数初始化	  
	uart_init(115200);	 	                         //串口初始化为115200
	LED_Init();		                               	 //初始化与LED连接的硬件接口
	
	MPU_Init();	                                     //初始化MPU6050
	while(mpu_dmp_init())                            //初始化mpu_dmp库
 	{
		LED1 = !LED1;delay_ms(50);LED1 = !LED1;     //LED闪烁指示
		printf("Initialization failed！\r\n");		//串口初始化失败上报
	}
	printf("Initialization succeed！\r\n");			//串口初始化成功上报
	delay_ms(999);									//延时初界面显示
	mpu6050.flag = 0;                               //采集成功标志位初始化
	mpu6050.speed = 0;								//上报速度初始化
}
/**
  * @brief  MPU6050数据读取函数
  * @param  无
  * @retval 无
  */
void MPU_Read(void)
{
	
	if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)//dmp处理得到数据，对返回值进行判断
	{ 
		temp=MPU_Get_Temperature();	                //得到温度值
		MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//得到加速度传感器数据
		MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//得到陀螺仪数据
		mpu6050.speed++;                            //上报速度自加
		if(mpu6050.speed == 4)						//上报速度阈值设置
		{
			mpu6050.flag = 1;						//采集成功标志位设置为有效
			mpu6050.speed = 0;						//上报速度归零
		}	
	}
	else 											//采集不成功										
	{
		mpu6050.flag = 0;							//采集成功标志位设置为无效
	}	
}
/**
  * @brief  MPU6050数据上报
  * @param  无
  * @retval 无
  */
void DATA_Report(void)
{
	if(mpu6050.flag == 1)						//采集成功时
	{ 
		if(temp<0)								//对数据正负判断，判断为负时
		{
			temp=-temp;							//对负数据取反
		}
		else                                    //判断为正时
		{
		}
		printf("temp:%d.%d,",temp/100,temp%10); //通过串口1输出温度
		
		temp=pitch*10;							 //赋temp为pitch
		if(temp<0)								//对数据正负判断，判断为负时
		{
			temp=-temp;						    //对负数据取反		
		}
		else                                    //判断为正时 
		{
		}
		printf("pitch:%d.%d,",temp/10,temp%10); //通过串口1输出pitch	
		
		temp=roll*10;                            //赋temp为roll
		if(temp<0)								//对数据正负判断，判断为负时
		{
			temp=-temp;						    //对负数据取反	
		}
		else                                    //判断为正时
		{
		}
		printf("roll:%d.%d,",temp/10,temp%10);//通过串口1输出roll
		
		temp=yaw*10;                           //赋temp为yaw
		if(temp<0)								//对数据正负判断，判断为负时
		{
			temp=-temp;						    //对负数据取反
		}
		else                                    //判断为正时
		{
		}
		printf("yaw:%d.%d,",temp/10,temp%10);//通过串口1输出yaw	
		printf("gyrox:%d,gyroy:%d,gyroz:%d,aacx:%d,aacy:%d,aacz:%d\r\n",gyrox,gyroy,gyroz,aacx,aacy,aacz);//上报角速度数据，角加速度数据
		LED1=!LED1;								 			//LED闪烁
		mpu6050.flag = 0;									//采集成功标志位设置为无效
	}
	else ;														//防卡死
}
