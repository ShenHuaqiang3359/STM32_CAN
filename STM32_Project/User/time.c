#include "stm32f4xx.h"                  // Device header
#include "./can/bsp_can.h"
extern CanTxMsg  TxMessage;

/*
 * ��������TIME1_Init
 * ����  ��TIME1��ʼ��
 * ����  ����
 * ���  : ��
 * ����  ���ⲿ����
 */
void TIME1_Init()
{ 
		/*��������������ǰ��*/
		NVIC_InitTypeDef NVIC_InitStructure;                 //NVIC�ṹ��
		TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;  //ʱ����Ԫ�ṹ��
	
		/*����TIME1ʱ��*/
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	
		/*ѡ��ʱ��Դ*/
		TIM_InternalClockConfig(TIM1);                 //ѡ��TIM2��ʱ��ԴΪ�ڲ�ʱ�ӣ�Ĭ��Ҳ���ڲ�ʱ�ӣ��˴���ʽ���ø�������180MHZ
	
	    /*����ʱ����Ԫ*/
		TIM_TimeBaseInitStructure.TIM_ClockDivision =TIM_CKD_DIV1;        //ʱ�ӷָ����Ƶ��TIM_CKD_DIV1��ʾ����ʱ���������ʱ��ͬ��
		TIM_TimeBaseInitStructure.TIM_CounterMode =TIM_CounterMode_Up;   // ����ģʽ�����ϼ�������0�������Զ���װ��ֵ������󴥷������¼���
		TIM_TimeBaseInitStructure.TIM_Period = 10000-1;                 //�Զ���װ��ֵ��������9999ʱ�����10000-1��
		TIM_TimeBaseInitStructure.TIM_Prescaler = 18000-1;          // Ԥ��Ƶ����180MHZ/((18000-1)+1)/((10000-1)+1)=1s
		TIM_TimeBaseInitStructure.TIM_RepetitionCounter =0;       // �ظ������������߼���ʱ����Ч��ͨ�ö�ʱ������Ϊ0
		TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);
	
		/*����TIME1�ж�*/
		TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
  
		/*����NVIC*/  
		NVIC_InitStructure.NVIC_IRQChannel =TIM1_UP_TIM10_IRQn;   // ָ���ж�ͨ����TIM2�ж�
		NVIC_InitStructure.NVIC_IRQChannelCmd =ENABLE;   //ʹ���ж�
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;  //��ռ���ȼ�0
		NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;  //��Ӧ���ȼ�0
		NVIC_Init(&NVIC_InitStructure);
		//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);   // ��������ֻ�趨��һ�Σ����������ط������
	
		/*������ʱ��1*/
		TIM_Cmd(TIM1, ENABLE);
}



/*
 * ��������TIM1_UP_TIM10_IRQHandler
 * ����  ��TIME1�жϺ�����������������ж�              
 * ����  ����
 * ���  : ��
 * ����  ���ⲿ����
 */
/*	 TIM_IT_Update ��ʱ���� �����¼����������������ʱ�ᴥ����
     ��ʱ����������CNT�������ֵ������ 0�����磬���ϼ���ģʽ����
     ��ʱ����������CNT���� 0 ���������ֵ�����磬���¼���ģʽ����
     �ֶ����������¼���ͨ�������λ UG λ����
     ��ʱ����ʼ����ɺ��״�����ʱ��*/
void TIM1_UP_TIM10_IRQHandler()
{
	/*�ж��Ƿ�����ж�*/
	if(TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
	{
		CAN_Transmit(CAN1, &TxMessage);                   //���ڷ��ͱ���
		printf("\r\n���͵ı���Ϊ��");
		printf("\r\n ID��StdId��0x%x ",TxMessage.StdId);  //��ӡ���ͱ���ID
		CAN_DEBUG_ARRAY(TxMessage.Data,8);               //��ӡ���ͱ�������
		printf("\n");
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);    //�����־λ
	}
}
