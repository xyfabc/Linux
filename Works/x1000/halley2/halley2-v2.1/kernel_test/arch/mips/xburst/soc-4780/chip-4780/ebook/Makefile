
obj-y += ebook-misc.o ebook-pm.o ebook-lcd.o

obj-$(CONFIG_I2C)		+= ebook-i2c.o
obj-$(CONFIG_MFD_ACT8600)	+= ebook-regulator.o
obj-$(CONFIG_MMC_JZ4780)	+= ebook-mmc.o
obj-$(CONFIG_JZ4780_INTERNAL_CODEC) += ebook-sound.o

ifeq ($(CONFIG_BOARD_TROOPER), y)
EXTRA_CFLAGS+=-DCONFIG_BOARD_WARRIOR_V_1_1
endif
