#include "bsp.h"

uint32_t CurrentElectric;				//总电量
//uint32_t CurrentElectricA;
//uint32_t CurrentElectricB;
//uint32_t CurrentElectricC;

#define HFCONST   369//((2.592 * ATT_G * ATT_G*10000*INATTV*INATTI)/(EC*UI*VI)) 
#define P_K       0.0259//2.592*pow(10,10)	/(HFCONST *EC *pow(2,23))  0.01225 //0.965595/HFCONST//0.0025745

uint32_t Device_ID = 0;
uint8_t VoltageA = 0;        //A 相电压有效值

//uint32_t PA_Q = 0;         // A电量
float CurrentA = 0;	         //A 相电流有效值
float  TempA = 0;            //A 相功率因数 

unsigned int Up_VolA = 0;
unsigned int Up_CurrentA = 0;
uint32_t Up_PA_Q = 0;
uint32_t Up_PA_Power = 0;
uint32_t Up_QA_Power = 0;

uint8_t VoltageB = 0;   //B 相电压有效值
//uint32_t PB_Q = 0;  // B电量
float CurrentB = 0;	 //B 相电流有效值
float TempB = 0; //B 相功率因数

unsigned int Up_VolB = 0;
unsigned int Up_CurrentB = 0;
uint32_t Up_PB_Q = 0;
uint32_t Up_PB_Power = 0;
uint32_t Up_QB_Power = 0;

uint8_t VoltageC = 0; //C 相电压有效值
//uint32_t PC_Q = 0;  // C电量
float CurrentC = 0;	  //C 相电流有效值
float  TempC = 0;     //C 相功率因数
unsigned int Up_VolC = 0;
unsigned int Up_CurrentC = 0;
uint32_t Up_PC_Q = 0;
uint32_t Up_PC_Power = 0;
uint32_t Up_QC_Power = 0;

uint16_t Huganqibeilv = 1;  //互感器倍率

uint32_t EST_Q = 0;         // 新计量的合相电量
uint32_t EST_Q_buff;        // 之前的总合相电量
uint8_t SetGainData;

/*
*********************************************************************************************************
*	函 数 名: bsp_SpiDelay
*	功能说明: 时序延迟
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_spiDelay(uint16_t _ucData)
{
	uint32_t i;

	for (i = 0; i < 2*_ucData; i++);
}

/*
*********************************************************************************************************
*	函 数 名: bsp_InitAtt7022euHard
*	功能说明: 配置att7022eu对应的GPIO
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitAtt7022euHard(void)
{		
  GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_ATT7022EU, ENABLE); 
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
														   
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SPI_CS;	
	GPIO_Init(GPIO_PORT_SPI_CS, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SPI_DI;	
	GPIO_Init(GPIO_PORT_SPI_DI, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SPI_DO;	
	GPIO_Init(GPIO_PORT_SPI_DO, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SPI_CLK;	
	GPIO_Init(GPIO_PORT_SPI_CLK, &GPIO_InitStructure);
																   
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SPI_DO;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	
	GPIO_Init(GPIO_PORT_SPI_DO, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*	函 数 名: bsp_ReadAtt7022
*	功能说明: 配置att7022eu对应的GPIO
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
uint32_t bsp_ReadAtt7022(uint8_t Address)
{
	uint8_t i, bTemp;
  uint32_t dwData;

  SPI_CS_HIGH();
  SPI_CLK_LOW();
  SPI_CS_LOW();    // 开启SPI传输
  bTemp = 0x80;
  for(i=0; i<8; i++)   //Write the Address
  {
		SPI_CLK_HIGH();		
    if(Address & bTemp)
		{
			SPI_DI_HIGH();
		}
    else
    SPI_DI_LOW();
    bTemp >>= 1;
		
		bsp_spiDelay(1);
    SPI_CLK_LOW();
		bsp_spiDelay(1);
  }
  bsp_spiDelay(3);

  dwData = 0x00000000;//Read 24bit
  for(i=0; i<24; i++)
  {
		dwData <<= 1;
    SPI_CLK_HIGH();
    // Delay_us(3);
    if (RDSPIData)
    {
			dwData |= 1;
    }
    else
                        ;
		//	Delay_us(1);
    SPI_CLK_LOW();
		bsp_spiDelay(1);
        }
        SPI_CS_HIGH();	  //关闭SPI传输
        bsp_spiDelay(2);
        return dwData;
}
/**************************Wr_Dat******************************/

void bsp_WriteAtt7022(uint8_t Address, uint32_t dwData)
{
uint8_t i, bTemp;
uint32_t dwTemp;

SPI_CS_HIGH();
SPI_CLK_LOW();
SPI_CS_LOW();	 //开启SPI传输

Address |= 0x80;//write
bTemp = 0x80;
for(i=0; i<8; i++)         //Write the Address
{
SPI_CLK_HIGH();
if(Address & bTemp)
{
SPI_DI_HIGH();
}
else
SPI_DI_LOW();
bTemp >>= 1;
bsp_spiDelay(1) ;
SPI_CLK_LOW();
bsp_spiDelay(1) ;
}
bsp_spiDelay(3);

dwTemp = 0x00800000;
for(i=0; i<24; i++)  //Write Data
{
if(dwData & dwTemp)
{
SPI_DI_HIGH();
}
else
SPI_DI_LOW();
SPI_CLK_HIGH();
bsp_spiDelay(1);
SPI_CLK_LOW();
bsp_spiDelay(1);
dwTemp >>= 1;
}
SPI_CS_HIGH();	    //结束传输
bsp_spiDelay(2);
}
void EMU_init(void)
{

  bsp_WriteAtt7022(0xD3,0x0000);     //rest
  bsp_spiDelay(2);
	bsp_WriteAtt7022(0xC3,0x000000);   //清除校表数据
	bsp_WriteAtt7022(0xc9,0x00005A);   //使能校表
	//Wr_Dat(0xC5,0x000000);        //关闭同步
	bsp_WriteAtt7022(0x01,0xB97E);     //配置模式寄存器
	bsp_WriteAtt7022(0x03,0xF884);     //EMUcfg 读后清零	0xF884 和功率有关
	bsp_WriteAtt7022(0x02,0x0200);   	//电流增益1  电压8 

	//   bsp_WriteAtt7022(0x30,0x0000); // 关闭所有中断
	bsp_WriteAtt7022(0x31,0x3437);      //模拟模块寄存器
	bsp_WriteAtt7022(0x6D, 0xFF00);     //Vrefgain 的补偿曲线系数TCcoffA ，TCcoffB ，TCcoffC(0x6D~0x6F)
	bsp_WriteAtt7022(0x6E, 0x0DB8);     //Vrefgain 的补偿曲线系数TCcoffA ，TCcoffB ，TCcoffC(0x6D~0x6F)
	bsp_WriteAtt7022(0x6F, 0xD1DA);     //Vrefgain 的补偿曲线系数TCcoffA ，TCcoffB ，TCcoffC(0x6D~0x6F)
	bsp_WriteAtt7022(0x1E,HFCONST);     // HFconst 3200  109	
	
	bsp_WriteAtt7022(0x17,(int)(0.98*32768));	//A相电压增益校准
	bsp_WriteAtt7022(0x18,(int)(0.98*32768));  //B 相电压增益校准
	bsp_WriteAtt7022(0x19,(int)(0.98*32768));  //C 相电压增益校准 
	
	bsp_WriteAtt7022(0x1A,1.918*32768); //A相电流增益   校准		
	bsp_WriteAtt7022(0x1B,1.918*32768); //B相电流增益   校准	
	bsp_WriteAtt7022(0x1C,1.908*32768); //C相电流增益   校准
						
	bsp_WriteAtt7022(0x04,1.442*32768); //A有功率增益   校准    
	bsp_WriteAtt7022(0x07,1.442*32768); //A无功率增益   校准
	bsp_WriteAtt7022(0x0A,1.442*32768); //A视在功率增益 校准

	bsp_WriteAtt7022(0x05,1.442*32768); //B有功率增益   校准	
	bsp_WriteAtt7022(0x08,1.442*32768); //B无功率增益   校准
	bsp_WriteAtt7022(0x0B,1.442*32768); //B视在功率增益 校准
		
	bsp_WriteAtt7022(0x06,1.43*32768);  //C有功率增益   校准
	bsp_WriteAtt7022(0x09,1.43*32768);  //C无功率增益   校准1.378*(pow(2,15))
	bsp_WriteAtt7022(0x0C,1.43*32768);  //C视在功率增益 校准1.378*(pow(2,15)

//	bsp_WriteAtt7022(0x24,0x036C);    //A相电压有效值的校准0x036C
//	bsp_WriteAtt7022(0x25,0x0301);    //  B相电压有效值的校准0x0301
//	bsp_WriteAtt7022(0x26,0x02E2);    //  C相电压有效值的校准0x02E2

	bsp_WriteAtt7022(0x35,0x080F); 	   //   I/O
	bsp_WriteAtt7022(0xC5,0x0002);			 ///打开同步
	bsp_WriteAtt7022(0xc9,0x000000); 	 //Close the write protection
	bsp_WriteAtt7022(0xC6,0x0000) ; 	   //打开计量寄存器数据
  Device_ID = bsp_ReadAtt7022(0x00);	 // Device ID 
}
void InitAmmeter(void)
{
	bsp_InitAtt7022euHard(); //管脚初始化
	EMU_init();    // 电表初始化
	Device_ID = bsp_ReadAtt7022(0x00);	  // Device ID 
	bsp_spiDelay(100);
}
void ReadAmmeterData(void)
{
	uint8_t ReadPowerFlag = 0;//是否读取电量标志  //用户自己根据需要周期性的读取电量，读取之后芯片内部电量清理，用户自己做累计 
	
	uint32_t Device_ID_buff;
	uint32_t TempSSAPwoer[3];
	uint32_t VolA = 0; // A  相电压
	uint32_t CurrA = 0; //A 相电流
	uint32_t PA_Power = 0; //A 相有功功
	uint32_t QA_Power = 0; //A 相无功功率
	uint32_t SA_Power = 0; //A 相视在功率
	float PPA_Power = 0;
	float QQA_Power = 0;
	float SSA_Power = 0;
	uint32_t VolB = 0; // B 相电压
	uint32_t CurrB = 0; //B 相电流
	uint32_t PB_Power = 0; //B 相有功功率
	uint32_t QB_Power = 0; //B 相无功功率
	uint32_t SB_Power = 0; // B相视在功率
	float PPB_Power = 0;
	float QQB_Power = 0;
	float SSB_Power = 0;
	uint32_t VolC = 0;      // C  相电压
	uint32_t CurrC = 0;     //C 相电流
	uint32_t PC_Power = 0;  //C 相有功功率
	uint32_t QC_Power = 0;  //C 相无功功率
	uint32_t SC_Power = 0;  //C相视在功率
  
	float PPC_Power = 0;
	float QQC_Power = 0;
	float SSC_Power = 0;
	/*************A 相********************************/
	
		Device_ID_buff = bsp_ReadAtt7022(0x00);	  // Device ID 
	  bsp_spiDelay(100);
		if (Device_ID_buff != Device_ID)
		{
				EMU_init();    // 电表初始化
				bsp_spiDelay(500);	
         return;
		}

	  VolA =  bsp_ReadAtt7022(0x0D);  //Vp2 A电压 ,
	  bsp_spiDelay(100);
	  CurrA = bsp_ReadAtt7022(0x10);   //Vp5 C电压  Vp1 A电压
	  bsp_spiDelay(100);
	  PA_Power = bsp_ReadAtt7022(0x01);  //A相有功功率
	  bsp_spiDelay(100);  
	  QA_Power = bsp_ReadAtt7022(0x05);  //A相无功功率
	  bsp_spiDelay(100); 
	  SA_Power = bsp_ReadAtt7022(0x09);  //A相视在功率
	  bsp_spiDelay(100); 
//	  PA_Q = (bsp_ReadAtt7022(0x35))/3200;  //A相视在电能
//	  delay_us(100);
	  VoltageA = (uint8_t)(VolA/8192)	;   //电压有效值   								
	   CurrentA = ((float)CurrA/8192)/UI_K;   //A相电流有效值
	//  CurrentA = (float)(CurrA/(pow(2,13)))/176;   //A相电流有效值
//	  CurrentA =CurrentA*10;
	  bsp_spiDelay(100);
	  TempA = bsp_ReadAtt7022(0x14); //A相功率因数
	  if(TempA > 8388608)
	  {
	  	TempA =(float) 	((16777216 - TempA)/8388608);
	  }
	  else 
		{
			TempA = (float) (TempA /8388608);
		}
  
	  if(PA_Power > 8388608) //         A相有功功率
	  {
	     PPA_Power = ((16777216 - (PA_Power))*P_K);//	
		  
		 // PPA_Power = ((PA_Power)- ((pow(2,24))))*P_K;
	  } 
	  else 
		{
	    PPA_Power =  (PA_Power)*P_K;		   //*0.000865
		}
	   if (QA_Power > 8388608)	 //         A相无功功率
	   {
	   		QQA_Power = ((16777216 - QA_Power )*P_K);
	   }
	   else 
		 {
	   		QQA_Power = (QA_Power*P_K);
		 }

	    if (SA_Power> 8388608)	//         A相视在功功率
	   {
	   		SSA_Power = ((16777216 - SA_Power )*P_K);
	   }
	   else 
         {
                SSA_Power = (SA_Power*P_K);	
         }
	   	
	   Up_VolA = (unsigned int)VoltageA;          						//上传电压
	   Up_CurrentA = (unsigned int)(CurrentA*Huganqibeilv + 0.5);     						//上电流
	   Up_PA_Power = (unsigned int)(PPA_Power*Huganqibeilv + 0.5);		       			    //有功功率
         
//      UsartSend_Bit3(Up_PA_Power,0);
//      UsartSend_Bit3((Up_PA_Power>>8),0);
//      UsartSend_Bit3(Up_PA_Power>>16,0);
//      UsartSend_Bit3((Up_PA_Power>>24),0);
	   Up_QA_Power = (unsigned int)(QQA_Power*Huganqibeilv + 0.5);			   			    //无功功率 
         
//	   CurrentElectricA = PA_Q*Huganqibeilv;							//A相视在电电量
	   
	   TempSSAPwoer[0] = (unsigned int)(SSA_Power*Huganqibeilv + 0.5);			    	    //倍率后的视在功率
//	   UsartSend_Bit3(TempSSAPwoer[0],0);
//      UsartSend_Bit3((TempSSAPwoer[0]>>8),0);
//      UsartSend_Bit3(TempSSAPwoer[0]>>16,0);
//      UsartSend_Bit3((TempSSAPwoer[0]>>24),0);
	  /***********B相**************************/

	  VolB =  bsp_ReadAtt7022(0x0E);  //Vp4 B电压 ,
	  bsp_spiDelay(100);
	  CurrB = bsp_ReadAtt7022(0x11);   //Vp5 C电压  Vp1 A电压	  VP3 B
	  bsp_spiDelay(100);
	  PB_Power = bsp_ReadAtt7022(0x02);  //B相有功功率
	  bsp_spiDelay(100);  
	  QB_Power = bsp_ReadAtt7022(0x06);  //B相无功功率
	  bsp_spiDelay(100); 
//	  PB_Q =(bsp_ReadAtt7022(0x36))/3200;   //B相有功电能+PB_Q
//	  delay_us(100);
	  SB_Power = bsp_ReadAtt7022(0x0A);
	  VoltageB = (uint8_t)(VolB/8192)	;   //电压有效值   有待
	  CurrentB = ((float)CurrB/8192)/UI_K;  //         B相电流有效值
	  bsp_spiDelay(100);
	  TempB = bsp_ReadAtt7022(0x15); //A相功率因数
	  if(TempB > 8388608)
	  {
	  	TempB =(float)((16777216 - TempB)/8388608);
	  }
	  else 
		{
		  TempB = (float)(TempB /8388608) ; 
		}
					
	  if(PB_Power > 8388608)    //B相有功功率
	  {
	     PPB_Power = (float)((16777216- PB_Power  )*P_K);
	  } 
	  else 
		{
	      PPB_Power = (float) (PB_Power*P_K);
		}

	   if (QB_Power> 8388608)	  //B相无功功率
	   {
	   		QQB_Power = (float)((16777216 - QB_Power)*P_K);
	   }
	   else 
		 {
	   		QQB_Power = (float)(QB_Power*P_K);
		 }

	   if (SB_Power> 8388608)	 //B相视在功功率
	   {
	   		SSB_Power = (float)((16777216- SB_Power )*P_K);
	   }
	   else 
		 {
	   		SSB_Power = (float)(SB_Power*P_K);	 
		 }
		   
	   
	   Up_VolB = (unsigned int)VoltageB;          						//上传电压
	   Up_CurrentB = (unsigned int)(CurrentB*Huganqibeilv + 0.5);     						//上电流
	   Up_PB_Power = (unsigned int)(PPB_Power*Huganqibeilv + 0.5);		       			    //有功功率
//      UsartSend_Bit3(Up_PB_Power,0);
//      UsartSend_Bit3((Up_PB_Power>>8),0);
//      UsartSend_Bit3(Up_PB_Power>>16,0);
//      UsartSend_Bit3((Up_PB_Power>>24),0);
	   Up_QB_Power = (unsigned int)(QQB_Power*Huganqibeilv + 0.5);			   			    //无功功率 
//	   CurrentElectricB = PB_Q*Huganqibeilv;							//A相视在电电量
	   
 	   TempSSAPwoer[1] = (unsigned int)(SSB_Power*Huganqibeilv + 0.5);			    	    //倍率后的视在功率
//	   UsartSend_Bit3(TempSSAPwoer[1],0);
//      UsartSend_Bit3((TempSSAPwoer[1]>>8),0);
//      UsartSend_Bit3(TempSSAPwoer[1]>>16,0);
//      UsartSend_Bit3((TempSSAPwoer[1]>>24),0);
	  /********************C相*************************/

	  VolC =  bsp_ReadAtt7022(0x0F);  //Vp4 C电压 ,
	  bsp_spiDelay(100);
	  CurrC = bsp_ReadAtt7022(0x12);   //Vp5 C电压  Vp1 A电压	  VP3 B
	  bsp_spiDelay(100);
	  PC_Power = bsp_ReadAtt7022(0x03);  //C相有功功率
	  bsp_spiDelay(100);  
	  QC_Power = bsp_ReadAtt7022(0x07);  //C相无功功率
	  bsp_spiDelay(100); 
//	  PC_Q = (bsp_ReadAtt7022(0x37));   //C相视在电能/3200+PC_Q
//	  delay_us(100);
	  SC_Power = bsp_ReadAtt7022(0x0B);		//c相视在功率	 
	  VoltageC = (uint8_t)(VolC/8192);   //电压有效值   有待
	  CurrentC = ((float)CurrC/8192)/UI_K;  //C 相电流有效
	  bsp_spiDelay(100);
	  TempC = bsp_ReadAtt7022(0x16); //A相功率因数
	  if(TempC > 8388608)
	  {
	  	TempC =(float) 	((16777216 - TempC)/8388608);
	  }
	  else 
		{
			TempC = (float) (TempC /8388608) ; 

		}
		
	  if(PC_Power > 8388608)   //C相有功功率
	  {
	     PPC_Power = (float)((16777216 - PC_Power )*P_K);
	  } 
	  else 
		{
	     PPC_Power = (float) (PC_Power*P_K);
		}

	   if (QC_Power> 8388608)  //C相无功功率
	   {
	   		QQC_Power = (float)((16777216 - QC_Power)*P_K);
	   }
	   else 
		 {
				QQC_Power = (float)(QC_Power*P_K);
		 }
	   		
	    if (SC_Power> 8388608)	    //C相视在功功率
	   {
	   		SSC_Power = (float)((16777216 - SC_Power)*P_K);
	   }
	   else 
		 {
				SSC_Power = (float)(SC_Power*P_K);
		 }
		 
		if (ReadPowerFlag)							//整点之前读电量以便整点时上报
		{
			 EST_Q = bsp_ReadAtt7022(0x38);
			
			 bsp_spiDelay(100);
			
			 EST_Q_buff += ((EST_Q+16)>>5)*Huganqibeilv;						//	 
			 CurrentElectric = EST_Q_buff/100;			//上传电量	
		}
		 
	   Up_VolC = (unsigned int)VoltageC;          						//上传电压
	   Up_CurrentC = (unsigned int)(CurrentC*Huganqibeilv + 0.5);     						//上电流
	   Up_PC_Power = (unsigned int)(PPC_Power*Huganqibeilv + 0.5);		       			    //有功功率
     
//	UsartSend_Bit3(Up_PC_Power,0);
//     UsartSend_Bit3((Up_PC_Power>>8),0);
//     UsartSend_Bit3(Up_PC_Power>>16,0);
//     UsartSend_Bit3((Up_PC_Power>>24),0);
	   Up_QC_Power = (unsigned int)(QQC_Power*Huganqibeilv + 0.5);			   			    //无功功率 
//	   CurrentElectricC = PC_Q*Huganqibeilv;							//B相视在电电量
	   
	   TempSSAPwoer[2] = (unsigned int)(SSC_Power*Huganqibeilv + 0.5);			    	    //倍率后的视在功率
//     UsartSend_Bit3(TempSSAPwoer[2],0);
//     UsartSend_Bit3((TempSSAPwoer[2]>>8),0);
//     UsartSend_Bit3(TempSSAPwoer[2]>>16,0);
//     UsartSend_Bit3((TempSSAPwoer[2]>>24),1);
//	   CurrentElectric = CurrentElectricC+CurrentElectricB+CurrentElectricA; //总电能
	   
	   	   
}



