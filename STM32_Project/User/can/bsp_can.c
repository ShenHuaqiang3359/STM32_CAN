#include "./can/bsp_can.h"


extern __IO uint32_t flag ;		 //用于标志是否接收到数据，在中断函数中赋值
extern CanRxMsg RxMessage;		 //接收缓冲区

/*
 * 函数名：CAN_GPIO_Config
 * 描述  ：CAN的GPIO 配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：内部调用
 */
static void CAN_GPIO_Config(void)
{
	/*GPIO 初始化结构体变量*/
 	GPIO_InitTypeDef GPIO_InitStructure_TX;                     //PB9 初始化结构体变量
	GPIO_InitTypeDef GPIO_InitStructure_RX;                     //PB8 初始化结构体变量
   	 

  /* 开启GPIO时钟 */
  RCC_AHB1PeriphClockCmd(CAN_TX_GPIO_CLK|CAN_RX_GPIO_CLK, ENABLE);
	
	 /* 复用GPIO引脚为CAN */
  GPIO_PinAFConfig(CAN_TX_GPIO_PORT, CAN_RX_SOURCE, CAN_AF_PORT);   //PB9 复用为 CAN_TX
  GPIO_PinAFConfig(CAN_RX_GPIO_PORT, CAN_TX_SOURCE, CAN_AF_PORT);   //PB8 复用为 CAN_RX

	 /*  PB9 - CAN TX 引脚 */
  GPIO_InitStructure_TX.GPIO_Pin = CAN_TX_PIN;
  GPIO_InitStructure_TX.GPIO_Mode = GPIO_Mode_AF;                //复用模式      
  GPIO_InitStructure_TX.GPIO_Speed = GPIO_Speed_50MHz;           //速度
  GPIO_InitStructure_TX.GPIO_OType = GPIO_OType_PP;               //推挽输出         
  GPIO_InitStructure_TX.GPIO_PuPd  = GPIO_PuPd_UP;               //上拉电阻
  GPIO_Init(CAN_TX_GPIO_PORT, &GPIO_InitStructure_TX);
	
	/* PB8 - CAN RX 引脚 */
  GPIO_InitStructure_RX.GPIO_Pin = CAN_RX_PIN ;
  GPIO_InitStructure_RX.GPIO_Mode = GPIO_Mode_AF;                //复用模式
  GPIO_InitStructure_RX.GPIO_Speed = GPIO_Speed_50MHz;           //速度
  GPIO_InitStructure_RX.GPIO_OType = GPIO_OType_PP;               //推挽输出（也可以做输入）
  GPIO_InitStructure_RX.GPIO_PuPd  = GPIO_PuPd_UP;               //上拉电阻
  GPIO_Init(CAN_RX_GPIO_PORT, &GPIO_InitStructure_RX);
}

/*
 * 函数名：CAN_NVIC_Config
 * 描述  ：CAN的NVIC 配置,第1优先级组，0，0优先级
 * 输入  ：无
 * 输出  : 无
 * 调用  ：内部调用
 */
static void CAN_NVIC_Config(void)
{
	/*NVIC 初始化结构体*/
   	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* 中断优先级分组1 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	 /*中断设置*/
	NVIC_InitStructure.NVIC_IRQChannel = CAN_RX_IRQ;	          //CAN1 RX0中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 //抢占优先级0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			 //子优先级为0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;              //中断使能
    NVIC_Init(&NVIC_InitStructure);
}

/*
 * 函数名：CAN_Mode_Config
 * 描述  ：CAN的模式 配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：内部调用
 */
static void CAN_Mode_Config(void)
{
	/*CAN 初始化结构体*/
	CAN_InitTypeDef  CAN_InitStructure;
	
	/* 开启CAN时钟 */
	RCC_APB1PeriphClockCmd(CAN_CLK, ENABLE);

	/*CAN恢复默认*/
	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);

	/*CAN单元初始化*/
	CAN_InitStructure.CAN_TTCM=DISABLE;			   //MCR-TTCM  关闭时间触发通信模式使能
	CAN_InitStructure.CAN_ABOM=ENABLE;			   //MCR-ABOM  自动离线管理 
	CAN_InitStructure.CAN_AWUM=ENABLE;			   //MCR-AWUM  使用自动唤醒模式
	CAN_InitStructure.CAN_NART=DISABLE;			   //MCR-NART  禁止报文自动重传	  DISABLE-自动重传
	CAN_InitStructure.CAN_RFLM=DISABLE;			   //MCR-RFLM  接收FIFO 锁定模式  DISABLE-溢出时新报文会覆盖原有报文  
	CAN_InitStructure.CAN_TXFP=DISABLE;			   //MCR-TXFP  发送FIFO优先级 DISABLE-优先级取决于报文标示符 
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;  //正常工作模式
	CAN_InitStructure.CAN_SJW=CAN_SJW_2tq;		   //BTR-SJW 重新同步跳跃宽度 2个时间单元
	 
	/* ss=1 bs1=5 bs2=3 位时间宽度为(1+5+3) 波特率即为时钟周期tq*(1+3+6)  */
	CAN_InitStructure.CAN_BS1=CAN_BS1_5tq;		   //BTR-TS1 时间段1 占用了5个时间单元
	CAN_InitStructure.CAN_BS2=CAN_BS2_3tq;		   //BTR-TS1 时间段2 占用了3个时间单元	
	
	/* CAN Baudrate = 1 MBps (1MBps已为stm32的CAN最高速率) (CAN 时钟频率为 APB 1 = 45 MHz) */
	CAN_InitStructure.CAN_Prescaler =5;		   ////BTR-BRP 波特率分频器  定义了时间单元的时间长度 45/(1+5+3)/5=1 Mbps=1000kpbs
	CAN_Init(CANx, &CAN_InitStructure);
}

/*
 * 函数名：CAN_Filter_Config
 * 描述  ：CAN的过滤器 配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：内部调用
 */
static void CAN_Filter_Config(void)
{
	/*CAN 过滤器初始化结构体*/
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;

	/*CAN筛选器初始化*/
	CAN_FilterInitStructure.CAN_FilterNumber=0;						//筛选器组0
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;	//工作在掩码模式
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;	//筛选器位宽为单个32位。
	/* 使能筛选器，按照标志的内容进行比对筛选，ID不是如下的就抛弃掉，是的话，会存入FIFO0。 */

	CAN_FilterInitStructure.CAN_FilterIdHigh = (0x123 << 5) & 0xFFFF; // ID高16位（标准帧左移5位），标准帧的 11 位 ID 必须存储在 32 位过滤器的 高 16 位的低 11 位（ bit5~bit15）  ，筛选出ID为0x123的报文
    CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;    // ID低16位
    //CAN_FilterInitStructure.CAN_FilterMaskIdHigh = (0x7FF << 5) & 0xFFFF; // 掩码高16位
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x000;     //掩码高16位不过滤
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;    // 掩码低16位，不过滤
	
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0 ;				//筛选器被关联到FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;			//使能筛选器
	CAN_FilterInit(&CAN_FilterInitStructure);
	/*CAN通信中断使能*/
	CAN_ITConfig(CANx, CAN_IT_FMP0, ENABLE);
}


/*
 * 函数名：CAN_Config
 * 描述  ：完整配置CAN的功能
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
void CAN_Config(void)
{
	/*CAN GPIO初始化*/
  CAN_GPIO_Config();
	
	/*CAN 中断初始化*/
  CAN_NVIC_Config();
	
	/*CAN 模式初始化*/
  CAN_Mode_Config();
	
	/*CAN 过滤器初始化*/
  CAN_Filter_Config();   
}


/*
 * 函数名：Init_RxMes
 * 描述  ：清空接收报文
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
void Init_RxMes(CanRxMsg *RxMessage)
{
  uint8_t ubCounter = 0;

	/*把接收结构体清零*/
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
 * 函数名：CAN_SetMsg
 * 描述  ：CAN通信报文内容设置
 * 输入  ：发送报文结构体
			报文数据数组
 * 输出  : 无
 * 调用  ：外部调用
 */	 
void CAN_SetMsg(CanTxMsg *TxMessage ,uint8_t *ch)
{	  
	uint8_t ubCounter = 0;

	/*CAN 报文结构体*/
  TxMessage->StdId=0x123;				     //id
  TxMessage->ExtId=0x0000;					 
  TxMessage->IDE=CAN_ID_STD;			     //标准帧模式
  TxMessage->RTR=CAN_RTR_DATA;				 //发送的是数据
  TxMessage->DLC=8;							 //数据长度为8字节
	
	/*设置要发送的数据数组*/
	for (ubCounter = 0; ubCounter < 8; ubCounter++)
  {
    TxMessage->Data[ubCounter] = ch[ubCounter];
  }
}

/*
 * 函数名：CAN_RX_IRQHandler
 * 描述  ：中断函数，接受到报文进入中断
 * 输入  ：无
 * 输出  : 无
 * 调用  ：不需调用
 */	 
void CAN_RX_IRQHandler(void)
{
	/*从邮箱中读出报文*/
	CAN_Receive(CANx, CAN_FIFO0, &RxMessage);
	
	/*接收标志位置1*/
	flag = 1; 
	
	/*筛选报文*/
	/*
	if((RxMessage.StdId==0x123) && (RxMessage.IDE==CAN_ID_STD) && (RxMessage.DLC==8) )
	{
		flag = 1; 					   //接收成功  
	}
		else
	{
		flag = 0; 					   //接收失败
	}*/
}












