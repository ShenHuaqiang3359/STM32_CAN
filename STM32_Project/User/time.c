#include "stm32f4xx.h"                  // Device header
#include "./can/bsp_can.h"
extern CanTxMsg  TxMessage;

/*
 * 函数名：TIME1_Init
 * 描述  ：TIME1初始化
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
void TIME1_Init()
{ 
		/*变量定义必须放在前面*/
		NVIC_InitTypeDef NVIC_InitStructure;                 //NVIC结构体
		TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;  //时基单元结构体
	
		/*开启TIME1时钟*/
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	
		/*选择时钟源*/
		TIM_InternalClockConfig(TIM1);                 //选择TIM2的时钟源为内部时钟（默认也是内部时钟，此处显式配置更清晰）180MHZ
	
	    /*配置时基单元*/
		TIM_TimeBaseInitStructure.TIM_ClockDivision =TIM_CKD_DIV1;        //时钟分割：不分频（TIM_CKD_DIV1表示采样时钟与计数器时钟同步
		TIM_TimeBaseInitStructure.TIM_CounterMode =TIM_CounterMode_Up;   // 计数模式：向上计数（从0计数到自动重装载值，溢出后触发更新事件）
		TIM_TimeBaseInitStructure.TIM_Period = 10000-1;                 //自动重装载值：计数到9999时溢出（10000-1）
		TIM_TimeBaseInitStructure.TIM_Prescaler = 18000-1;          // 预分频器，180MHZ/((18000-1)+1)/((10000-1)+1)=1s
		TIM_TimeBaseInitStructure.TIM_RepetitionCounter =0;       // 重复计数器：仅高级定时器有效，通用定时器设置为0
		TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);
	
		/*开启TIME1中断*/
		TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
  
		/*配置NVIC*/  
		NVIC_InitStructure.NVIC_IRQChannel =TIM1_UP_TIM10_IRQn;   // 指定中断通道：TIM2中断
		NVIC_InitStructure.NVIC_IRQChannelCmd =ENABLE;   //使能中断
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;  //抢占优先级0
		NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;  //响应优先级0
		NVIC_Init(&NVIC_InitStructure);
		//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);   // 整个工程只需定义一次，已在其他地方定义过
	
		/*开启定时器1*/
		TIM_Cmd(TIM1, ENABLE);
}



/*
 * 函数名：TIM1_UP_TIM10_IRQHandler
 * 描述  ：TIME1中断函数，计数溢出进入中断              
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
/*	 TIM_IT_Update 定时器的 更新事件，当发生以下情况时会触发：
     定时器计数器（CNT）从最大值计数到 0（上溢，向上计数模式）；
     定时器计数器（CNT）从 0 计数到最大值（下溢，向下计数模式）；
     手动触发更新事件（通过软件置位 UG 位）；
     定时器初始化完成后（首次启动时）*/
void TIM1_UP_TIM10_IRQHandler()
{
	/*判断是否进入中断*/
	if(TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
	{
		CAN_Transmit(CAN1, &TxMessage);                   //周期发送报文
		printf("\r\n发送的报文为：");
		printf("\r\n ID号StdId：0x%x ",TxMessage.StdId);  //打印发送报文ID
		CAN_DEBUG_ARRAY(TxMessage.Data,8);               //打印发送报文数据
		printf("\n");
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);    //清除标志位
	}
}
