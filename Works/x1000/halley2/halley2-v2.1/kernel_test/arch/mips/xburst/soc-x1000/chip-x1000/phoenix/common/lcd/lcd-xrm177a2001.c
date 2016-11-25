#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mm.h>
#include <linux/console.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/pwm_backlight.h>
#include <linux/lcd.h>
#include <linux/regulator/consumer.h>
#include <mach/jzfb.h>
#include "../board_base.h"
#include "board.h"
#include <linux/delay.h>


struct xrm177a2001_power{
	struct regulator *vlcdio;
	struct regulator *vlcdvcc;
	int inited;
};

static struct xrm177a2001_power lcd_power = {
	NULL,
	NULL,
	0
};

int xrm177a2001_power_init(struct lcd_device *ld)
{
	int ret ;

	if (GPIO_LCD_RST > 0) {
		ret = gpio_request(GPIO_LCD_RST, "lcd rst");
		if (ret) {
			printk(KERN_ERR "can's request lcd rst\n");
			return ret;
		}
	}

	if (GPIO_LCD_CS > 0) {
		ret = gpio_request(GPIO_LCD_CS, "lcd cs");
		if (ret) {
			printk(KERN_ERR "can's request lcd cs\n");
			return ret;
		}
	}

	if (GPIO_LCD_RD > 0) {
		ret = gpio_request(GPIO_LCD_RD, "lcd rd");
		if (ret) {
			printk(KERN_ERR "can's request lcd rd\n");
			return ret;
		}
	}
	lcd_power.inited = 1;

	return 0;
}

int xrm177a2001_power_reset(struct lcd_device *ld)
{
	if (!lcd_power.inited)
		return -EFAULT;

	gpio_direction_output(GPIO_LCD_RST, 1);
	mdelay(10);
	gpio_direction_output(GPIO_LCD_RST, 0);
	mdelay(10);
	gpio_direction_output(GPIO_LCD_RST, 1);
	mdelay(120);

	return 0;
}

int xrm177a2001_power_on(struct lcd_device *ld, int enable)
{
	if (!lcd_power.inited && xrm177a2001_power_init(ld))
		return -EFAULT;

	if (enable) {
		gpio_direction_output(GPIO_LCD_CS, 1);
		gpio_direction_output(GPIO_LCD_RD, 1);

		xrm177a2001_power_reset(ld);

		mdelay(5);
		gpio_direction_output(GPIO_LCD_CS, 0);

	} else {
		mdelay(5);
		gpio_direction_output(GPIO_LCD_CS, 0);
		gpio_direction_output(GPIO_LCD_RD, 0);
		gpio_direction_output(GPIO_LCD_RST, 0);
	}
	return 0;
}

static struct lcd_platform_data xrm177a2001_pdata = {
	.reset = xrm177a2001_power_reset,
	.power_on = xrm177a2001_power_on,
};

/* LCD Panel Device */
struct platform_device xrm177a2001_device = {
	.name = "xrm177a2001_slcd",
	.dev = {
		.platform_data = &xrm177a2001_pdata,
	},
};

static struct smart_lcd_data_table xrm177a2001_data_table[] = {
	{SMART_CONFIG_CMD, 0x0011}, {SMART_CONFIG_UDELAY, 120000},
	{SMART_CONFIG_CMD, 0x00b1}, {SMART_CONFIG_DATA,	0x05}, {SMART_CONFIG_DATA, 0x3c}, {SMART_CONFIG_DATA, 0x3c},
	{SMART_CONFIG_CMD, 0x00b2}, {SMART_CONFIG_DATA,	0x05}, {SMART_CONFIG_DATA, 0x3c}, {SMART_CONFIG_DATA, 0x3c},
	{SMART_CONFIG_CMD, 0x00b3}, {SMART_CONFIG_DATA,	0x05}, {SMART_CONFIG_DATA, 0x3c}, {SMART_CONFIG_DATA, 0x3c},
	{SMART_CONFIG_DATA, 0x05}, {SMART_CONFIG_DATA, 0x3c}, {SMART_CONFIG_DATA, 0x3c},
	{SMART_CONFIG_CMD, 0x00b4}, {SMART_CONFIG_DATA,	0x03},
	{SMART_CONFIG_CMD, 0x00c0}, {SMART_CONFIG_DATA,	0x28}, {SMART_CONFIG_DATA, 0x08}, {SMART_CONFIG_DATA, 0x04},
	{SMART_CONFIG_CMD, 0x00c1}, {SMART_CONFIG_DATA,	0xc0},
	{SMART_CONFIG_CMD, 0x00c2}, {SMART_CONFIG_DATA,	0x0d}, {SMART_CONFIG_DATA, 0x00},
	{SMART_CONFIG_CMD, 0x00c3}, {SMART_CONFIG_DATA,	0x8d}, {SMART_CONFIG_DATA, 0x2a},
	{SMART_CONFIG_CMD, 0x00c4}, {SMART_CONFIG_DATA,	0x8d}, {SMART_CONFIG_DATA, 0xee},
	{SMART_CONFIG_CMD, 0x00c5}, {SMART_CONFIG_DATA,	0x1a},
	/* {SMART_CONFIG_CMD, 0x0036}, {SMART_CONFIG_DATA,	0x08}, */
	{SMART_CONFIG_CMD, 0x00e0}, {SMART_CONFIG_DATA,	0x04}, {SMART_CONFIG_DATA, 0x22}, {SMART_CONFIG_DATA, 0x07},
	{SMART_CONFIG_DATA, 0x0a}, {SMART_CONFIG_DATA, 0x2e}, {SMART_CONFIG_DATA, 0x30},
	{SMART_CONFIG_DATA, 0x25}, {SMART_CONFIG_DATA, 0x2a}, {SMART_CONFIG_DATA, 0x28},
	{SMART_CONFIG_DATA, 0x26}, {SMART_CONFIG_DATA, 0x2e}, {SMART_CONFIG_DATA, 0x3a},
	{SMART_CONFIG_DATA, 0x00}, {SMART_CONFIG_DATA, 0x01}, {SMART_CONFIG_DATA, 0x03},
	{SMART_CONFIG_DATA, 0x13},
	{SMART_CONFIG_CMD, 0x00e1}, {SMART_CONFIG_DATA,	0x04}, {SMART_CONFIG_DATA, 0x16}, {SMART_CONFIG_DATA, 0x06},
	{SMART_CONFIG_DATA, 0x0d}, {SMART_CONFIG_DATA, 0x2d}, {SMART_CONFIG_DATA, 0x26},
	{SMART_CONFIG_DATA, 0x23}, {SMART_CONFIG_DATA, 0x27}, {SMART_CONFIG_DATA, 0x27},
	{SMART_CONFIG_DATA, 0x25}, {SMART_CONFIG_DATA, 0x2d}, {SMART_CONFIG_DATA, 0x3b},
	{SMART_CONFIG_DATA, 0x00}, {SMART_CONFIG_DATA, 0x01}, {SMART_CONFIG_DATA, 0x04},
	{SMART_CONFIG_DATA, 0x13},
	{SMART_CONFIG_CMD, 0x002a}, {SMART_CONFIG_DATA, 0x00}, {SMART_CONFIG_DATA, 0x02},
	{SMART_CONFIG_DATA,0x00}, {SMART_CONFIG_DATA,0x81},
	{SMART_CONFIG_CMD, 0x002b}, {SMART_CONFIG_DATA, 0x00}, {SMART_CONFIG_DATA, 0x01},
	{SMART_CONFIG_DATA,0x00}, {SMART_CONFIG_DATA,0xa0},
	{SMART_CONFIG_CMD, 0x003a}, {SMART_CONFIG_DATA,	0x06},
	{SMART_CONFIG_CMD, 0x0029},
};

unsigned long xrm177a2001_cmd_buf[] = {
	0x2c2c2c2c,
};

struct fb_videomode jzfb_videomode = {
	.name = "128x160",
	.refresh = 60,
	.xres = 128,
	.yres = 160,
	.pixclock = KHZ2PICOS(22119 * 2),
	.left_margin = 0,
	.right_margin = 0,
	.upper_margin = 0,
	.lower_margin = 0,
	.hsync_len = 0,
	.vsync_len = 0,
	.sync = FB_SYNC_HOR_HIGH_ACT | FB_SYNC_VERT_HIGH_ACT,
	.vmode = FB_VMODE_NONINTERLACED,
	.flag = 0,
};

struct jzfb_platform_data jzfb_pdata = {

	.num_modes = 1,
	.modes = &jzfb_videomode,
	.lcd_type = LCD_TYPE_SLCD,
	.bpp = 18,
	.width = 31,
	.height = 39,
	.pinmd = 0,

	.smart_config.rsply_cmd_high = 0,
	.smart_config.csply_active_high = 0,
	.smart_config.newcfg_fmt_conv = 0,


	.smart_config.write_gram_cmd = xrm177a2001_cmd_buf,
	.smart_config.length_cmd = ARRAY_SIZE(xrm177a2001_cmd_buf),
	.smart_config.bus_width = 8,
	.smart_config.data_table = xrm177a2001_data_table,
	.smart_config.length_data_table = ARRAY_SIZE(xrm177a2001_data_table),
	.smart_config.datatx_type_serial = 1,
	.smart_config.cmdtx_type_serial = 1,
	.dither_enable = 0,
};

#ifdef CONFIG_BACKLIGHT_PWM
static int backlight_init(struct device *dev)
{
	return 0;
}

static int backlight_notify(struct device *dev, int brightness)
{
	return brightness;
}

static void backlight_exit(struct device *dev)
{
}

static struct platform_pwm_backlight_data backlight_data = {
	.pwm_id     = 4,
	.max_brightness = 255,
	.dft_brightness = 120,
	.pwm_period_ns  = 30000,
	.init       = backlight_init,
	.exit       = backlight_exit,
	.notify		= backlight_notify,
};
struct platform_device backlight_device = {
	.name       = "pwm-backlight",
	.dev        = {
		.platform_data  = &backlight_data,
	},
};
#endif
