
#include "includes.h"


/*
*********************************************************************************************************
*                                       静态全局变量
*********************************************************************************************************
*/                                                          
static  OS_TCB   AppTaskStartTCB;
static  CPU_STK  AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];

static  OS_TCB   AppTaskMsgProTCB;
static  CPU_STK  AppTaskMsgProStk[APP_CFG_TASK_MsgPro_STK_SIZE];

static  OS_TCB   AppTaskCOMTCB;
static  CPU_STK  AppTaskCOMStk[APP_CFG_TASK_COM_STK_SIZE];

static  OS_TCB   AppTaskUserIFTCB;
static  CPU_STK  AppTaskUserIFStk[APP_CFG_TASK_USER_IF_STK_SIZE];

static  OS_SEM   SEM_MUTEX;	   /* 用于互斥 */

/*
*********************************************************************************************************
*                                      函数声明
*********************************************************************************************************
*/
static  void  AppTaskStart    (void  *p_arg);
static  void  AppTaskMsgPro   (void  *p_arg);
static  void  AppTaskUserIF   (void  *p_arg);
static  void  AppTaskCOM			(void  *p_arg);
static  void  AppTaskCreate   (void);
static  void  DispTaskInfo    (void);
static  void  AppObjCreate    (void);

void    App_Printf(CPU_CHAR *format, ...);

/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: 标准c程序入口。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int main(void)
{
    OS_ERR  err;  
	
	/* 初始化uC/OS-III 内核 */
    OSInit(&err);  

	/* 创建一个启动任务（也就是主任务）。启动任务会创建所有的应用程序任务 */
	OSTaskCreate((OS_TCB       *)&AppTaskStartTCB,  /* 任务控制块地址 */           
                 (CPU_CHAR     *)"App Task Start",  /* 任务名 */
                 (OS_TASK_PTR   )AppTaskStart,      /* 启动任务函数地址 */
                 (void         *)0,                 /* 传递给任务的参数 */
                 (OS_PRIO       )APP_CFG_TASK_START_PRIO, /* 任务优先级 */
                 (CPU_STK      *)&AppTaskStartStk[0],     /* 堆栈基地址 */
                 (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE / 10, /* 堆栈监测区，这里表示后10%作为监测区 */
                 (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE,  /* 堆栈空间大小 */
                 (OS_MSG_QTY    )0,  /* 本任务支持接受的最大消息数 */
                 (OS_TICK       )0,  /* 设置时间片 */
                 (void         *)0,  /* 堆栈空间大小 */  
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
		
				 /*  定义如下：
					OS_TASK_OPT_STK_CHK      使能检测任务栈，统计任务栈已用的和未用的
					OS_TASK_OPT_STK_CLR      在创建任务时，清零任务栈
					OS_TASK_OPT_SAVE_FP      如果CPU有浮点寄存器，则在任务切换时保存浮点寄存器的内容
				 */  
                 (OS_ERR       *)&err);

	/* 启动多任务系统，控制权交给uC/OS-III */
    OSStart(&err);                                               
    
    (void)&err;
    
    return (0);
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskStart
*	功能说明: 这是一个启动任务，在多任务系统启动后，必须初始化滴答计数器。本任务主要实现按键检测。
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级: 2
*********************************************************************************************************
*/
static  void  AppTaskStart (void *p_arg)
{
	OS_ERR      err;

   (void)p_arg;
	
	CPU_Init();
 	bsp_Init();
	BSP_Tick_Init();
	
#if OS_CFG_STAT_TASK_EN > 0u
     OSStatTaskCPUUsageInit(&err);   
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif
    
	/* 创建任务 */
    AppTaskCreate();

	/* 创建任务通信组件 */
	AppObjCreate();
	
	
    while (1)
	{  
		bsp_KeyScan();
		OSTimeDly(10, OS_OPT_TIME_DLY, &err);
    }
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskMsgPro
*	功能说明: 函数OSTaskQPend的使用。
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级: 3
*********************************************************************************************************
*/
static void AppTaskMsgPro(void *p_arg)
{
	OS_ERR       err;
	CPU_TS	     ts;
	OS_MSG_SIZE	 msg_size;
	void		*p_msg;
	uint8_t		 *ucReceive, i;

	(void)p_arg;
		  
	while(1)
	{	
		/* 接受数据 */
		p_msg = OSTaskQPend(0,
							OS_OPT_PEND_BLOCKING,
							&msg_size,
							&ts,
							&err);
		
		if(err == OS_ERR_NONE)
		{
			ucReceive = (uint8_t *)p_msg;
			/* 将接受到数据打印出来 */
			for(i = 0; i < msg_size; i++)
			{
				App_Printf("ucReceive = %d msg_size = %d\r\n", *ucReceive++, msg_size);
			}
		}								  	 	       											  
	}   
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCom
*	功能说明: LED闪烁
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级: 4
*********************************************************************************************************
*/
static void AppTaskCOM(void *p_arg)
{	
	OS_ERR      	err;
	(void)p_arg;
	 
	while(1)
	{
	  bsp_LedToggle(1);
	  OSTimeDly(500, OS_OPT_TIME_DLY, &err);
	} 						  	 	       											   
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskUserIF
*	功能说明: 此任务主要用于得到按键的键值并进行处理
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级: 5
*********************************************************************************************************
*/
static void AppTaskUserIF(void *p_arg)
{
	OS_ERR      err;
	uint8_t  ucKeyCode;
	uint8_t  ucMsg[4], ucTemp = 0;
	
	(void)p_arg;	              /* 避免编译器报警 */
  
	while (1) 
	{   		
//		ucKeyCode = bsp_GetKey();
//		
//		if (ucKeyCode != KEY_NONE)
//		{
//			switch (ucKeyCode)
//			{
//				case KEY_DOWN_K1:			  /* K1键按下 打印任务执行情况 */
//					DispTaskInfo();	     
//					break;

//				case KEY_DOWN_K2:			 /* K2键按下 通过函数OSTaskQPost，给任务AppTaskMsgPro发数据 */
//					ucMsg[0] = ucTemp++;
//				  ucMsg[1] = ucTemp++;
//					ucMsg[2] = ucTemp++;
//					ucMsg[3] = ucTemp++;
//					OSTaskQPost(&AppTaskMsgProTCB,
//								(void *)ucMsg,
//								 sizeof(ucMsg),
//								 OS_OPT_POST_FIFO,
//								 &err);
//					break;
//				
//				default:                    /* 其他的键值不处理 */
//					break;
//			}
//		}
		
		OSTimeDly(20, OS_OPT_TIME_DLY, &err);
	}
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCreate
*	功能说明: 创建应用任务
*	形    参：p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
*********************************************************************************************************
*/
static  void  AppTaskCreate (void)
{
	OS_ERR      err;
	
	/**************创建MsgPro任务*********************/
	OSTaskCreate((OS_TCB       *)&AppTaskMsgProTCB,             
                 (CPU_CHAR     *)"App Msp Pro",
                 (OS_TASK_PTR   )AppTaskMsgPro, 
                 (void         *)0,
                 (OS_PRIO       )APP_CFG_TASK_MsgPro_PRIO,
                 (CPU_STK      *)&AppTaskMsgProStk[0],
                 (CPU_STK_SIZE  )APP_CFG_TASK_MsgPro_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_CFG_TASK_MsgPro_STK_SIZE,
                 (OS_MSG_QTY    )5,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);

	/**************创建COM任务*********************/
	OSTaskCreate((OS_TCB       *)&AppTaskCOMTCB,            
                 (CPU_CHAR     *)"App Task COM",
                 (OS_TASK_PTR   )AppTaskCOM, 
                 (void         *)0,
                 (OS_PRIO       )APP_CFG_TASK_COM_PRIO,
                 (CPU_STK      *)&AppTaskCOMStk[0],
                 (CPU_STK_SIZE  )APP_CFG_TASK_COM_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_CFG_TASK_COM_STK_SIZE,
                 (OS_MSG_QTY    )0,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);
	
	/**************创建USER IF任务*********************/
	OSTaskCreate((OS_TCB       *)&AppTaskUserIFTCB,             
                 (CPU_CHAR     *)"App Task UserIF",
                 (OS_TASK_PTR   )AppTaskUserIF, 
                 (void         *)0,
                 (OS_PRIO       )APP_CFG_TASK_USER_IF_PRIO,
                 (CPU_STK      *)&AppTaskUserIFStk[0],
                 (CPU_STK_SIZE  )APP_CFG_TASK_USER_IF_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_CFG_TASK_USER_IF_STK_SIZE,
                 (OS_MSG_QTY    )0,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);
		 
}

/*
*********************************************************************************************************
*	函 数 名: AppObjCreate
*	功能说明: 创建任务通讯
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
*********************************************************************************************************
*/
static  void  AppObjCreate (void)
{
	OS_ERR      err;
	
	/* 
	   创建信号量数值为1的时候可以实现互斥功能，也就是只有一个资源可以使用 
	   本例程是将串口1的打印函数作为保护的资源。防止串口打印的时候被其它任务抢占
	   造成串口打印错乱。
	*/
	OSSemCreate((OS_SEM    *)&SEM_MUTEX,
				(CPU_CHAR  *)"SEM_MUTEX",
				(OS_SEM_CTR )1,
				(OS_ERR    *)&err);
}

/*
*********************************************************************************************************
*	函 数 名: App_Printf
*	功能说明: 线程安全的printf方式		  			  
*	形    参: 同printf的参数。
*             在C中，当无法列出传递函数的所有实参的类型和数目时,可以用省略号指定参数表
*	返 回 值: 无
*********************************************************************************************************
*/
void  App_Printf(CPU_CHAR *format, ...)
{
    CPU_CHAR  buf_str[80 + 1];
    va_list   v_args;
    OS_ERR    os_err;


    va_start(v_args, format);
   (void)vsnprintf((char       *)&buf_str[0],
                   (size_t      ) sizeof(buf_str),
                   (char const *) format,
                                  v_args);
    va_end(v_args);

	/* 互斥操作 */
    OSSemPend((OS_SEM  *)&SEM_MUTEX,
              (OS_TICK  )0u,
              (OS_OPT   )OS_OPT_PEND_BLOCKING,
              (CPU_TS  *)0,
              (OS_ERR  *)&os_err);

    printf("%s", buf_str);

   (void)OSSemPost((OS_SEM  *)&SEM_MUTEX,
                   (OS_OPT   )OS_OPT_POST_1,
                   (OS_ERR  *)&os_err);

}

/*
*********************************************************************************************************
*	函 数 名: DispTaskInfo
*	功能说明: 将uCOS-III任务信息通过串口打印出来
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void DispTaskInfo(void)
{
	OS_TCB      *p_tcb;	        /* 定义一个任务控制块指针, TCB = TASK CONTROL BLOCK */
	float CPU = 0.0f;
	CPU_SR_ALLOC();

	CPU_CRITICAL_ENTER();
    p_tcb = OSTaskDbgListPtr;
    CPU_CRITICAL_EXIT();
	
	/* 打印标题 */
	App_Printf("===============================================================\r\n");
	App_Printf(" 优先级 使用栈 剩余栈 百分比 利用率   任务名\r\n");
	App_Printf("  Prio   Used  Free   Per    CPU     Taskname\r\n");

	/* 遍历任务控制块列表(TCB list)，打印所有的任务的优先级和名称 */
	while (p_tcb != (OS_TCB *)0) 
	{
		CPU = (float)p_tcb->CPUUsage / 100;
		App_Printf("   %2d  %5d  %5d   %02d%%   %5.2f%%   %s\r\n", 
		p_tcb->Prio, 
		p_tcb->StkUsed, 
		p_tcb->StkFree, 
		(p_tcb->StkUsed * 100) / (p_tcb->StkUsed + p_tcb->StkFree),
		CPU,
		p_tcb->NamePtr);		
	 	
		CPU_CRITICAL_ENTER();
        p_tcb = p_tcb->DbgNextPtr;
        CPU_CRITICAL_EXIT();
	}
}


