#ifndef __BOARD_H__
#define __BOARD_H__
#include <gpio.h>
#include <soc/gpio.h>

/* ****************************GPIO KEY START******************************** */
#define GPIO_BOOT_SEL0         GPIO_PB(28)
#define ACTIVE_LOW_F1 		1
/* ****************************GPIO KEY END********************************** */

/* ****************************WIFI/BT START********************************** */
#define GPIO_WIFI_RST_N		GPIO_PC(17)
#define GPIO_WIFI_WAKE 		GPIO_PC(16)
#define WLAN_SDIO_INDEX			1
/* ****************************WIFI/BT END*********************************** */

/* ****************************MMC START************************************* */
/* MSC GPIO Definition */
#define GPIO_SD0_CD_N		GPIO_PA(7)
/* ****************************MMC END************************************** */

/* ****************************SPI START************************************** */
#ifdef CONFIG_SPI_GPIO
#define GPIO_SPI_SCK		GPIO_PA(26)
#define GPIO_SPI_MOSI		GPIO_PA(29)
#define GPIO_SPI_MISO		GPIO_PA(28)
#endif

#if defined(CONFIG_JZ_SPI) || defined(CONFIG_JZ_SFC)
#define SPI_CHIP_ENABLE GPIO_PA(27)
#endif
/* ****************************SPI END************************************** */

/* ****************************GPIO USB START******************************** */
#define GPIO_USB_ID			-1	/* GPIO_PC(21) */
#define GPIO_USB_ID_LEVEL	LOW_ENABLE
#ifdef CONFIG_BOARD_HAS_NO_DETE_FACILITY
#define GPIO_USB_DETE			-1 	/* GPIO_PC(22) */
#define GPIO_USB_DETE_LEVEL	LOW_ENABLE
#else
#define GPIO_USB_DETE		GPIO_PB(8)
#define GPIO_USB_DETE_LEVEL	HIGH_ENABLE
#endif
#define GPIO_USB_DRVVBUS		-1		/* GPIO_PB(25) */
#define GPIO_USB_DRVVBUS_LEVEL	HIGH_ENABLE
/* ****************************GPIO USB END********************************** */

/* ****************************GPIO AUDIO START****************************** */
#define GPIO_HP_MUTE			-1		/* hp mute gpio */
#define GPIO_HP_MUTE_LEVEL		-1		/* vaild level */

#define GPIO_SPEAKER_EN		GPIO_PC(25)		/* speaker enable gpio */
#define GPIO_SPEAKER_EN_LEVEL		1
#define GPIO_AMP_POWER_EN		-1		/* amp power enable pin */
#define GPIO_AMP_POWER_EN_LEVEL		-1


#define GPIO_HANDSET_EN			-1		/* handset enable gpio */
#define GPIO_HANDSET_EN_LEVEL		-1

#define GPIO_HP_DETECT			-1		/* hp detect gpio */
#define GPIO_HP_INSERT_LEVEL		1
#define GPIO_LINEIN_DETECT		-1		/* linein detect gpio */
#define GPIO_LINEIN_INSERT_LEVEL	-1
#define GPIO_MIC_SELECT			-1		/* mic select gpio */
#define GPIO_BUILDIN_MIC_LEVEL		-1		/* builin mic select level */
#define GPIO_MIC_DETECT			-1
#define GPIO_MIC_INSERT_LEVEL		-1
#define GPIO_MIC_DETECT_EN		-1		/* mic detect enable gpio */
#define GPIO_MIC_DETECT_EN_LEVEL	-1		/* mic detect enable gpio */

#define HP_SENSE_ACTIVE_LEVEL		1
#define HOOK_ACTIVE_LEVEL		-1
/* ****************************GPIO AUDIO END******************************** */

/* ****************************EFUSE START*********************************** */
#define GPIO_EFUSE_VDDQ	GPIO_PB(27)		/* EFUSE must be -ENODEV or a gpio */
/* ****************************EFUSE END************************************ */

#endif /* __BOARD_H__ */
