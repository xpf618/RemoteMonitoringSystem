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

#define RDSPIData   GPIO_ReadInputDataBit(GPIO_PORT_SPI_DO,GPIO_PIN_SPI_DO)//������

#define ATT_G   1.163
#define INATTV 	72			      //ģ������8��         
#define INATTI 	5			        //��������Ӧ��ѹ ����1000��  
#define EC      3200 					//������
#define UI	    220 					  //���ѹ
#define VI	    0.0478
#define UI_K    19.8  // 


#define Exterl_meter_start_number 2
//#define  HFCONST ((2.592*ATT_G*ATT_G*10000*INATTV*INATTI)/(EC*UI*VI))  =76
//#define P_K   1.829E-4//1000000*UI*VI/(ATT_G*ATT_G*INATTV*INATTI*8388608)	//	  2^23 = 8388608

extern uint32_t CurrentElectric;		//�ܵ���
extern uint32_t CurrentElectricA;   //A�����
extern uint32_t CurrentElectricB;   //B���ѹ
extern uint32_t CurrentElectricC;   //C���ѹ

extern uint8_t SetGainData;					//������������

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

extern uint16_t Huganqibeilv;                   //����������
extern uint8_t RS485_read_command[8];
extern uint8_t UP_Exter_data[250];					    //�ϱ��ⲿ��������
extern uint8_t ReadAmmeterData_min;						  //�����ķ�������
extern uint32_t EST_Q_buff;                     //֮ǰ���ܺ������
extern uint8_t Exter_number_meter;					    //��ӵ������
extern uint8_t Exter_Data_Reading_flag;			    //�ⲿ������ݶ�ȡ��
extern uint8_t Exter_Data_Read_flag;			      //�ⲿ������ݶ�ȡ��־
extern uint8_t Read_Exter_meter_num;				    //�ⲿ������
extern uint8_t Read_Exter_meter_count;			    //��ȡ�ⲿ������
extern uint8_t Read_Exter_meter_Outtime_flag;	  //��ȡ�ⲿ���ʱ��־
extern uint8_t Read_Exter_meter_Outtime_count;	//��ȡ�ⲿ���ʱ��������

extern uint8_t first_Read_Exter_meter_flag;			//�ϵ��һ�ζ��ⲿ���ı�־

void bsp_InitAtt7022euHard(void);
uint32_t bsp_ReadAtt7022(uint8_t Address);
void bsp_WriteAtt7022(uint8_t address, uint32_t write_data);
void EMU_init(void);
void InitAmmeter(void);
void ReadAmmeterData(void);

void Read_external_meter(void);							//��ȡ�ⲿ�������

