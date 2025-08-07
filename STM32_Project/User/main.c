#include "stm32f4xx.h"
#include "bsp_led.h"
#include "bsp_debug_usart.h"
#include "bsp_can.h"
#include "bsp_key.h"
#include "delay.h"
#include "time.h"

/*

1、按键WAKUP按下发送一次报文，并通过串口打印

2、定时器中断 1s 发送一次报文，并通过串口打印

3、接收到报文进入中断，打印接收到的报文信息

*/

__IO uint32_t flag = 0;			 //用于标志是否接收到数据，在中断函数中赋值
CanTxMsg TxMessage;			     //发送报文缓冲区
CanRxMsg RxMessage;				 //接收报文缓冲区
uint8_t Data[8]={0,1,2,3,4,5,6,7}; //发送报文数据




int main(void)
{
	/*LED初始化*/
	LED_GPIO_Config();
	
	/*初始化USART1*/
	Debug_USART_Config();
	
	/*初始化按键*/
	Key_GPIO_Config();
 
	/*初始化can,在中断接收CAN数据包*/
	CAN_Config();
	
	/*TIME1初始化*/
	TIME1_Init();
	

  while(1)
	{
		/*设置发送报文*/
		CAN_SetMsg(&TxMessage,Data); 
		
		/*按一次WAKUP按键发送一次数据*/
		if(	Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON)
		{
			
			CAN_Transmit(CAN1, &TxMessage);            //发送报文
			Delay_ms(100);                             //等待发送完毕
			 			
			printf("\r\n发送的报文内容为：");
			printf("\r\n ID号StdId：0x%x" ,TxMessage.StdId);  //串口打印发送报文ID
			CAN_DEBUG_ARRAY(TxMessage.Data,8);                // 串口打印发送报文数据
			printf("\n");
			GPIO_ResetBits(LED1_GPIO_PORT,LED1_PIN);          // LED亮

		}
		
		/*判断是否接收到CAN报文，接收到报文进入中断*/
		if(flag==1)
		{		
			
			printf("\r\n接收的报文内容为：");
			printf("\r\n ID号StdId：0x%x",RxMessage.StdId);    //串口打印收到的报文ID
			CAN_DEBUG_ARRAY(RxMessage.Data,8);                  //打印接收到的报文数据
			printf("\n");
			flag=0;                                             // 清楚接收标志位
		}
	
	
	}

}




