#include "stm32f4xx.h"
#include "bsp_led.h"
#include "bsp_debug_usart.h"
#include "bsp_can.h"
#include "bsp_key.h"
#include "delay.h"
#include "time.h"

/*

1������WAKUP���·���һ�α��ģ���ͨ�����ڴ�ӡ

2����ʱ���ж� 1s ����һ�α��ģ���ͨ�����ڴ�ӡ

3�����յ����Ľ����жϣ���ӡ���յ��ı�����Ϣ

*/

__IO uint32_t flag = 0;			 //���ڱ�־�Ƿ���յ����ݣ����жϺ����и�ֵ
CanTxMsg TxMessage;			     //���ͱ��Ļ�����
CanRxMsg RxMessage;				 //���ձ��Ļ�����
uint8_t Data[8]={0,1,2,3,4,5,6,7}; //���ͱ�������




int main(void)
{
	/*LED��ʼ��*/
	LED_GPIO_Config();
	
	/*��ʼ��USART1*/
	Debug_USART_Config();
	
	/*��ʼ������*/
	Key_GPIO_Config();
 
	/*��ʼ��can,���жϽ���CAN���ݰ�*/
	CAN_Config();
	
	/*TIME1��ʼ��*/
	TIME1_Init();
	

  while(1)
	{
		/*���÷��ͱ���*/
		CAN_SetMsg(&TxMessage,Data); 
		
		/*��һ��WAKUP��������һ������*/
		if(	Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON)
		{
			
			CAN_Transmit(CAN1, &TxMessage);            //���ͱ���
			Delay_ms(100);                             //�ȴ��������
			 			
			printf("\r\n���͵ı�������Ϊ��");
			printf("\r\n ID��StdId��0x%x" ,TxMessage.StdId);  //���ڴ�ӡ���ͱ���ID
			CAN_DEBUG_ARRAY(TxMessage.Data,8);                // ���ڴ�ӡ���ͱ�������
			printf("\n");
			GPIO_ResetBits(LED1_GPIO_PORT,LED1_PIN);          // LED��

		}
		
		/*�ж��Ƿ���յ�CAN���ģ����յ����Ľ����ж�*/
		if(flag==1)
		{		
			
			printf("\r\n���յı�������Ϊ��");
			printf("\r\n ID��StdId��0x%x",RxMessage.StdId);    //���ڴ�ӡ�յ��ı���ID
			CAN_DEBUG_ARRAY(RxMessage.Data,8);                  //��ӡ���յ��ı�������
			printf("\n");
			flag=0;                                             // ������ձ�־λ
		}
	
	
	}

}




