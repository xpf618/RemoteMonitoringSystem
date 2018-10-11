#include "bsp.h"

uint32_t CurrentElectric;				//�ܵ���
//uint32_t CurrentElectricA;
//uint32_t CurrentElectricB;
//uint32_t CurrentElectricC;

#define HFCONST   369//((2.592 * ATT_G * ATT_G*10000*INATTV*INATTI)/(EC*UI*VI)) 
#define P_K       0.0259//2.592*pow(10,10)	/(HFCONST *EC *pow(2,23))  0.01225 //0.965595/HFCONST//0.0025745

uint32_t Device_ID = 0;
uint8_t VoltageA = 0;        //A ���ѹ��Чֵ

//uint32_t PA_Q = 0;         // A����
float CurrentA = 0;	         //A �������Чֵ
float  TempA = 0;            //A �๦������ 

unsigned int Up_VolA = 0;
unsigned int Up_CurrentA = 0;
uint32_t Up_PA_Q = 0;
uint32_t Up_PA_Power = 0;
uint32_t Up_QA_Power = 0;

uint8_t VoltageB = 0;   //B ���ѹ��Чֵ
//uint32_t PB_Q = 0;  // B����
float CurrentB = 0;	 //B �������Чֵ
float TempB = 0; //B �๦������

unsigned int Up_VolB = 0;
unsigned int Up_CurrentB = 0;
uint32_t Up_PB_Q = 0;
uint32_t Up_PB_Power = 0;
uint32_t Up_QB_Power = 0;

uint8_t VoltageC = 0; //C ���ѹ��Чֵ
//uint32_t PC_Q = 0;  // C����
float CurrentC = 0;	  //C �������Чֵ
float  TempC = 0;     //C �๦������
unsigned int Up_VolC = 0;
unsigned int Up_CurrentC = 0;
uint32_t Up_PC_Q = 0;
uint32_t Up_PC_Power = 0;
uint32_t Up_QC_Power = 0;

uint16_t Huganqibeilv = 1;  //����������

uint32_t EST_Q = 0;         // �¼����ĺ������
uint32_t EST_Q_buff;        // ֮ǰ���ܺ������
uint8_t SetGainData;

/*
*********************************************************************************************************
*	�� �� ��: bsp_SpiDelay
*	����˵��: ʱ���ӳ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_spiDelay(uint16_t _ucData)
{
	uint32_t i;

	for (i = 0; i < 2*_ucData; i++);
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitAtt7022euHard
*	����˵��: ����att7022eu��Ӧ��GPIO
*	��    ��: ��
*	�� �� ֵ: ��
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
*	�� �� ��: bsp_ReadAtt7022
*	����˵��: ����att7022eu��Ӧ��GPIO
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint32_t bsp_ReadAtt7022(uint8_t Address)
{
	uint8_t i, bTemp;
  uint32_t dwData;

  SPI_CS_HIGH();
  SPI_CLK_LOW();
  SPI_CS_LOW();    // ����SPI����
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
        SPI_CS_HIGH();	  //�ر�SPI����
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
SPI_CS_LOW();	 //����SPI����

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
SPI_CS_HIGH();	    //��������
bsp_spiDelay(2);
}
void EMU_init(void)
{

  bsp_WriteAtt7022(0xD3,0x0000);     //rest
  bsp_spiDelay(2);
	bsp_WriteAtt7022(0xC3,0x000000);   //���У������
	bsp_WriteAtt7022(0xc9,0x00005A);   //ʹ��У��
	//Wr_Dat(0xC5,0x000000);        //�ر�ͬ��
	bsp_WriteAtt7022(0x01,0xB97E);     //����ģʽ�Ĵ���
	bsp_WriteAtt7022(0x03,0xF884);     //EMUcfg ��������	0xF884 �͹����й�
	bsp_WriteAtt7022(0x02,0x0200);   	//��������1  ��ѹ8 

	//   bsp_WriteAtt7022(0x30,0x0000); // �ر������ж�
	bsp_WriteAtt7022(0x31,0x3437);      //ģ��ģ��Ĵ���
	bsp_WriteAtt7022(0x6D, 0xFF00);     //Vrefgain �Ĳ�������ϵ��TCcoffA ��TCcoffB ��TCcoffC(0x6D~0x6F)
	bsp_WriteAtt7022(0x6E, 0x0DB8);     //Vrefgain �Ĳ�������ϵ��TCcoffA ��TCcoffB ��TCcoffC(0x6D~0x6F)
	bsp_WriteAtt7022(0x6F, 0xD1DA);     //Vrefgain �Ĳ�������ϵ��TCcoffA ��TCcoffB ��TCcoffC(0x6D~0x6F)
	bsp_WriteAtt7022(0x1E,HFCONST);     // HFconst 3200  109	
	
	bsp_WriteAtt7022(0x17,(int)(0.98*32768));	//A���ѹ����У׼
	bsp_WriteAtt7022(0x18,(int)(0.98*32768));  //B ���ѹ����У׼
	bsp_WriteAtt7022(0x19,(int)(0.98*32768));  //C ���ѹ����У׼ 
	
	bsp_WriteAtt7022(0x1A,1.918*32768); //A���������   У׼		
	bsp_WriteAtt7022(0x1B,1.918*32768); //B���������   У׼	
	bsp_WriteAtt7022(0x1C,1.908*32768); //C���������   У׼
						
	bsp_WriteAtt7022(0x04,1.442*32768); //A�й�������   У׼    
	bsp_WriteAtt7022(0x07,1.442*32768); //A�޹�������   У׼
	bsp_WriteAtt7022(0x0A,1.442*32768); //A���ڹ������� У׼

	bsp_WriteAtt7022(0x05,1.442*32768); //B�й�������   У׼	
	bsp_WriteAtt7022(0x08,1.442*32768); //B�޹�������   У׼
	bsp_WriteAtt7022(0x0B,1.442*32768); //B���ڹ������� У׼
		
	bsp_WriteAtt7022(0x06,1.43*32768);  //C�й�������   У׼
	bsp_WriteAtt7022(0x09,1.43*32768);  //C�޹�������   У׼1.378*(pow(2,15))
	bsp_WriteAtt7022(0x0C,1.43*32768);  //C���ڹ������� У׼1.378*(pow(2,15)

//	bsp_WriteAtt7022(0x24,0x036C);    //A���ѹ��Чֵ��У׼0x036C
//	bsp_WriteAtt7022(0x25,0x0301);    //  B���ѹ��Чֵ��У׼0x0301
//	bsp_WriteAtt7022(0x26,0x02E2);    //  C���ѹ��Чֵ��У׼0x02E2

	bsp_WriteAtt7022(0x35,0x080F); 	   //   I/O
	bsp_WriteAtt7022(0xC5,0x0002);			 ///��ͬ��
	bsp_WriteAtt7022(0xc9,0x000000); 	 //Close the write protection
	bsp_WriteAtt7022(0xC6,0x0000) ; 	   //�򿪼����Ĵ�������
  Device_ID = bsp_ReadAtt7022(0x00);	 // Device ID 
}
void InitAmmeter(void)
{
	bsp_InitAtt7022euHard(); //�ܽų�ʼ��
	EMU_init();    // ����ʼ��
	Device_ID = bsp_ReadAtt7022(0x00);	  // Device ID 
	bsp_spiDelay(100);
}
void ReadAmmeterData(void)
{
	uint8_t ReadPowerFlag = 0;//�Ƿ��ȡ������־  //�û��Լ�������Ҫ�����ԵĶ�ȡ��������ȡ֮��оƬ�ڲ����������û��Լ����ۼ� 
	
	uint32_t Device_ID_buff;
	uint32_t TempSSAPwoer[3];
	uint32_t VolA = 0; // A  ���ѹ
	uint32_t CurrA = 0; //A �����
	uint32_t PA_Power = 0; //A ���й���
	uint32_t QA_Power = 0; //A ���޹�����
	uint32_t SA_Power = 0; //A �����ڹ���
	float PPA_Power = 0;
	float QQA_Power = 0;
	float SSA_Power = 0;
	uint32_t VolB = 0; // B ���ѹ
	uint32_t CurrB = 0; //B �����
	uint32_t PB_Power = 0; //B ���й�����
	uint32_t QB_Power = 0; //B ���޹�����
	uint32_t SB_Power = 0; // B�����ڹ���
	float PPB_Power = 0;
	float QQB_Power = 0;
	float SSB_Power = 0;
	uint32_t VolC = 0;      // C  ���ѹ
	uint32_t CurrC = 0;     //C �����
	uint32_t PC_Power = 0;  //C ���й�����
	uint32_t QC_Power = 0;  //C ���޹�����
	uint32_t SC_Power = 0;  //C�����ڹ���
  
	float PPC_Power = 0;
	float QQC_Power = 0;
	float SSC_Power = 0;
	/*************A ��********************************/
	
		Device_ID_buff = bsp_ReadAtt7022(0x00);	  // Device ID 
	  bsp_spiDelay(100);
		if (Device_ID_buff != Device_ID)
		{
				EMU_init();    // ����ʼ��
				bsp_spiDelay(500);	
         return;
		}

	  VolA =  bsp_ReadAtt7022(0x0D);  //Vp2 A��ѹ ,
	  bsp_spiDelay(100);
	  CurrA = bsp_ReadAtt7022(0x10);   //Vp5 C��ѹ  Vp1 A��ѹ
	  bsp_spiDelay(100);
	  PA_Power = bsp_ReadAtt7022(0x01);  //A���й�����
	  bsp_spiDelay(100);  
	  QA_Power = bsp_ReadAtt7022(0x05);  //A���޹�����
	  bsp_spiDelay(100); 
	  SA_Power = bsp_ReadAtt7022(0x09);  //A�����ڹ���
	  bsp_spiDelay(100); 
//	  PA_Q = (bsp_ReadAtt7022(0x35))/3200;  //A�����ڵ���
//	  delay_us(100);
	  VoltageA = (uint8_t)(VolA/8192)	;   //��ѹ��Чֵ   								
	   CurrentA = ((float)CurrA/8192)/UI_K;   //A�������Чֵ
	//  CurrentA = (float)(CurrA/(pow(2,13)))/176;   //A�������Чֵ
//	  CurrentA =CurrentA*10;
	  bsp_spiDelay(100);
	  TempA = bsp_ReadAtt7022(0x14); //A�๦������
	  if(TempA > 8388608)
	  {
	  	TempA =(float) 	((16777216 - TempA)/8388608);
	  }
	  else 
		{
			TempA = (float) (TempA /8388608);
		}
  
	  if(PA_Power > 8388608) //         A���й�����
	  {
	     PPA_Power = ((16777216 - (PA_Power))*P_K);//	
		  
		 // PPA_Power = ((PA_Power)- ((pow(2,24))))*P_K;
	  } 
	  else 
		{
	    PPA_Power =  (PA_Power)*P_K;		   //*0.000865
		}
	   if (QA_Power > 8388608)	 //         A���޹�����
	   {
	   		QQA_Power = ((16777216 - QA_Power )*P_K);
	   }
	   else 
		 {
	   		QQA_Power = (QA_Power*P_K);
		 }

	    if (SA_Power> 8388608)	//         A�����ڹ�����
	   {
	   		SSA_Power = ((16777216 - SA_Power )*P_K);
	   }
	   else 
         {
                SSA_Power = (SA_Power*P_K);	
         }
	   	
	   Up_VolA = (unsigned int)VoltageA;          						//�ϴ���ѹ
	   Up_CurrentA = (unsigned int)(CurrentA*Huganqibeilv + 0.5);     						//�ϵ���
	   Up_PA_Power = (unsigned int)(PPA_Power*Huganqibeilv + 0.5);		       			    //�й�����
         
//      UsartSend_Bit3(Up_PA_Power,0);
//      UsartSend_Bit3((Up_PA_Power>>8),0);
//      UsartSend_Bit3(Up_PA_Power>>16,0);
//      UsartSend_Bit3((Up_PA_Power>>24),0);
	   Up_QA_Power = (unsigned int)(QQA_Power*Huganqibeilv + 0.5);			   			    //�޹����� 
         
//	   CurrentElectricA = PA_Q*Huganqibeilv;							//A�����ڵ����
	   
	   TempSSAPwoer[0] = (unsigned int)(SSA_Power*Huganqibeilv + 0.5);			    	    //���ʺ�����ڹ���
//	   UsartSend_Bit3(TempSSAPwoer[0],0);
//      UsartSend_Bit3((TempSSAPwoer[0]>>8),0);
//      UsartSend_Bit3(TempSSAPwoer[0]>>16,0);
//      UsartSend_Bit3((TempSSAPwoer[0]>>24),0);
	  /***********B��**************************/

	  VolB =  bsp_ReadAtt7022(0x0E);  //Vp4 B��ѹ ,
	  bsp_spiDelay(100);
	  CurrB = bsp_ReadAtt7022(0x11);   //Vp5 C��ѹ  Vp1 A��ѹ	  VP3 B
	  bsp_spiDelay(100);
	  PB_Power = bsp_ReadAtt7022(0x02);  //B���й�����
	  bsp_spiDelay(100);  
	  QB_Power = bsp_ReadAtt7022(0x06);  //B���޹�����
	  bsp_spiDelay(100); 
//	  PB_Q =(bsp_ReadAtt7022(0x36))/3200;   //B���й�����+PB_Q
//	  delay_us(100);
	  SB_Power = bsp_ReadAtt7022(0x0A);
	  VoltageB = (uint8_t)(VolB/8192)	;   //��ѹ��Чֵ   �д�
	  CurrentB = ((float)CurrB/8192)/UI_K;  //         B�������Чֵ
	  bsp_spiDelay(100);
	  TempB = bsp_ReadAtt7022(0x15); //A�๦������
	  if(TempB > 8388608)
	  {
	  	TempB =(float)((16777216 - TempB)/8388608);
	  }
	  else 
		{
		  TempB = (float)(TempB /8388608) ; 
		}
					
	  if(PB_Power > 8388608)    //B���й�����
	  {
	     PPB_Power = (float)((16777216- PB_Power  )*P_K);
	  } 
	  else 
		{
	      PPB_Power = (float) (PB_Power*P_K);
		}

	   if (QB_Power> 8388608)	  //B���޹�����
	   {
	   		QQB_Power = (float)((16777216 - QB_Power)*P_K);
	   }
	   else 
		 {
	   		QQB_Power = (float)(QB_Power*P_K);
		 }

	   if (SB_Power> 8388608)	 //B�����ڹ�����
	   {
	   		SSB_Power = (float)((16777216- SB_Power )*P_K);
	   }
	   else 
		 {
	   		SSB_Power = (float)(SB_Power*P_K);	 
		 }
		   
	   
	   Up_VolB = (unsigned int)VoltageB;          						//�ϴ���ѹ
	   Up_CurrentB = (unsigned int)(CurrentB*Huganqibeilv + 0.5);     						//�ϵ���
	   Up_PB_Power = (unsigned int)(PPB_Power*Huganqibeilv + 0.5);		       			    //�й�����
//      UsartSend_Bit3(Up_PB_Power,0);
//      UsartSend_Bit3((Up_PB_Power>>8),0);
//      UsartSend_Bit3(Up_PB_Power>>16,0);
//      UsartSend_Bit3((Up_PB_Power>>24),0);
	   Up_QB_Power = (unsigned int)(QQB_Power*Huganqibeilv + 0.5);			   			    //�޹����� 
//	   CurrentElectricB = PB_Q*Huganqibeilv;							//A�����ڵ����
	   
 	   TempSSAPwoer[1] = (unsigned int)(SSB_Power*Huganqibeilv + 0.5);			    	    //���ʺ�����ڹ���
//	   UsartSend_Bit3(TempSSAPwoer[1],0);
//      UsartSend_Bit3((TempSSAPwoer[1]>>8),0);
//      UsartSend_Bit3(TempSSAPwoer[1]>>16,0);
//      UsartSend_Bit3((TempSSAPwoer[1]>>24),0);
	  /********************C��*************************/

	  VolC =  bsp_ReadAtt7022(0x0F);  //Vp4 C��ѹ ,
	  bsp_spiDelay(100);
	  CurrC = bsp_ReadAtt7022(0x12);   //Vp5 C��ѹ  Vp1 A��ѹ	  VP3 B
	  bsp_spiDelay(100);
	  PC_Power = bsp_ReadAtt7022(0x03);  //C���й�����
	  bsp_spiDelay(100);  
	  QC_Power = bsp_ReadAtt7022(0x07);  //C���޹�����
	  bsp_spiDelay(100); 
//	  PC_Q = (bsp_ReadAtt7022(0x37));   //C�����ڵ���/3200+PC_Q
//	  delay_us(100);
	  SC_Power = bsp_ReadAtt7022(0x0B);		//c�����ڹ���	 
	  VoltageC = (uint8_t)(VolC/8192);   //��ѹ��Чֵ   �д�
	  CurrentC = ((float)CurrC/8192)/UI_K;  //C �������Ч
	  bsp_spiDelay(100);
	  TempC = bsp_ReadAtt7022(0x16); //A�๦������
	  if(TempC > 8388608)
	  {
	  	TempC =(float) 	((16777216 - TempC)/8388608);
	  }
	  else 
		{
			TempC = (float) (TempC /8388608) ; 

		}
		
	  if(PC_Power > 8388608)   //C���й�����
	  {
	     PPC_Power = (float)((16777216 - PC_Power )*P_K);
	  } 
	  else 
		{
	     PPC_Power = (float) (PC_Power*P_K);
		}

	   if (QC_Power> 8388608)  //C���޹�����
	   {
	   		QQC_Power = (float)((16777216 - QC_Power)*P_K);
	   }
	   else 
		 {
				QQC_Power = (float)(QC_Power*P_K);
		 }
	   		
	    if (SC_Power> 8388608)	    //C�����ڹ�����
	   {
	   		SSC_Power = (float)((16777216 - SC_Power)*P_K);
	   }
	   else 
		 {
				SSC_Power = (float)(SC_Power*P_K);
		 }
		 
		if (ReadPowerFlag)							//����֮ǰ�������Ա�����ʱ�ϱ�
		{
			 EST_Q = bsp_ReadAtt7022(0x38);
			
			 bsp_spiDelay(100);
			
			 EST_Q_buff += ((EST_Q+16)>>5)*Huganqibeilv;						//	 
			 CurrentElectric = EST_Q_buff/100;			//�ϴ�����	
		}
		 
	   Up_VolC = (unsigned int)VoltageC;          						//�ϴ���ѹ
	   Up_CurrentC = (unsigned int)(CurrentC*Huganqibeilv + 0.5);     						//�ϵ���
	   Up_PC_Power = (unsigned int)(PPC_Power*Huganqibeilv + 0.5);		       			    //�й�����
     
//	UsartSend_Bit3(Up_PC_Power,0);
//     UsartSend_Bit3((Up_PC_Power>>8),0);
//     UsartSend_Bit3(Up_PC_Power>>16,0);
//     UsartSend_Bit3((Up_PC_Power>>24),0);
	   Up_QC_Power = (unsigned int)(QQC_Power*Huganqibeilv + 0.5);			   			    //�޹����� 
//	   CurrentElectricC = PC_Q*Huganqibeilv;							//B�����ڵ����
	   
	   TempSSAPwoer[2] = (unsigned int)(SSC_Power*Huganqibeilv + 0.5);			    	    //���ʺ�����ڹ���
//     UsartSend_Bit3(TempSSAPwoer[2],0);
//     UsartSend_Bit3((TempSSAPwoer[2]>>8),0);
//     UsartSend_Bit3(TempSSAPwoer[2]>>16,0);
//     UsartSend_Bit3((TempSSAPwoer[2]>>24),1);
//	   CurrentElectric = CurrentElectricC+CurrentElectricB+CurrentElectricA; //�ܵ���
	   
	   	   
}



