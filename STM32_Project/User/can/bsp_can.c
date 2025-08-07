#include "./can/bsp_can.h"


extern __IO uint32_t flag ;		 //���ڱ�־�Ƿ���յ����ݣ����жϺ����и�ֵ
extern CanRxMsg RxMessage;		 //���ջ�����

/*
 * ��������CAN_GPIO_Config
 * ����  ��CAN��GPIO ����
 * ����  ����
 * ���  : ��
 * ����  ���ڲ�����
 */
static void CAN_GPIO_Config(void)
{
	/*GPIO ��ʼ���ṹ�����*/
 	GPIO_InitTypeDef GPIO_InitStructure_TX;                     //PB9 ��ʼ���ṹ�����
	GPIO_InitTypeDef GPIO_InitStructure_RX;                     //PB8 ��ʼ���ṹ�����
   	 

  /* ����GPIOʱ�� */
  RCC_AHB1PeriphClockCmd(CAN_TX_GPIO_CLK|CAN_RX_GPIO_CLK, ENABLE);
	
	 /* ����GPIO����ΪCAN */
  GPIO_PinAFConfig(CAN_TX_GPIO_PORT, CAN_RX_SOURCE, CAN_AF_PORT);   //PB9 ����Ϊ CAN_TX
  GPIO_PinAFConfig(CAN_RX_GPIO_PORT, CAN_TX_SOURCE, CAN_AF_PORT);   //PB8 ����Ϊ CAN_RX

	 /*  PB9 - CAN TX ���� */
  GPIO_InitStructure_TX.GPIO_Pin = CAN_TX_PIN;
  GPIO_InitStructure_TX.GPIO_Mode = GPIO_Mode_AF;                //����ģʽ      
  GPIO_InitStructure_TX.GPIO_Speed = GPIO_Speed_50MHz;           //�ٶ�
  GPIO_InitStructure_TX.GPIO_OType = GPIO_OType_PP;               //�������         
  GPIO_InitStructure_TX.GPIO_PuPd  = GPIO_PuPd_UP;               //��������
  GPIO_Init(CAN_TX_GPIO_PORT, &GPIO_InitStructure_TX);
	
	/* PB8 - CAN RX ���� */
  GPIO_InitStructure_RX.GPIO_Pin = CAN_RX_PIN ;
  GPIO_InitStructure_RX.GPIO_Mode = GPIO_Mode_AF;                //����ģʽ
  GPIO_InitStructure_RX.GPIO_Speed = GPIO_Speed_50MHz;           //�ٶ�
  GPIO_InitStructure_RX.GPIO_OType = GPIO_OType_PP;               //���������Ҳ���������룩
  GPIO_InitStructure_RX.GPIO_PuPd  = GPIO_PuPd_UP;               //��������
  GPIO_Init(CAN_RX_GPIO_PORT, &GPIO_InitStructure_RX);
}

/*
 * ��������CAN_NVIC_Config
 * ����  ��CAN��NVIC ����,��1���ȼ��飬0��0���ȼ�
 * ����  ����
 * ���  : ��
 * ����  ���ڲ�����
 */
static void CAN_NVIC_Config(void)
{
	/*NVIC ��ʼ���ṹ��*/
   	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* �ж����ȼ�����1 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	 /*�ж�����*/
	NVIC_InitStructure.NVIC_IRQChannel = CAN_RX_IRQ;	          //CAN1 RX0�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 //��ռ���ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			 //�����ȼ�Ϊ0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;              //�ж�ʹ��
    NVIC_Init(&NVIC_InitStructure);
}

/*
 * ��������CAN_Mode_Config
 * ����  ��CAN��ģʽ ����
 * ����  ����
 * ���  : ��
 * ����  ���ڲ�����
 */
static void CAN_Mode_Config(void)
{
	/*CAN ��ʼ���ṹ��*/
	CAN_InitTypeDef  CAN_InitStructure;
	
	/* ����CANʱ�� */
	RCC_APB1PeriphClockCmd(CAN_CLK, ENABLE);

	/*CAN�ָ�Ĭ��*/
	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);

	/*CAN��Ԫ��ʼ��*/
	CAN_InitStructure.CAN_TTCM=DISABLE;			   //MCR-TTCM  �ر�ʱ�䴥��ͨ��ģʽʹ��
	CAN_InitStructure.CAN_ABOM=ENABLE;			   //MCR-ABOM  �Զ����߹��� 
	CAN_InitStructure.CAN_AWUM=ENABLE;			   //MCR-AWUM  ʹ���Զ�����ģʽ
	CAN_InitStructure.CAN_NART=DISABLE;			   //MCR-NART  ��ֹ�����Զ��ش�	  DISABLE-�Զ��ش�
	CAN_InitStructure.CAN_RFLM=DISABLE;			   //MCR-RFLM  ����FIFO ����ģʽ  DISABLE-���ʱ�±��ĻḲ��ԭ�б���  
	CAN_InitStructure.CAN_TXFP=DISABLE;			   //MCR-TXFP  ����FIFO���ȼ� DISABLE-���ȼ�ȡ���ڱ��ı�ʾ�� 
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;  //��������ģʽ
	CAN_InitStructure.CAN_SJW=CAN_SJW_2tq;		   //BTR-SJW ����ͬ����Ծ��� 2��ʱ�䵥Ԫ
	 
	/* ss=1 bs1=5 bs2=3 λʱ����Ϊ(1+5+3) �����ʼ�Ϊʱ������tq*(1+3+6)  */
	CAN_InitStructure.CAN_BS1=CAN_BS1_5tq;		   //BTR-TS1 ʱ���1 ռ����5��ʱ�䵥Ԫ
	CAN_InitStructure.CAN_BS2=CAN_BS2_3tq;		   //BTR-TS1 ʱ���2 ռ����3��ʱ�䵥Ԫ	
	
	/* CAN Baudrate = 1 MBps (1MBps��Ϊstm32��CAN�������) (CAN ʱ��Ƶ��Ϊ APB 1 = 45 MHz) */
	CAN_InitStructure.CAN_Prescaler =5;		   ////BTR-BRP �����ʷ�Ƶ��  ������ʱ�䵥Ԫ��ʱ�䳤�� 45/(1+5+3)/5=1 Mbps=1000kpbs
	CAN_Init(CANx, &CAN_InitStructure);
}

/*
 * ��������CAN_Filter_Config
 * ����  ��CAN�Ĺ����� ����
 * ����  ����
 * ���  : ��
 * ����  ���ڲ�����
 */
static void CAN_Filter_Config(void)
{
	/*CAN ��������ʼ���ṹ��*/
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;

	/*CANɸѡ����ʼ��*/
	CAN_FilterInitStructure.CAN_FilterNumber=0;						//ɸѡ����0
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;	//����������ģʽ
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;	//ɸѡ��λ��Ϊ����32λ��
	/* ʹ��ɸѡ�������ձ�־�����ݽ��бȶ�ɸѡ��ID�������µľ����������ǵĻ��������FIFO0�� */

	CAN_FilterInitStructure.CAN_FilterIdHigh = (0x123 << 5) & 0xFFFF; // ID��16λ����׼֡����5λ������׼֡�� 11 λ ID ����洢�� 32 λ�������� �� 16 λ�ĵ� 11 λ�� bit5~bit15��  ��ɸѡ��IDΪ0x123�ı���
    CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;    // ID��16λ
    //CAN_FilterInitStructure.CAN_FilterMaskIdHigh = (0x7FF << 5) & 0xFFFF; // �����16λ
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x000;     //�����16λ������
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;    // �����16λ��������
	
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0 ;				//ɸѡ����������FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;			//ʹ��ɸѡ��
	CAN_FilterInit(&CAN_FilterInitStructure);
	/*CANͨ���ж�ʹ��*/
	CAN_ITConfig(CANx, CAN_IT_FMP0, ENABLE);
}


/*
 * ��������CAN_Config
 * ����  ����������CAN�Ĺ���
 * ����  ����
 * ���  : ��
 * ����  ���ⲿ����
 */
void CAN_Config(void)
{
	/*CAN GPIO��ʼ��*/
  CAN_GPIO_Config();
	
	/*CAN �жϳ�ʼ��*/
  CAN_NVIC_Config();
	
	/*CAN ģʽ��ʼ��*/
  CAN_Mode_Config();
	
	/*CAN ��������ʼ��*/
  CAN_Filter_Config();   
}


/*
 * ��������Init_RxMes
 * ����  ����ս��ձ���
 * ����  ����
 * ���  : ��
 * ����  ���ⲿ����
 */
void Init_RxMes(CanRxMsg *RxMessage)
{
  uint8_t ubCounter = 0;

	/*�ѽ��սṹ������*/
  RxMessage->StdId = 0x00;
  RxMessage->ExtId = 0x00;
  RxMessage->IDE = CAN_ID_STD;
  RxMessage->DLC = 0;
  RxMessage->FMI = 0;
  for (ubCounter = 0; ubCounter < 8; ubCounter++)
  {
    RxMessage->Data[ubCounter] = 0x00;
  }
}


/*
 * ��������CAN_SetMsg
 * ����  ��CANͨ�ű�����������
 * ����  �����ͱ��Ľṹ��
			������������
 * ���  : ��
 * ����  ���ⲿ����
 */	 
void CAN_SetMsg(CanTxMsg *TxMessage ,uint8_t *ch)
{	  
	uint8_t ubCounter = 0;

	/*CAN ���Ľṹ��*/
  TxMessage->StdId=0x123;				     //id
  TxMessage->ExtId=0x0000;					 
  TxMessage->IDE=CAN_ID_STD;			     //��׼֡ģʽ
  TxMessage->RTR=CAN_RTR_DATA;				 //���͵�������
  TxMessage->DLC=8;							 //���ݳ���Ϊ8�ֽ�
	
	/*����Ҫ���͵���������*/
	for (ubCounter = 0; ubCounter < 8; ubCounter++)
  {
    TxMessage->Data[ubCounter] = ch[ubCounter];
  }
}

/*
 * ��������CAN_RX_IRQHandler
 * ����  ���жϺ��������ܵ����Ľ����ж�
 * ����  ����
 * ���  : ��
 * ����  ���������
 */	 
void CAN_RX_IRQHandler(void)
{
	/*�������ж�������*/
	CAN_Receive(CANx, CAN_FIFO0, &RxMessage);
	
	/*���ձ�־λ��1*/
	flag = 1; 
	
	/*ɸѡ����*/
	/*
	if((RxMessage.StdId==0x123) && (RxMessage.IDE==CAN_ID_STD) && (RxMessage.DLC==8) )
	{
		flag = 1; 					   //���ճɹ�  
	}
		else
	{
		flag = 0; 					   //����ʧ��
	}*/
}












