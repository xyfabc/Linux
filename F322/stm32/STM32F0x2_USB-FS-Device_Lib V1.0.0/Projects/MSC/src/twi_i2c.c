#include "stm32f0xx.h"  
#include "stm32f0xx_gpio.h"
#include "twi_i2c.h"
  
//条件编译 1:使用软件模拟I2C  
#define PIN_SCL   GPIO_Pin_0  
#define PIN_SDA   GPIO_Pin_1 

  
static __inline void TWI_SCL_0(void)        { GPIOA->BRR=PIN_SCL; }  
static __inline void TWI_SCL_1(void)        { GPIOA->BSRR=PIN_SCL;}  
static __inline void TWI_SDA_0(void)        { GPIOA->BRR=PIN_SDA; }  
static __inline void TWI_SDA_1(void)        { GPIOA->BSRR=PIN_SDA;}  
static __inline u8   TWI_SDA_STATE(void)        { return (GPIOA->IDR & PIN_SDA) != 0; }  
//static __inline void TWI_NOP(void)        {  TWI_Delay(); }  
  
static const u8 TWI_ACK     =0;  
static const u8 TWI_READY   =0;  
static const u8 TWI_NACK    =1;  
static const u8 TWI_BUS_BUSY    =2;  
static const u8 TWI_BUS_ERROR   =3;  
  
static const u8  TWI_RETRY_COUNT = 3; //重试次数  
  
extern void Delay_mS(u32 n);  
  
void TWI_Initialize(void);  
u8 TWI_START(void);  
u8 TWI_START_SHT(void);  
void TWI_STOP(void);  
u8  TWI_SendByte(u8 Data);  
u8 TWI_ReceiveByte(u8 ack);  
void TWI_SendACK(u8 ack);  


  
/******************************************************************************* 
 * 函数名称:TWI_Delay                                                                      
 * 描    述:延时函数                                                                      
 *                                                                                
 * 输    入:无                                                                      
 * 输    出:无                                                                      
 * 返    回:无                                                                      
 * 作    者: eric.xi                                                                     
 * 修改日期:2016年11月10日                                                                     
 *******************************************************************************/  
static void TWI_NOP(void)  
{  
 vu32 i, j;  
 vu32 sum = 0;  
 i = 20;  
 while(i--)  
 {  
     for (j = 0; j < 10; j++)  
     sum += i;   
 }  
 sum = i;  
}  
  
/******************************************************************************* 
 * 函数名称:TWI_Initialize                                                                      
 * 描    述:I2C初始化函数                                                                      
 *                                                                                
 * 输    入:无                                                                      
 * 输    出:无                                                                      
 * 返    回:无                                                                      
 * 作    者: eric.xi                                                                     
 * 修改日期:2016年11月10日                                                                     
 *******************************************************************************/  
void TWI_Initialize()  
{  
  	GPIO_InitTypeDef GPIO_InitStructure;  
	
	RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOAEN, ENABLE);
	
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;  
  	GPIO_Init(GPIOA, &GPIO_InitStructure);  
  
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;  
  	GPIO_Init(GPIOA, &GPIO_InitStructure);  
    
  	TWI_SDA_1();  
  	TWI_SCL_0();   
    
  //////DebugPrint("Software TWI Initializing...\n");   
}  

/******************************************************************************* 
 * 函数名称:TWI_SDA_MODE                                                                      
 * 描    述:设置管脚的输入输出模式                                                                     
 *                                                                                
 * 输    入:无                                                                      
 * 输    出:无                                                                      
 * 返    回:无                                                                      
 * 作    者: eric.xi                                                                     
 * 修改日期:2016年11月10日                                                                     
 *******************************************************************************/  
static void TWI_SDA_MODE(u8 mode)  
{  
  GPIO_InitTypeDef GPIO_InitStructure;  
  if(I2C_SDA_OUTPUT == mode){
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  }else if(I2C_SDA_INPUT == mode){
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  }
  //GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
  GPIO_InitStructure.GPIO_Pin = PIN_SDA;  
  GPIO_Init(GPIOA, &GPIO_InitStructure);   
}  

static u8 TWI_START(void)  
{   
 TWI_SDA_1();       
 TWI_SCL_1();   
 TWI_NOP();      
  
 if(!TWI_SDA_STATE())  
 {  
  ////DebugPrint("TWI_START:BUSY\n");  
  return TWI_BUS_BUSY;  
 }  
 TWI_SDA_0();  
 TWI_NOP();  
    
 TWI_SCL_0();    
  
 if(TWI_SDA_STATE())  
 {  
  ////DebugPrint("TWI_START:BUS ERROR\n");  
  return TWI_BUS_ERROR;  
 }   
   
 return TWI_READY;  
} 

/* --------------------------------------------------------------------------*/  
/**  
 * @Brief:  TWI_START_SHT  
 *  
 * @Returns:    
 * 作    者: eric.xi                                                                     
 * 修改日期:2016年11月10日  
/* --------------------------------------------------------------------------*/  
static u8 TWI_START_SHT(void)  
{  
    TWI_SDA_1();  
    TWI_SCL_0();  
    TWI_NOP();  
  
    TWI_SDA_1();   
    TWI_SCL_1();   
    TWI_NOP();  
  
    if(!TWI_SDA_STATE())  
    {  
        return TWI_BUS_BUSY;  
    }  
    TWI_SDA_0();  
    TWI_NOP();  
  
    TWI_SCL_0();    
    TWI_NOP();   
  
    TWI_SCL_1();  
    TWI_NOP();  
  
    TWI_SDA_1();  
    TWI_NOP();  
  
    TWI_SCL_0();  
    TWI_NOP();  
  
    return TWI_READY;  
}  
  
  
/* --------------------------------------------------------------------------*/  
/**  
 * @Brief:  TWI_STOP  
 * 作    者: eric.xi                                                                     
 * 修改日期:2016年11月10日  
/* --------------------------------------------------------------------------*/  
static void TWI_STOP(void)  
{ 
 TWI_SCL_0();  
 TWI_NOP(); 
 
 TWI_SDA_0();   
 TWI_NOP();  
     
 TWI_SCL_1();   
 TWI_NOP();      
  
 TWI_SDA_1();  
    
 //////DebugPrint("TWI_STOP\n");     
}  
  
/* --------------------------------------------------------------------------*/  
/**  
 * @Brief:  TWI_SendACK  
 * 作    者: eric.xi                                                                     
 * 修改日期:2016年11月10日  
/* --------------------------------------------------------------------------*/  
static void TWI_SendACK(u8 ack)  
{ 
 TWI_SDA_MODE(I2C_SDA_OUTPUT);
 if(ack){
	TWI_SDA_1();
 }else{
	TWI_SDA_0();
 }

 TWI_SCL_1(); //
 TWI_NOP();
 TWI_SCL_0();  //
 TWI_NOP();    
}  
   
/* --------------------------------------------------------------------------*/  
/**  
 * @Brief:  TWI_SendByte  
 *  
 * @Param: Data 
 *  
 * @Returns:   
 * 作    者: eric.xi                                                                     
 * 修改日期:2016年11月10日  
/* --------------------------------------------------------------------------*/  
static u8 TWI_SendByte(u8 Data)  
{  
 u8 i,mask;  
 TWI_SCL_0();
 TWI_SDA_0();  
 for(i=0;i<8;i++)  
 {    
  //---------数据建立----------  
  if(Data&0x80){  
   TWI_SDA_1();  
  }else{  
   TWI_SDA_0();  
  }   
  Data<<=1;  
  TWI_NOP();  
 
  TWI_SCL_1();  
  TWI_NOP();  
  TWI_SCL_0();   
 }  
 //接收从机的应答  
 TWI_SDA_MODE(I2C_SDA_INPUT);
 TWI_NOP();

 //在高电平时读取ack
 TWI_SCL_1(); //
 TWI_NOP();
 
 mask = TWI_SDA_STATE();// read acknowledge
 TWI_NOP();

 TWI_SDA_0(); 
 TWI_NOP();
 
 TWI_SDA_MODE(I2C_SDA_OUTPUT);
 TWI_NOP(); 
 return mask;
}  
  
/* --------------------------------------------------------------------------*/  
/**  
 * @Brief:  TWI_ReceiveByte  
 *  
 * @Returns:    
 * 作    者: eric.xi                                                                     
 * 修改日期:2016年11月10日    
/* --------------------------------------------------------------------------*/  
static u8 TWI_ReceiveByte(u8 ack)  
{  
 u8 i,Dat; 
 TWI_SDA_MODE(I2C_SDA_INPUT);
 //TWI_SDA_1();  
 TWI_SCL_0();
 TWI_NOP(); 
 Dat=0;  
 for(i=0;i<8;i++)  
 {    
  Dat<<=1;  
  if(TWI_SDA_STATE()) //读引脚状态  
  {  
   Dat|=0x01;   
  } 
  TWI_SCL_1();
  TWI_NOP(); 
  TWI_SCL_0();   
  TWI_NOP();    
 } 
 TWI_SendACK(ack); 
 //////DebugPrint("TWI_Dat:%x\n",Dat);  
 return Dat;  
} 

/* --------------------------------------------------------------------------*/  
/**  
 * @Brief:  SyncCommandWrite  同步写
 *  
 * @Returns:    
 * 作    者: eric.xi                                                                     
 * 修改日期:2016年11月10日    
/* --------------------------------------------------------------------------*/ 
u8 SyncCommandWrite(
		u8 Commandword,
		u8 addr1,
		u8 addr2,
		u8 num,
		u8 *pdata)
{
	u8 i,temp;
 	temp = TWI_START();
	if(TWI_BUS_ERROR == temp){
		return -1;
	}
	temp = TWI_SendByte(Commandword);
	if(temp){
		return -1;
	}
	TWI_SendByte(addr1);
	TWI_SendByte(addr2);
	TWI_SendByte(num);
	
	for(i=0;i<num;i++) {
		TWI_SendByte(pdata[i]); 
	}
	TWI_STOP(); 
	return 0;
}

//
/* --------------------------------------------------------------------------*/  
/**  
 * @Brief:SyncCommandRead  同步读命令
 *  
 * @Returns:    
 * 作    者: eric.xi                                                                     
 * 修改日期:2016年11月10日    
/* --------------------------------------------------------------------------*/ 
u8 SyncCommandRead(
		u8 Commandword,
		u8 addr1,
		u8 addr2,
		u8 num,
		u8 *pdata)
{
	u8 i,temp;

	temp = TWI_START();
	if(TWI_BUS_ERROR == temp){
		return -1;
	}
	temp = TWI_SendByte(Commandword);
	if(temp){
		return -1;
	}
	TWI_SendByte(addr1);
	TWI_SendByte(addr2);
	TWI_SendByte(num);
	for(i=0;i<num-1;i++) {
		pdata[i] = TWI_ReceiveByte(0);	//pdata[i]=com_I2CMasterRead(0, GPx_sdata_BP, GPx_sclk_BP);
	}
	pdata[i] = TWI_ReceiveByte(1);	//pdata[i]=com_I2CMasterRead(1, GPx_sdata_BP, GPx_sclk_BP);
	TWI_STOP();					//com_I2CStop(GPx_sdata_BP, GPx_sclk_BP);
	return 0;
}