#ifndef _C210_AT88_H_
#define _C210_AT88_H_

#define COM_I2C_INIT		0
#define	COM_I2C_START		1
#define COM_I2C_STOP    2
#define COM_I2C_WRITE   3
#define COM_I2C_READ    4

#define	GPIO_INPUT		0
#define	GPIO_OUTPUT		1

#define GPx_sdata_BP	(32 * 2 + 20)		//Gpio STD
#define GPx_sclk_BP	(32 * 2 + 21)		//Gpio SLK

#define AT_SDA 	GPx_sdata_BP
#define AT_SCK 	GPx_sclk_BP

#endif
