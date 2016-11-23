#ifndef _ENCRYPT_H_
#define _ENCRYPT_H_

#ifdef __cplusplus
extern "C"{
#endif

//protocol about
//Command
#define CMD_USER_ZONE_WRITE 		0xB0
#define CMD_USER_ZONE_READ 			0xB2
#define CMD_SYSTEM_WRITE 			0xB4
#define CMD_SYSTEM_READ				0xB6
#define CMD_CRYPTO_VERIFY			0xB8
#define CMD_PASSWORD_VERIFY			0xBA

//Fabrication
#define LEN_ANSWER_TO_RESET			0x08
#define LEN_FAB_CODE				0x02
#define LEN_MTZ						0x02
#define LEN_CARD_MAN_CODE			0x04

#define ADDR_ANSWER_TO_RESET		0x00
#define ADDR_FAB_CODE				0x08
#define ADDR_MTZ					0x0A
#define ADDR_CARD_MAN_CODE			0x0C

//Security
#define LEN_LOT_HISTORY_CODE		0x08
#define ADDR_LOT_HISTORY_CODE		0x10

//Identification
#define LEN_DCR						0x01
#define LEN_IDENTI_NUM_Nc			0x07
#define LEN_AR						0x01
#define LEN_PR						0x01
#define LEN_ISSUER_CODE				0x10
#define LEN_AAC						0x01
#define LEN_CRYPTOGRAM_Ci			0x07
#define LEN_SESSION_ENCRY_KEY		0x08

#define ADDR_DCR					0x18
#define ADDR_IDENTI_NUM_Nc			0x19
#define ADDR_AR(i)					(0x20+i*2)
#define ADDR_PR(i)					(0x21+i*2)
#define ADDR_ISSUER_CODE			0x40
#define ADDR_AAC(i)					(0x50+i*0x10)
#define ADDR_CPRYTOGRAM_Ci(i)		(0x51+i*0x10)
#define ADDR_SESSION_ENCRY_KEY(i)	(0x58+i*0x10)

//Secret
#define LEN_SECRET_SEED_G			0x08
#define ADDR_SECRET_SEED_G(i)		(0x90+i*8)


//Passwords
#define LEN_PAC_W				0x01
#define LEN_WRITE				0x03
#define LEN_PAC_R				0x01
#define LEN_READ				0x03

#define ADDR_PAC_W(i)				(0xB0+i*8)
#define ADDR_WRITE(i)				(0xB1+i*8)
#define ADDR_PAC_R(i)				(0xB4+i*8)
#define ADDR_READ(i)				(0xB5+i*8)



#define ADDR_TST_MEM 				0x0A


//¼Ä´æÆ÷ÉèÖÃ
/////////////AR register/////////////////////
#define AR_PM_FREE (3<<6)	//no password is require
#define AR_PM_WRITE (1<<7)	//Write password is require
#define AR_PM_BOTH (1<<6)	//read write password is require

#define AR_AM_FREE (3<<4)	//No 
#define AR_AM_WRITE (1<<5)	//Write require
#define AR_AM_NORMAL (1<<4)	//both
#define AR_AM_DUAL 0		//used with POK

#define AR_ER_EN 0				//encrypted enable
#define AR_ER_DIS (1<<3)		//encrypted disable

#define AR_WLM_LOCK 0			//Write lock mode
#define AR_WLM_UNLOCK (1<<2)	//no lock mode

#define AR_MDF_EN 0				//Modify forbidden
#define AR_MDF_DIS (1<<1)		

#define AR_PRO_EN 0				//only can be changged from 1 to 0
#define AR_PRO_DIS 1
////////////////PR register////////////////////
#define PR_AK_OF(Gx) (Gx<<6)		//Authentication Key Select
#define PR_POK_OF(Gx) (Gx<<4)		//Program Only Key
#define PR_PW_OF(Px) (Px)			//Password set
//////////////////////////////////
unsigned char FillUserDataZone(unsigned char zone,unsigned char add,unsigned char *pointer,unsigned char length);
unsigned char GetUserDataZone(unsigned char zone,unsigned char add,unsigned char *pointer,unsigned char length);
unsigned char BurnMain(unsigned char (*ppBuffer)[4], unsigned char len[4]);
int TestEncrypt( int *len1,int* len2 );
#ifdef __cplusplus
}
#endif

#endif
