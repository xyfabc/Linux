#ifndef __TWI_I2C_H
#define __TWI_I2C_H

#define I2C_M_RD		0x0001
#define I2C_M_TEN		0x0010

#define I2C_SDA_OUTPUT		0x0000
#define I2C_SDA_INPUT		0x0001

#define u8 unsigned char 
#define u16 unsigned short int
#define u32 unsigned int 
#define vu32 unsigned int 
	


void TWI_Initialize();

u8 SyncCommandWrite(
		u8 Commandword,		//命令
		u8 addr1,			//地址1
		u8 addr2,			//地址2
		u8 num,				//要发送的数据长度
		u8 *pdata);			//要发送的数据缓冲区


u8 SyncCommandRead(
		u8 Commandword,		//命令
		u8 addr1,			//地址1
		u8 addr2,			//地址2
		u8 num,				//要接收的数据长度
		u8 *pdata);			//要接收的数据缓冲区


#endif