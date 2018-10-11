#include "stdio.h"
#include "stdint.h"

#define RCC_ATT7022EU 	(RCC_APB2Periph_GPIOB)

#define GPIO_PORT_SPI_CS GPIOB
#define GPIO_PIN_SPI_CS  GPIO_Pin_12

#define GPIO_PORT_SPI_DI GPIOB
#define GPIO_PIN_SPI_DI  GPIO_Pin_15

#define GPIO_PORT_SPI_DO GPIOB
#define GPIO_PIN_SPI_DO  GPIO_Pin_14

#define GPIO_PORT_SPI_CLK GPIOB
#define GPIO_PIN_SPI_CLK  GPIO_Pin_13

#define SPI_CS_HIGH() 	 GPIO_SetBits(GPIO_PORT_SPI_CS, GPIO_PIN_SPI_CS)
#define SPI_DI_HIGH()    GPIO_SetBits(GPIO_PORT_SPI_DI, GPIO_PIN_SPI_DI)
#define SPI_DO_HIGH()    GPIO_SetBits(GPIO_PORT_SPI_DO, GPIO_PIN_SPI_DO)
#define SPI_CLK_HIGH()   GPIO_SetBits(GPIO_PORT_SPI_CLK, GPIO_PIN_SPI_CLK)

#define SPI_CS_LOW() 	  GPIO_ResetBits(GPIO_PORT_SPI_CS, GPIO_PIN_SPI_CS)
#define SPI_DI_LOW()    GPIO_ResetBits(GPIO_PORT_SPI_DI, GPIO_PIN_SPI_DI)
#define SPI_DO_LOW()    GPIO_ResetBits(GPIO_PORT_SPI_DO, GPIO_PIN_SPI_DO)
#define SPI_CLK_LOW()   GPIO_ResetBits(GPIO_PORT_SPI_CLK, GPIO_PIN_SPI_CLK)

#define RDSPIData   GPIO_ReadInputDataBit(GPIO_PORT_SPI_DO,GPIO_PIN_SPI_DO)//读数据

#define ATT_G   1.163
#define INATTV 	72			      //模拟扩大8倍         
#define INATTI 	5			        //互感器感应电压 扩大1000倍  
#define EC      3200 					//电表表常数
#define UI	    220 					  //额定电压
#define VI	    0.0478
#define UI_K    19.8  // 


#define Exterl_meter_start_number 2
//#define  HFCONST ((2.592*ATT_G*ATT_G*10000*INATTV*INATTI)/(EC*UI*VI))  =76
//#define P_K   1.829E-4//1000000*UI*VI/(ATT_G*ATT_G*INATTV*INATTI*8388608)	//	  2^23 = 8388608

extern uint32_t CurrentElectric;		//总电量
extern uint32_t CurrentElectricA;   //A相电量
extern uint32_t CurrentElectricB;   //B相电压
extern uint32_t CurrentElectricC;   //C相电压

extern uint8_t SetGainData;					//设置增益数据

extern uint32_t Up_PA_Q;            //
extern uint32_t Up_PA_Power;
extern uint32_t Up_QA_Power;

extern uint32_t Up_PB_Q;
extern uint32_t Up_PB_Power;
extern uint32_t Up_QB_Power;

extern uint32_t Up_PC_Q;
extern uint32_t Up_PC_Power;
extern uint32_t Up_QC_Power;

extern uint32_t Up_CurrentA;
extern uint32_t Up_CurrentB;
extern uint32_t Up_CurrentC;

extern uint32_t Up_VolA;
extern uint32_t Up_VolB;
extern uint32_t Up_VolC;

extern uint16_t Huganqibeilv;                   //互感器倍率
extern uint8_t RS485_read_command[8];
extern uint8_t UP_Exter_data[250];					    //上报外部电量数据
extern uint8_t ReadAmmeterData_min;						  //读电表的分钟数据
extern uint32_t EST_Q_buff;                     //之前的总合相电量
extern uint8_t Exter_number_meter;					    //外接电表数量
extern uint8_t Exter_Data_Reading_flag;			    //外部电表数据读取中
extern uint8_t Exter_Data_Read_flag;			      //外部电表数据读取标志
extern uint8_t Read_Exter_meter_num;				    //外部电表标编号
extern uint8_t Read_Exter_meter_count;			    //读取外部电表计数
extern uint8_t Read_Exter_meter_Outtime_flag;	  //读取外部电表超时标志
extern uint8_t Read_Exter_meter_Outtime_count;	//读取外部电表超时次数计数

extern uint8_t first_Read_Exter_meter_flag;			//上电第一次读外部电表的标志

void bsp_InitAtt7022euHard(void);
uint32_t bsp_ReadAtt7022(uint8_t Address);
void bsp_WriteAtt7022(uint8_t address, uint32_t write_data);
void EMU_init(void);
void InitAmmeter(void);
void ReadAmmeterData(void);

void Read_external_meter(void);							//读取外部电表数据

