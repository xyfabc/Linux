#ifndef __BOARD_H__
#define __BOARD_H__
#include <gpio.h>
#include <soc/gpio.h>


/* MSC GPIO Definition */
#define GPIO_SD0_CD_N       GPIO_PC(21)

#ifdef CONFIG_BCMDHD_1_141_66
/**
 *  ** Bluetooth gpio
 *   **/
#define BLUETOOTH_UPORT_NAME    "ttyS0"
#define GPIO_BT_REG_ON          GPIO_PC(18)
#define GPIO_BT_WAKE            GPIO_PC(20)
#define GPIO_BT_INT             GPIO_PC(19)
#define GPIO_BT_UART_RTS        GPIO_PC(13)
/*wifi*/
#define GPIO_WIFI_RST_N		GPIO_PC(17)
#define GPIO_WIFI_WAKE 		GPIO_PC(16)

#endif

#ifdef CONFIG_SPI_GPIO
#define GPIO_SPI_SCK  GPIO_PA(26)
#define GPIO_SPI_MOSI GPIO_PA(29)
#define GPIO_SPI_MISO GPIO_PA(28)
#endif

/* ****************************GPIO KEY START******************************** */
#define GPIO_BOOT_SEL0		GPIO_PB(28)
#define GPIO_WKUP		GPIO_PB(31)
//#define GPIO_MACTXEN		GPIO_PA(10)
//#define GPIO_MACMDC		GPIO_PA(11)
#define ACTIVE_LOW_POWERDOWN 	1
#define ACTIVE_LOW_F4 		0
#define ACTIVE_LOW_F5 		0
/* ****************************GPIO KEY END******************************** */

/* ****************************GPIO USB START******************************** */
#define GPIO_USB_ID             GPIO_PD(2)/*GPIO_PD(2)*/
#define GPIO_USB_ID_LEVEL       LOW_ENABLE
#define GPIO_USB_DETE           GPIO_PB(8) /*GPIO_PB(8)*/
#define GPIO_USB_DETE_LEVEL     LOW_ENABLE
#define GPIO_USB_DRVVBUS        GPIO_PB(25)
#define GPIO_USB_DRVVBUS_LEVEL      HIGH_ENABLE
/* ****************************GPIO USB END********************************** */

/* ****************************GPIO AUDIO START****************************** */
#define GPIO_HP_MUTE        -1  /*hp mute gpio*/
#define GPIO_HP_MUTE_LEVEL  -1  /*vaild level*/

#define GPIO_SPEAKER_EN    	GPIO_PC(23)         /*speaker enable gpio*/
#define GPIO_SPEAKER_EN_LEVEL   0
#define GPIO_AMP_POWER_EN	-1	/* amp power enable pin */
#define GPIO_AMP_POWER_EN_LEVEL	-1

#define GPIO_HANDSET_EN     	-1  /*handset enable gpio*/
#define GPIO_HANDSET_EN_LEVEL   -1

#define GPIO_HP_DETECT  	-1      /*hp detect gpio*/
#define GPIO_HP_INSERT_LEVEL    -1
#define GPIO_LINEIN_DETECT          -1      /*linein detect gpio*/
#define GPIO_LINEIN_INSERT_LEVEL    -1
#define GPIO_MIC_SELECT     	-1  /*mic select gpio*/
#define GPIO_BUILDIN_MIC_LEVEL  -1  /*builin mic select level*/
#define GPIO_MIC_DETECT     	-1
#define GPIO_MIC_INSERT_LEVEL   -1
#define GPIO_MIC_DETECT_EN  	-1  /*mic detect enable gpio*/
#define GPIO_MIC_DETECT_EN_LEVEL -1 /*mic detect enable gpio*/

#define HOOK_ACTIVE_LEVEL       -1

/* ****************************GPIO AUDIO END******************************** */

/* ****************************GPIO LCD START****************************** */
#ifdef  CONFIG_LCD_XRM2002903
#define GPIO_LCD_RD     GPIO_PB(16)
#define GPIO_LCD_CS     GPIO_PB(18)
#define GPIO_LCD_RST    GPIO_PD(5)
//#define GPIO_BL_PWR_EN  GPIO_PC(24)
#define GPIO_LCD_PWM    GPIO_PC(24)
#endif
#ifdef  CONFIG_LCD_TRULY_TFT240240_2_E
#define GPIO_LCD_RD     GPIO_PB(16)
#define GPIO_LCD_CS     GPIO_PB(18)
#define GPIO_LCD_RST    GPIO_PD(0)
#define GPIO_BL_PWR_EN  GPIO_PD(1)
#define GPIO_LCD_PWM    GPIO_PC(25)
#endif
/* ****************************GPIO LCD END******************************** */

/* ****************************GPIO TOUCHSCREEN START****************************** */
#if defined(CONFIG_TOUCHSCREEN_FT6X06) || defined(CONFIG_TOUCHSCREEN_FT6X06_MODULE)
#define GPIO_TP_INT		GPIO_PD(4)
#define GPIO_TP_RESET		GPIO_PD(5)
#endif
/* ****************************GPIO TOUCHSCREEN END******************************** */

#define GPIO_EFUSE_VDDQ			GPIO_PC(27)	/* EFUSE must be -ENODEV or a gpio */

#ifdef CONFIG_I2C_GPIO

#ifdef CONFIG_SOFT_I2C0_GPIO_V12_JZ
#define GPIO_I2C0_SDA		-1
#define GPIO_I2C0_SCK		-1
#endif

#ifdef CONFIG_SOFT_I2C1_GPIO_V12_JZ
#define GPIO_I2C1_SDA		-1
#define GPIO_I2C1_SCK		-1
#endif

#endif /* CONFIG_I2C_GPIO */

#ifdef CONFIG_VIDEO_JZ_CIM_HOST
#define FRONT_CAMERA_INDEX  0
#define BACK_CAMERA_INDEX   1

#define CAMERA_SENSOR_RESET GPIO_PD(5)
#define CAMERA_FRONT_SENSOR_PWDN  GPIO_PD(4)
#define CAMERA_VDD_EN  GPIO_PD(3)
#endif

#endif /* __BOARD_H__ */
