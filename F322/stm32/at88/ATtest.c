#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <unistd.h>
#include <linux/fb.h>
#include <asm/mman.h>
#include <sys/mman.h>
#include <string.h>
#include <ctype.h>
#include <termios.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>

#include "AT88.h"
#include "md5.h"
#include "encrypt.h"
#define AT88_DEVICE   "/dev/AT88SC"

int at88_fd = -1;

//#define JYZ 1
#ifdef JYZ
#define JYZ_Printf(msg...) printf(msg);
#else
#define JYZ_Printf(msg...)
#endif

///////////////////初始化参数////////////////////////////////////////////
///Identification Number Nc Addr:0x19
static unsigned char myNC[7]={ 0x60,0x61,0x62,0x63,0x64,0x65,0x66};
//Cryptogram Ci Addr:0x51,0x61,0x71,0x81
static unsigned char CRYPTOG_myRAM[4][7]={
	{0x21,0x22,0x23,0x24,0x25,0x26,0x27},
	{0x22,0x22,0x23,0x24,0x25,0x26,0x27},
	{0x23,0x22,0x23,0x24,0x25,0x26,0x27},
	{0x24,0x22,0x23,0x24,0x25,0x26,0x27},
};
//初始验证码
static unsigned char SupperKey[8]={'W','H','X','R','B','S','J','Z'};
//校验密码
static unsigned char CONFIG_ZONE_PWD[3]={ 0xdd,0x42,0x97};
//4个用户区域读写密码
static unsigned char USER_DATAZONE_WPWD[4][3] ={{0x33,0x55,0x78},{ 0x34,0x55,0x78},{ 0x35,0x55,0x78},{ 0x36,0x55,0x78}};
static unsigned char USER_DATAZONE_RPWD[4][3] ={{0x22,0x66,0x97},{ 0x23,0x66,0x97},{ 0x24,0x66,0x97},{ 0x25,0x66,0x97}};
//Card Manufacturer Code Addr:0x0C
static unsigned char CMC[4] = {'H','W','R','X'};
//Issuer Code Addr:0x40
static unsigned char IC[16]={'H','W','K','J','R','X','S','B','J','I','N','Y','O','U','Z','H'};







volatile unsigned char encrypted_en = 0;  //加密是否成功
volatile unsigned char authorized_en = 0;	//验证是否成功

////////////////////////For GPA Function///////////////////////////////////////////
static unsigned char DATAIN;
static unsigned char R_STATA=0;
static unsigned char R_STATB=0;
static unsigned char R_STATC=0;
static unsigned char R_STATD=0;
static unsigned char R_STATE=0;
static unsigned char R_STATF=0;
static unsigned char R_STATG=0;
static unsigned char S_STATA=0;
static unsigned char S_STATB=0;
static unsigned char S_STATC=0;
static unsigned char S_STATD=0;
static unsigned char S_STATE=0;
static unsigned char S_STATF=0;
static unsigned char S_STATG=0;
static unsigned char T_STATA=0;
static unsigned char T_STATB=0;
static unsigned char T_STATC=0;
static unsigned char T_STATD=0;
static unsigned char T_STATE=0;
static unsigned char GPAOUT=0;
///////////////////////Interface Define////////////////////////////////////////////////

extern unsigned int SCL;
extern unsigned int SDA;
static void PreSendFirstCmd(void);//第一个命令发送出去之前5个时钟脉冲
static void CommandWriteWithRty(unsigned char Commandword,unsigned char addr1,unsigned char addr2,unsigned char num,unsigned char *pdata);
static void CommandReadWithRty(unsigned char Commandword,unsigned char addr1,unsigned char addr2,unsigned char num,unsigned char *pdata);
static unsigned char SyncCommandWrite(
	unsigned char Commandword,
	unsigned char addr1,
	unsigned char addr2,
	unsigned char num,
	unsigned char *pdata
	);
static unsigned char SyncCommandRead(
	unsigned char Commandword,
	unsigned char addr1,
	unsigned char addr2,
	unsigned char num,
	unsigned char *pdata
	);


static unsigned char Authentication_Standard(unsigned char zone,unsigned char *secretKey,unsigned char *sessionkey);
static void ChipFunGenCH(unsigned char* gc,unsigned char *ci,unsigned char *q0,unsigned char *ch);
static void ChipFunGenQ2(unsigned char *q2,unsigned char *sessionkey);
static void GenRandomnumber(unsigned char *prd);
static void SetUserDataZone_Standard(unsigned char zone);
static void SetUserDataZone_Encrypt(unsigned char zone);
static void ReadUserDataZone_Standard(unsigned char zone,unsigned char add, unsigned char* rdata, unsigned char num);
static void ReadUserDataZone_Encrypt(unsigned char zone,unsigned char add, unsigned char* rdata, unsigned char num);
static void WriteUserDataZone_Standard(unsigned char zone,unsigned char add, unsigned char* wdata, unsigned char num);
static void WriteUserDataZone_Encrypt(unsigned char zone,unsigned char add, unsigned char* wdata, unsigned char num);

static unsigned char Encryption_zone(unsigned char zone,unsigned char *sessionkey,unsigned char *sessionkey2);

static void GenerateGC(unsigned char *pNc,unsigned char *sk,unsigned char *prt);
static void gpa_fun();

static unsigned char ReadFuse();
static void WriteFuse();
static void WriteconfigVerify();
static unsigned char ConfigUnlock(unsigned char zone);
static void InitialAT88SC0104();



//同步写
static unsigned char SyncCommandWrite(
		unsigned char Commandword,
		unsigned char addr1,
		unsigned char addr2,
		unsigned char num,
		unsigned char *pdata)
{
	unsigned char i,temp;
	//	Attention();
	ioctl(at88_fd,COM_I2C_START,0); 	//com_I2CStart(GPx_sdata_BP, GPx_sclk_BP);
	temp = ioctl(at88_fd,COM_I2C_WRITE,Commandword);  //temp=com_I2CMasterWrite(Commandword, GPx_sdata_BP, GPx_sclk_BP);   
	if(temp) 
	{			
		//no ack
		//		JYZ_Printf("write failed  %x,%x,%x,%x,%x\n",Commandword,addr1,addr2,num,pdata[0]);
		return temp;
	}

	ioctl(at88_fd,COM_I2C_WRITE,addr1);	//com_I2CMasterWrite(addr1, GPx_sdata_BP, GPx_sclk_BP);
	ioctl(at88_fd,COM_I2C_WRITE,addr2);	//com_I2CMasterWrite(addr2, GPx_sdata_BP, GPx_sclk_BP);
	ioctl(at88_fd,COM_I2C_WRITE,(num));		//com_I2CMasterWrite(num, GPx_sdata_BP, GPx_sclk_BP);
	for(i=0;i<num;i++) {
		ioctl(at88_fd,COM_I2C_WRITE,pdata[i]);	//com_I2CMasterWrite(pdata[i], GPx_sdata_BP, GPx_sclk_BP);
	}
	ioctl(at88_fd,COM_I2C_STOP,0); 					//com_I2CStop(GPx_sdata_BP, GPx_sclk_BP);
	return temp;

}
//同步读命令
//----------------------------------------------------------------
static unsigned char SyncCommandRead(
		unsigned char Commandword,
		unsigned char addr1,
		unsigned char addr2,
		unsigned char num,
		unsigned char *pdata)
{
	unsigned char i,temp;

	ioctl(at88_fd,COM_I2C_START,0); 	//com_I2CStart(GPx_sdata_BP, GPx_sclk_BP);
	temp = ioctl(at88_fd,COM_I2C_WRITE,Commandword);  //temp=com_I2CMasterWrite(Commandword, GPx_sdata_BP, GPx_sclk_BP);
	if(temp) 
	{		//no ack
		JYZ_Printf("read failed%x,%x,%x,%x\n",Commandword,addr1,addr2,num);
		return temp;
	}
	ioctl(at88_fd,COM_I2C_WRITE,addr1);	//com_I2CMasterWrite(addr1, GPx_sdata_BP, GPx_sclk_BP);
	ioctl(at88_fd,COM_I2C_WRITE,addr2);	//I2CMasterWrite(addr2, GPx_sdata_BP, GPx_sclk_BP);
	ioctl(at88_fd,COM_I2C_WRITE,num);		//I2CMasterWrite(num, GPx_sdata_BP, GPx_sclk_BP);
	for(i=0;i<num-1;i++) {
		pdata[i]=ioctl(at88_fd,COM_I2C_READ,0);	//pdata[i]=com_I2CMasterRead(0, GPx_sdata_BP, GPx_sclk_BP);
	}
	pdata[i]=ioctl(at88_fd,COM_I2C_READ,1);	//pdata[i]=com_I2CMasterRead(1, GPx_sdata_BP, GPx_sclk_BP);
	ioctl(at88_fd,COM_I2C_STOP,0); 					//com_I2CStop(GPx_sdata_BP, GPx_sclk_BP);
	return temp;
}


//================读熔丝值==================
static unsigned char ReadFuse()
{
	unsigned char temp;
	CommandReadWithRty(CMD_SYSTEM_READ,0x01,0,1,&temp  );//固定格式不可修改
	return temp;
}
//================固化熔丝=======================
static void WriteFuse()
{//fcp
	unsigned char temp;
	//=========================固定格式不可修改======================
	CommandWriteWithRty(CMD_SYSTEM_WRITE,0x01,0x06,0,&temp  );
	CommandWriteWithRty(CMD_SYSTEM_WRITE,0x01,0x04,0,&temp  );
	CommandWriteWithRty(CMD_SYSTEM_WRITE,0x01,0x0,0,&temp  );	
	//=================================================================
}
//======================测试密码 写配置区域之前需要验证密码========================
static void WriteconfigVerify()
{
	//write password 7
	CommandWriteWithRty(CMD_PASSWORD_VERIFY,0x07,0x0,3,CONFIG_ZONE_PWD);
}
///////////////////////////////初始化AT88SC0104////////////////////////////////////////////////////////////
static void InitialAT88SC0104()
{
	unsigned char temp;
	unsigned char gc[8];
	int i;
	CommandWriteWithRty(CMD_SYSTEM_WRITE,0x0,ADDR_CARD_MAN_CODE,LEN_CARD_MAN_CODE,CMC);
	CommandWriteWithRty(CMD_SYSTEM_WRITE,0x0,ADDR_IDENTI_NUM_Nc,LEN_IDENTI_NUM_Nc,myNC);
	CommandWriteWithRty(CMD_SYSTEM_WRITE,0x0,ADDR_ISSUER_CODE,LEN_ISSUER_CODE,IC);
	for(i = 0 ; i<1; i ++)
	{
		temp = AR_AM_NORMAL|AR_PM_FREE|AR_ER_DIS|AR_WLM_UNLOCK|AR_MDF_DIS|AR_PRO_DIS;
		CommandWriteWithRty(CMD_SYSTEM_WRITE,0x0,ADDR_AR(i),LEN_AR,&temp);//
		temp = PR_AK_OF(i)|PR_POK_OF(i)|PR_PW_OF(i);
		CommandWriteWithRty(CMD_SYSTEM_WRITE,0x0,ADDR_PR(i),LEN_PR,&temp);//
	}
	//dcr 设备设置寄存器
	temp=0x0f;
	CommandWriteWithRty(CMD_SYSTEM_WRITE,0x0,ADDR_DCR,LEN_DCR,&temp);
	//尝试的次数都设置为0xff
	temp=0xff;
	for(i = 0 ; i<1; i++)
	{
		CommandWriteWithRty(CMD_SYSTEM_WRITE,0x0,ADDR_PAC_W(i),LEN_PAC_W,&temp);
		CommandWriteWithRty(CMD_SYSTEM_WRITE,0x0,ADDR_PAC_R(i),LEN_PAC_R,&temp);
	}
	//aac认证的次数
	temp=0xff;
	for(i = 0 ; i<1; i++)
	{
		CommandWriteWithRty(CMD_SYSTEM_WRITE,0x0,ADDR_AAC(i),LEN_AAC,&temp);
	}
	GenerateGC(myNC,SupperKey,gc);
	//ci 加密需要的随机数  set 1
	for(i = 0 ; i <1; i++)
	{
		CommandWriteWithRty(CMD_SYSTEM_WRITE,0x0,ADDR_CPRYTOGRAM_Ci(i),LEN_CRYPTOGRAM_Ci,CRYPTOG_myRAM[i]);	
		CommandWriteWithRty(CMD_SYSTEM_WRITE,0x0,ADDR_SECRET_SEED_G(i),LEN_SECRET_SEED_G,gc);
		CommandWriteWithRty(CMD_SYSTEM_WRITE,0x0,ADDR_WRITE(i),	LEN_WRITE,USER_DATAZONE_WPWD[i]);
		CommandWriteWithRty(CMD_SYSTEM_WRITE,0x0,ADDR_READ(i),LEN_READ,USER_DATAZONE_RPWD[i]);	
	}
	////////////////////////////////////////////////////////////////////////////////////////
}




static void repeat_clock(unsigned char repeatnum, unsigned char data)
{	
	int i;
	for(i = 0; i<repeatnum;i++)
	{
		DATAIN = data;
		gpa_fun();
	}
}
//=====================================================================
static void VerifyUserZone(unsigned char zone,unsigned char wr_)
{	
	unsigned char*password;
	unsigned char temp;
	if(wr_)
		temp = zone;
	else
		temp = zone+0x10;
	password=(unsigned char*)USER_DATAZONE_WPWD[zone];
	if(authorized_en)
	{
		int i;
		for(i=0;i<3;i++)
		{
			repeat_clock(5,password[i]);
			password[i] = GPAOUT;
		}
	}
	CommandWriteWithRty(CMD_PASSWORD_VERIFY,temp,0,0x3,password);
}
//=====================================================================
static void ReadCheckSum(unsigned char * ck)
{
	//////////////check sum/////////////////
	repeat_clock(10,0);
	ck[0]= GPAOUT;
	repeat_clock(5,0);
	ck[1]=GPAOUT;
	repeat_clock(5,0x00);
}
//=====================================================================
static void SendCheckSum(unsigned char * ck)
{
	//////////////check sum/////////////////
	repeat_clock(15,0);
	ck[0]= GPAOUT;
	repeat_clock(5,0);
	ck[1]=GPAOUT;
	CommandWriteWithRty(CMD_USER_ZONE_WRITE,0x02,0,0x2,ck);
}
//-------------加密写数据---------------------------------------------------
static void WriteUserDataZone_Encrypt(unsigned char zone,unsigned char add, unsigned char* wdata, unsigned char num)
{
	int i;
	unsigned char ck[2];
	unsigned char temp;
	SetUserDataZone_Encrypt(zone);

	repeat_clock(5,0);
	repeat_clock(1,add);
	repeat_clock(5,0);
	repeat_clock(1,num);	
	for(i=0;i<num;i++)
	{
		repeat_clock(5,0);
		temp = wdata[i];
		if(encrypted_en)
			wdata[i] = wdata[i] ^ GPAOUT;  //c[j]=p[j] xor GPA_byte
		repeat_clock(1,temp);
	}
	CommandWriteWithRty(CMD_USER_ZONE_WRITE,0x0,add,num,wdata);
	if(authorized_en)
		SendCheckSum(ck);
	
}
//------------非加密写数据-------------------------------
static void WriteUserDataZone_Standard(unsigned char zone,unsigned char add, unsigned char* wdata, unsigned char num)
{
	SetUserDataZone_Standard(zone);	
	CommandWriteWithRty(CMD_USER_ZONE_WRITE,0x0,add,num,wdata);
}
//----------加密读数据------------------
static void ReadUserDataZone_Encrypt(unsigned char zone,unsigned char add, unsigned char* rdata, unsigned char num)
{
	int i;	
	
	SetUserDataZone_Encrypt(zone);	
	CommandReadWithRty(CMD_USER_ZONE_READ,0x0,add,num,rdata);
	repeat_clock(5,0);
	repeat_clock(1,add);
	repeat_clock(5,0);
	repeat_clock(1,num);
	if(encrypted_en)
	for(i=0;i<num;i++)
	{
		if(encrypted_en)
			rdata[i] = rdata[i]^GPAOUT;
		repeat_clock(1,rdata[i]);
		repeat_clock(5,0);
	}	
}

//===============非加密读数据=================
static void ReadUserDataZone_Standard(unsigned char zone,unsigned char add, unsigned char* rdata, unsigned char num)
{	
	SetUserDataZone_Standard(zone);	
	CommandReadWithRty(CMD_USER_ZONE_READ,0x0,add,num,rdata);
}
//================加密读配置区================
void ReadConfigurationZone_Encrypt(unsigned char add,unsigned char num,unsigned char* buffer)
{
	
	int i;

	repeat_clock(5,0);
	repeat_clock(1,add);
	repeat_clock(5,0x0);
	repeat_clock(1,num);
	CommandReadWithRty(CMD_SYSTEM_READ,0x0,add,num,buffer);
	for(i = 0; i<num; i++)
	{
		if(encrypted_en&&(add+i)>=0xB0&&(add+i)<=0xE8)
			buffer[i] = buffer[i] ^ GPAOUT;
		repeat_clock(1,buffer[i]);
		repeat_clock(5,0);
	}
}
//===============加密写配置区======================
static void WriteConfigurationZone_Encrypt(unsigned char add,unsigned char num,unsigned char* buffer,unsigned char encrypt_en)
{
	int i;
	repeat_clock(5,0);
	repeat_clock(1,0x19);
	repeat_clock(5,0x0);
	repeat_clock(1,7);
	CommandReadWithRty(CMD_SYSTEM_READ,0x0,add,num,buffer);
	for(i = 0; i<num; i++)
	{
		if(encrypt_en)
			buffer[i] = buffer[i] ^ GPAOUT;
		repeat_clock(1,buffer[i]);
	}
}

//----------------------------------------------------------------
static void CommandWriteWithRty(
	unsigned char Commandword,
	unsigned char addr1,
	unsigned char addr2,
	unsigned char num,
	unsigned char *pdata)
{
	
	unsigned int i;
	while(1) {
		for(i=0;i<1024*1024;i++);
		if(SyncCommandWrite(Commandword,addr1,addr2,num,pdata)==0)
			break;

		JYZ_Printf("CommandWriteWithRty retry\n");

	}
}

//----------------------------------------------------------------
static void CommandReadWithRty(
	unsigned char Commandword,
	unsigned char addr1,
	unsigned char addr2,
	unsigned char num,
	unsigned char *pdata)
{
	unsigned char temp;
	unsigned int i;
	while(1) {
		for(i=0;i<1024*1024;i++);
		temp=SyncCommandRead(Commandword,addr1,addr2,num,pdata);
		if(temp==0) {
			
			break;
		}
		JYZ_Printf("SyncCommandRead retry\n");
	}
}

static void gpa_fun()
{
	unsigned char datain_temp;
	unsigned char r_sum,t_sum,s_sum;
	unsigned char r_temp,s_temp;
	unsigned char ri,si,ti;
	volatile unsigned char i;
	unsigned char gpai=0;
	datain_temp=DATAIN ^ GPAOUT;
	ri=datain_temp&0x1f;
	si=datain_temp;
	si=(si<<3)&0x78;
	si+=((datain_temp>>5)&0x07);
	ti=(datain_temp>>3)&0x1f;
	///////////////////
	r_temp=R_STATG;
	r_temp=(r_temp<<1)&0x1e;
	r_temp+=((R_STATG>>4)&0x1);
	if((R_STATD+r_temp)>31)
	{
		r_sum=R_STATD+r_temp-31;
	}
	else
	{
		r_sum=R_STATD+r_temp;	
	}
	R_STATG=R_STATF;
	R_STATF=R_STATE;
	R_STATE=R_STATD;
	R_STATD=R_STATC ^ ri;
	R_STATC=R_STATB;
	R_STATB=R_STATA;
	R_STATA=r_sum;
	///////////////////////////////
	s_temp=S_STATG;
	s_temp=(s_temp<<1)&0x7e;
	s_temp+=((S_STATG>>6)&0x1);
	if((S_STATF+s_temp)>127)
	{
		s_sum=S_STATF+s_temp-127;
	}
	else
	{
		s_sum=S_STATF+s_temp;
	}
	S_STATG=S_STATF;
	S_STATF=S_STATE^si;
	S_STATE=S_STATD;
	S_STATD=S_STATC;
	S_STATC=S_STATB;
	S_STATB=S_STATA;
	S_STATA=s_sum;
	//////////////////
	if((T_STATE+T_STATC)>31)
	{
		t_sum=T_STATE+T_STATC-31;
	}
	else
	{
		t_sum=T_STATE+T_STATC;
	}
	T_STATE=T_STATD;
	T_STATD=T_STATC;
	T_STATC=T_STATB^ti;
	T_STATB=T_STATA;
	T_STATA=t_sum;
	//////////////
	for(i=0;i<4;i++)
	{
		if(((S_STATA>>i)&0x1)==0)
		{
			gpai |= ((R_STATA ^ R_STATE)&(1<<i));
		}
		else
		{
			gpai |= ((T_STATA ^ T_STATD)&(1<<i));
		}
	}
	GPAOUT=((GPAOUT<<4)&0xf0)+(gpai&0xf);
}

static void GenerateGC(unsigned char *pNc,unsigned char *sk,unsigned char *prt)
//use myself fun to get gc
//gc=f(nc,sk)
{
	unsigned char hash[16];
	MD5_CTX contex;
	MD5Init(&contex);
	MD5Update(&contex,pNc,7);
	MD5Update(&contex,sk,8);
	MD5Final(hash,&contex);
	memcpy(prt,hash,8);
}

//----------------------------------------------------------------
static void ChipFunGenCH(unsigned char* gc,unsigned char *ci,unsigned char *q0,unsigned char *ch)
{
	unsigned char i,k;
	//reset
	R_STATA=0;
	R_STATB=0;
	R_STATC=0;
	R_STATD=0;
	R_STATE=0;
	R_STATF=0;
	R_STATG=0;
	          
	S_STATA=0;
	S_STATB=0;
	S_STATC=0;
	S_STATD=0;
	S_STATE=0;
	S_STATF=0;
	S_STATG=0;
	    
	T_STATA=0;
	T_STATB=0;
	T_STATC=0;
	T_STATD=0;
	T_STATE=0;
	GPAOUT=0;
	//
	for(i=0;i<4;i++)
	{
		for(k=0;k<3;k++) {
			DATAIN=ci[2*i];
			gpa_fun();
		}
		for(k=0;k<3;k++) {
			DATAIN=ci[2*i+1];
			gpa_fun();
		}
		for(k=0;k<1;k++) {
			DATAIN=q0[i];
			gpa_fun();
		}
	}
	
	for(i=0;i<4;i++)
	{
		for(k=0;k<3;k++) {
			DATAIN=gc[2*i];
			gpa_fun();
		}
		for(k=0;k<3;k++) {
			DATAIN=gc[2*i+1];
			gpa_fun();
		}
		for(k=0;k<1;k++) {
			DATAIN=q0[i+4];
			gpa_fun();
		}
	}
	
	for(i=0;i<6;i++) {
		DATAIN=0;
		gpa_fun();
		
	}
	ch[0]=GPAOUT;				//将送给chip
	for(i=1;i<8;i++) {
		for(k=0 ;k<7;k++) {
			DATAIN=0;
			gpa_fun();
		}
		ch[i]=GPAOUT;
	}	
}
//----------------------Q2生成------------------------------------------
static void ChipFunGenQ2(unsigned char *q2,unsigned char *sessionkey)
{
	unsigned char i,k;
	
	q2[0]=0xff;
	for(i=1;i<8;i++) {
		for(k=0;k<2;k++) {
			DATAIN=0;
			gpa_fun();
		}
		q2[i]=GPAOUT;
	}
	for(i=0;i<8;i++) {
		for(k=0;k<2;k++) {
			DATAIN=0;
			gpa_fun();
		}
				
		sessionkey[i]=GPAOUT;
	}
	for(i=0;i<3;i++) {
		DATAIN=0;
		gpa_fun();
	}
}

//-------------------随机数生成---------------------------------------------
static void GenRandomnumber(unsigned char *prd)
{
	unsigned int *ptemp;
	ptemp=(unsigned int *)prd;
	ptemp[0]=rand();
	ptemp[1]=rand();
}
//================加密切换用户区域=========================
static void SetUserDataZone_Encrypt(unsigned char zone)
{
	unsigned char temp;
	repeat_clock(1,zone);
	CommandWriteWithRty(CMD_SYSTEM_WRITE,0x03,zone,0,&temp);		
}
//================非加密切换用户区域=======================
static void SetUserDataZone_Standard(unsigned char zone)
{
	unsigned char temp;
	CommandWriteWithRty(CMD_SYSTEM_WRITE,0x03,zone,0,&temp);		
}

//----------------------------不带加密验证------------------------------------
static unsigned char  Authentication_Standard(unsigned char zone,unsigned char *secretKey,unsigned char *sessionkey)
{
	unsigned char nc[7];
	unsigned char ci[8];		//包含前一个byte aac
	//unsigned char sk[8];
	unsigned char gc[8];
	unsigned char rand[8];
	unsigned char ch[8];
	unsigned char q2[8];
	unsigned char vfydata[16];
	unsigned char newci[8];
	
	//get nc ci
	CommandReadWithRty(CMD_SYSTEM_READ,0x0,ADDR_IDENTI_NUM_Nc,LEN_IDENTI_NUM_Nc,nc);	
	CommandReadWithRty(CMD_SYSTEM_READ,0x0,ADDR_AAC(zone),(LEN_AAC+LEN_CRYPTOGRAM_Ci),ci);	
	
	GenerateGC(nc,secretKey,gc);
	
	//gen q0 random data
	GenRandomnumber(rand);
	//gen q1=f2(gc,ci,q0)  is ch
	ChipFunGenCH(gc,ci,rand,ch);
	//q2=f2(gc q1)
	ChipFunGenQ2(q2,sessionkey);
	//send auth command
	memcpy(vfydata,rand,8);
	memcpy(vfydata+8,ch,8);
	CommandWriteWithRty(CMD_CRYPTO_VERIFY,0x0+zone,0x0,0x10,vfydata);
	//read new ci
	CommandReadWithRty(CMD_SYSTEM_READ,0x0,ADDR_AAC(zone),(LEN_AAC+LEN_CRYPTOGRAM_Ci),newci);
	
	//compare ci q2
	if(memcmp(newci,q2,8)==0)
	{
		
		JYZ_Printf("encrypt ok!\n");
		
		return 0;
	}
	else
	{
		
		JYZ_Printf("encrypt ERROR!\n");
		
		return 1;
	}
}
//==================带加密验证========================
static unsigned char Authentication_Encpryt(unsigned char zone,unsigned char *secretKey,unsigned char *sessionkey)
{
	unsigned char nc[7];
	unsigned char ci[8];		//包含前一个byte aac
	//unsigned char sk[8];
	unsigned char gc[8];
	unsigned char rand[8];
	unsigned char ch[8];
	unsigned char q2[8];
	unsigned char vfydata[16];
	unsigned char newci[8];
	//volatile int i;
	//get nc ci
	
	
	
	ReadConfigurationZone_Encrypt(ADDR_AAC(zone),(LEN_AAC+LEN_CRYPTOGRAM_Ci),ci);

	ReadConfigurationZone_Encrypt(ADDR_IDENTI_NUM_Nc,LEN_IDENTI_NUM_Nc,nc);	

	GenerateGC(nc,secretKey,gc);

	//gen q0 random data
	GenRandomnumber(rand);

	//gen q1=f2(gc,ci,q0)  is ch
	ChipFunGenCH(gc,ci,rand,ch);

	//q2=f2(gc q1)
	ChipFunGenQ2(q2,sessionkey);

	//send auth command
	memcpy(vfydata,rand,8);
	memcpy(vfydata+8,ch,8);


		
	CommandWriteWithRty(CMD_CRYPTO_VERIFY,0x0+zone,0x0,0x10,vfydata);
	//read new ci
	ReadConfigurationZone_Encrypt(ADDR_AAC(zone),(LEN_AAC+LEN_CRYPTOGRAM_Ci),newci);
	
	//compare ci q2
	if(memcmp(newci,q2,8)==0)
	{
		
		JYZ_Printf("encrypt ok!\n");
		
		authorized_en = 1;
		return 0;
	}
	else
	{
		
		JYZ_Printf("encrypt ERROR!\n");
		
		authorized_en = 0;
		return 1;
	}
}

//================启动加密====================
static unsigned char Encryption_zone(unsigned char zone,unsigned char *sessionkey,unsigned char *sessionkey2)
{
	unsigned char nc[7];
	unsigned char ci[8];		//包含前一个byte aac
	unsigned char rand[8];
	unsigned char ch[8];
	unsigned char q2[8];
	unsigned char vfydata[16];
	unsigned char newci[8];
	
	ReadConfigurationZone_Encrypt(ADDR_IDENTI_NUM_Nc,LEN_IDENTI_NUM_Nc,nc);
	ReadConfigurationZone_Encrypt(ADDR_AAC(zone),(LEN_AAC+LEN_CRYPTOGRAM_Ci),ci);
	
	//gen q0 random data
	GenRandomnumber(rand);
	//gen q1=f2(sesk,ci,q0)  is ch
	ChipFunGenCH(sessionkey,ci,rand,ch);
	//q2=f2(gc q1)
	ChipFunGenQ2(q2,sessionkey2);
	//send auth command
	memcpy(vfydata,rand,8);
	memcpy(vfydata+8,ch,8);
	
	CommandWriteWithRty(CMD_CRYPTO_VERIFY,0x10+zone,0x0,0x10,vfydata);
	//read new ci

	ReadConfigurationZone_Encrypt(ADDR_AAC(zone),(LEN_AAC+LEN_CRYPTOGRAM_Ci),newci);
	ReadConfigurationZone_Encrypt(ADDR_AAC(zone),(LEN_AAC+LEN_CRYPTOGRAM_Ci),newci);
	//compare ci q2
	if(memcmp(newci,q2,8)==0)
	{
		
		JYZ_Printf("encrypt2 ok!\n");
		
		encrypted_en = 1;
		return 0;
	}
	else
	{
		
		JYZ_Printf("encrypt2 ERROR!\n");
		
		encrypted_en = 0;
		return 1;
	}
}
//测试 MTZ
static unsigned char memtest()
{
	unsigned char tempa,tempb=0;
	unsigned int i;
	tempa=0xaa;
	if(SyncCommandWrite(CMD_SYSTEM_WRITE,0x0,ADDR_MTZ,1,&tempa))
		return 1;
	for(i=0;i<1024*1024;i++);
	if(SyncCommandRead(CMD_SYSTEM_READ,0x0,ADDR_MTZ,1,&tempb))
		return 1;
	
	if(tempb==tempa) 
	{
		
		JYZ_Printf("memtest ok!\n");
		
		return 0;
	}
	else
	{
		
		JYZ_Printf("memtest ERROR!\n");
		
		return 1;
	}
}
//验证
static unsigned char ConfigUnlock(unsigned char zone)
{
	unsigned char sessionkey[8];
	unsigned char temp8;
	
	temp8 = Authentication_Standard(zone,SupperKey,sessionkey);
	if(temp8!=0)
		return 1;
	return 0;
}


static void _delayTime()
{
	volatile int j;
	for(j = 0; j<1024*100;j++)
	{
		;
	}
}



//=========初始化4个用户区域的数据===========================
static unsigned char BurnUserDataZone(unsigned char (*ppBuffer)[4],unsigned char len[4])
{
	unsigned char rdata[32];
	unsigned char i;
	for(i = 0 ; i < 1 ; i++)
		if(len[i]>0x20)
			return 1;
	//设置用户区域数据
	for(i = 0 ; i <  1; i++)
	{
		if(len[i])
		{
			if(ppBuffer[i]==NULL)
				return 1;
			WriteUserDataZone_Standard(i,0,ppBuffer[i],len[i]);
			_delayTime();
			ReadUserDataZone_Standard(i,0,rdata,len[i]);
			if(memcmp(ppBuffer[i],rdata,len[i]))
				return 1;	
		}
	}
	return 0;
}
//========================烧写配置区域==================================	
static unsigned char BurnConfigZone()
{
	WriteconfigVerify();
	InitialAT88SC0104();//b4 system
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
static void PreSendFirstCmd(void)
{
	//pre5CLK(GPx_sdata_BP, GPx_sclk_BP);
	return;
}
//=========================烧写输出接口======================================
unsigned char BurnMain(unsigned char (*ppBuffer)[4], unsigned char len[4])
{	

	PreSendFirstCmd();
	if(memtest()!= 0)//判断芯片是否正常 
	{
		JYZ_Printf("memtest error\n"); 
		return 1;
	}
	if(ReadFuse()!=7)//判断熔丝是否正常
	{
		JYZ_Printf("fuse value is error\n");
		return 2;
	}
	JYZ_Printf("fuse value is OK\n");
	return 0;
	if(BurnUserDataZone(ppBuffer,len)) //初始化用户区域
	{
		JYZ_Printf("write user data zone fail\n");
		return 3;
	}
	BurnConfigZone();				//初始化配置区域
	if(ConfigUnlock(0)!=0)				//验证配置区域是否正常
	{
		JYZ_Printf("encryption unlick error\n");
		return 4;
	}
	
	WriteFuse();						//固化熔丝
	if(ReadFuse()!=0)					//判断是否固化成功n
	{
		JYZ_Printf("fuse blown fail\n");
		return 5;
	}
	
	return 0;
}
//===================读写通信输出接口=============================
unsigned char GetUserDataZone(unsigned char zone,unsigned char add,unsigned char *pointer,unsigned char length)
{
	unsigned char temp8;
	unsigned char sessionkey2[8],sessionkey[8];
	JYZ_Printf("GetUserDataZone\n");
	temp8 = Authentication_Encpryt(zone,SupperKey,sessionkey);
	if(temp8!=0)
		return 1;
	JYZ_Printf("GetUserDataZone\n");
	temp8 = Encryption_zone(zone,sessionkey,sessionkey2);
	if(temp8!=0)
		return 1;
	JYZ_Printf("GetUserDataZone\n");
	ReadUserDataZone_Encrypt(zone,add,pointer,length);
	JYZ_Printf("GetUserDataZone\n");
	return 0;
}
unsigned char FillUserDataZone(unsigned char zone,unsigned char add,unsigned char *pointer,unsigned char length)
{
	unsigned char temp8;
	unsigned char sessionkey2[8],sessionkey[8];
	temp8 = Authentication_Encpryt(zone,SupperKey,sessionkey);
	if(temp8!=0)
		return 1;
	temp8 = Encryption_zone(zone,sessionkey,sessionkey2);
	if(temp8!=0)
		return 1;
	temp8 = Encryption_zone(zone,sessionkey,sessionkey2);
	if(temp8!=0)
		return 1;
	WriteUserDataZone_Encrypt(zone,add,pointer,length);
	return 0;
}

int TestEncrypt( int *len1,int* len2 )
{
	int j;

	int ret;
	char ppBuffer[4][4]={{0x12,0x34,0x56,0x78},};
	int len[4]={0x4,0x00,0x00,0x00};
	JYZ_Printf("begin Test Encrypt\n");

	at88_fd = open(AT88_DEVICE, O_RDWR);
	if (at88_fd == -1)
	{
		JYZ_Printf("open fail");
		return 1;
	}
	//SDA 	:GPD22 	= 32*3 +22 	=0x76
	//SCL	:GPD9    	= 32*3 +9	=0x69
	//if(ioctl(at88_fd,COM_I2C_INIT,0x3132)) // scl = 0x31 = GPIO49 SDA = 0x32=GPIO50
	if(ioctl(at88_fd,COM_I2C_INIT,0x6976)) // scl = 0x31 = GPIO49 SDA = 0x32=GPIO50
	{
		JYZ_Printf("Error reading ipu information\n");
		close(at88_fd);
		at88_fd= -1;
		return 1;
	}

	#if 0
		BurnMain(ppBuffer,len);
	#else
		if(GetUserDataZone(0,0,len1,4))
		    return 1;
		if(GetUserDataZone(0,4,len2,4))
		    return 1;
	#endif

	close(at88_fd);

    //printf( "Len1 = %d, Len2 = %d\n", *len1, *len2 );
	JYZ_Printf("end Test Encrypt \n");
	return 0;
}

