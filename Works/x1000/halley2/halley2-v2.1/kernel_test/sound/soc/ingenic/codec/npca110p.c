/*
 * npca110p.c  --  NPCA110P ALSA Soc Audio driver
 *
 * Copyright 2016 Ingenic Semiconductor Co.,Ltd
 *
 * Author: tjin <tao.jin@ingenic.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/initval.h>
#include <sound/tlv.h>
#include <mach/jzsnd.h>
#include <linux/gpio.h>

#include "npca110p.h"

/* codec private data */
struct npca110p_priv {
	unsigned int sysclk;
	struct i2c_client * i2c_client;
	struct mutex i2c_access_lock;
	enum snd_soc_control_type control_type;
	struct npca110p_platform_data *control_pin;
}* npca110p;

/*
 * npca110p virtual register cache
 */
/*
 * Note that:
 * We make the virtual register, refer to the npca110p's i2c command.
 * The register value must be equal to the same register value in npca110p.h.
 * Otherwise there will be a problem.
 * The MSB byte is fix to 0x00.
*/
static const u32 npca110p_reg[] = {
	0x00005050,		/* DAC1_VOL */
	0x00005050,		/* DAC2_VOL */
	0x00001212,		/* ADC_PGA_VOL */
	0x00001414,		/* ADC_VOL */
	0x00000088,		/* ADC_INPUT_SELECT */
	0x0000cb00,		/* ORIGINAL_BASS */
	0x0000cf00,		/* MAXX_BYPASS */
	0x0000d139,		/* MAXX_BASS_INTENSITY */
	0x0000d2ab,		/* MAXX_TREBLE_INTENSITY */
	0x0000d368,		/* MAXX_3D_INTENSITY */
};

static unsigned char npca110p_codec_command[] = {
	0xff, 0xad, 0x80,	/* DAC1_VOL */
	0xff, 0xad, 0x87,	/* DAC2_VOL */
	0xff, 0xad, 0x81,	/* ADC_PGA_VOL */
	0xff, 0xad, 0x82,	/* ADC_VOL */
	0xff, 0xad, 0x83,	/* ADC_INPUT_SELECT */
};

/* codec command latch */
static unsigned char npca110p_codec_latch[] = {
	0xff, 0xad, 0x86,
	0x00, 0x00, 0x00,
	0xff, 0xad, 0x86,
	0x00, 0x00, 0x01,
};

static int npca110p_i2c_write(unsigned char* data, unsigned int len)
{
        int ret = -1;
        struct i2c_client *client = npca110p->i2c_client;

        if (client == NULL) {
                printk("===>NOTE: %s error\n", __func__);
                return ret;
        }

        if(!data || len <= 0){
                printk("===>ERROR: %s\n", __func__);
                return ret;
        }

        mutex_lock(&npca110p->i2c_access_lock);

        ret = i2c_master_send(client, data, len);
        if (ret < len)
                printk("%s err %d!\n", __func__, ret);

        mutex_unlock(&npca110p->i2c_access_lock);

        return ret < len ? ret : 0;
}

static int npca110p_i2c_read(unsigned char* data, unsigned int len)
{
        int ret = -1;
        struct i2c_client *client = npca110p->i2c_client;

        if (client == NULL) {
                printk("===>NOTE: %s error\n", __func__);
                return ret;
        }

        if(!data || len <= 0){
                printk("===>ERROR: %s\n", __func__);
                return ret;
        }

        mutex_lock(&npca110p->i2c_access_lock);

        ret = i2c_master_recv(client, data, len);
        if (ret < 0)
                printk("%s err\n", __func__);

        mutex_unlock(&npca110p->i2c_access_lock);

        return ret < len ? ret : 0;
}

/*
 * read npca110p register cache
 */
static inline unsigned int npca110p_read_reg_cache(struct snd_soc_codec *codec,
							unsigned int reg)
{
	u32 *cache = codec->reg_cache;

        if (reg >= NPCA110P_REGNUM)
                return 0;

        return cache[reg];
}

/*
 * write npca110p register cache
 */
static inline void npca110p_write_reg_cache(struct snd_soc_codec *codec,
	unsigned int reg, unsigned int value)
{
	u32 *cache = codec->reg_cache;

        if (reg >= NPCA110P_REGNUM)
                return;

        cache[reg] = (u32)value;
	return;
}

/*
 * write to the NPCA110P register space
 */
static int npca110p_write(struct snd_soc_codec *codec, unsigned int reg,
	unsigned int value)
{
	int ret =0;
	u8 data[3];
	int i;
	data[0] = (u8)((value >> 16) & 0xff);
	data[1] = (u8)((value >> 8)  & 0xff);
	data[2] = (u8)(value & 0xff);

	//printk("%s reg: 0x%02x,val: 0x%02x 0x%02x 0x%02x \n", __func__,reg, data[0], data[1], data[2]);
	/* If it's a codec register, the first command send */
	if (reg <= CODEC_REGNUM)
		ret |= npca110p_i2c_write((npca110p_codec_command + reg*3), 3);

	ret |= npca110p_i2c_write(data, 3);

	/* If it's a codec register, latch the command */
	if (reg <= CODEC_REGNUM) {
		for (i = 0; i < 4; i++)
			ret |= npca110p_i2c_write((npca110p_codec_latch + i*3), 3);
	}

	if (ret != 0)
		printk("%s i2c error \n", __func__);
	else
		npca110p_write_reg_cache(codec, reg, value);

	return 0;
}

static int codec_reset(void)
{
        int ret = 0;

        if (npca110p->control_pin->reset->gpio != -1) {
		gpio_set_value(npca110p->control_pin->reset->gpio, npca110p->control_pin->reset->active_level);
		mdelay(50);

		gpio_set_value(npca110p->control_pin->reset->gpio, !npca110p->control_pin->reset->active_level);
		mdelay(50);
	}

        return ret;
}


static int ingenic_snd_soc_info_volsw(struct snd_kcontrol *kcontrol,
        struct snd_ctl_elem_info *uinfo)
{
        struct soc_mixer_control *mc =
                (struct soc_mixer_control *)kcontrol->private_value;
        int platform_max;
        unsigned int shift = mc->shift;
        unsigned int rshift = mc->rshift;

	if (!mc->platform_max)
                mc->platform_max = mc->max;
        platform_max = mc->platform_max;

	if (platform_max == 1 && !strstr(kcontrol->id.name, " Volume"))
                uinfo->type = SNDRV_CTL_ELEM_TYPE_BOOLEAN;
        else
                uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;

        uinfo->count = shift == rshift ? 1 : 2;
        uinfo->value.integer.min = 0;
        uinfo->value.integer.max = platform_max - mc->min;
        return 0;
}

static int ingenic_snd_soc_put_volsw(struct snd_kcontrol *kcontrol,
        struct snd_ctl_elem_value *ucontrol)
{
	struct soc_mixer_control *mc =
                (struct soc_mixer_control *)kcontrol->private_value;
        struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
        unsigned int reg = mc->reg;
        unsigned int shift = mc->shift;
        unsigned int rshift = mc->rshift;
        int max = mc->max;
	int min = mc->min;
        unsigned int mask = (1 << fls(max)) - 1;
        unsigned int invert = mc->invert;
        unsigned int val, val2, val_mask;

        val = ((ucontrol->value.integer.value[0] + min) & mask);
        if (invert)
                val = max - val + min;
        val_mask = mask << shift;
        val = val << shift;
        if (shift != rshift) {
                val2 = ((ucontrol->value.integer.value[1] + min) & mask);
                if (invert)
                        val2 = max - val2 + min;
                val_mask |= mask << rshift;
                val |= val2 << rshift;
	}
	return snd_soc_update_bits_locked(codec, reg, val_mask, val);
}

static int ingenic_snd_soc_get_volsw(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	struct soc_mixer_control *mc =
		(struct soc_mixer_control *)kcontrol->private_value;
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	unsigned int reg = mc->reg;
	unsigned int shift = mc->shift;
	unsigned int rshift = mc->rshift;
	int max = mc->max;
	int min = mc->min;
	unsigned int mask = (1 << fls(max)) - 1;
	unsigned int invert = mc->invert;

	ucontrol->value.integer.value[0] =
		((snd_soc_read(codec, reg) >> shift) & mask) - min;
	if (shift != rshift)
		ucontrol->value.integer.value[1] =
			((snd_soc_read(codec, reg) >> rshift) & mask) - min;
	if (invert) {
		ucontrol->value.integer.value[0] =
			max - ucontrol->value.integer.value[0] - min;
		if (shift != rshift)
			ucontrol->value.integer.value[1] =
				max - ucontrol->value.integer.value[1] - min;
	}
	return 0;
}

#define INGENIC_SOC_DOUBLE_SX_TLV(xname, xreg, shift_left, shift_right, xmin, xmax, xinvert, tlv_array) \
{       .iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = (xname),\
	.access = SNDRV_CTL_ELEM_ACCESS_TLV_READ |\
		SNDRV_CTL_ELEM_ACCESS_READWRITE,\
	.tlv.p = (tlv_array), \
	.info = ingenic_snd_soc_info_volsw, .get = ingenic_snd_soc_get_volsw, \
	.put = ingenic_snd_soc_put_volsw, \
	.private_value = (unsigned long)&(struct soc_mixer_control) \
		{.reg = xreg, .shift = shift_left, .rshift = shift_right,\
		.min = xmin, .max = xmax, .platform_max = xmax, .invert = xinvert} }


static const char *npca110p_input_pin[] = {"RIN1/LIN1", "RIN2/LIN2", "RIN3/LIN3"};
static const char *npca110p_input_mix[] = {"Stereo", "Mono"};

static const char *npca110p_origial_bass[] = {"BASS OFF", "BASS ON"};
static const char *npca110p_maxxbass_bypass[] = {"ENABLE", "BYPASS"};
static const char *npca110p_maxxtreble_bypass[] = {"ENABLE", "BYPASS"};
static const char *npca110p_maxx3d_bypass[] = {"ENABLE", "BYPASS"};

static const struct soc_enum npca110p_enum[] = {
	SOC_ENUM_DOUBLE(ADC_INPUT_SELECT, 0, 4, 3, npca110p_input_pin),
	SOC_ENUM_SINGLE(ADC_INPUT_SELECT, 8, 2, npca110p_input_mix),

	SOC_ENUM_SINGLE(ORIGINAL_BASS, 0, 2, npca110p_origial_bass),
	SOC_ENUM_SINGLE(MAXX_BYPASS, 4, 2, npca110p_maxxbass_bypass),
	SOC_ENUM_SINGLE(MAXX_BYPASS, 5, 2, npca110p_maxxtreble_bypass),
	SOC_ENUM_SINGLE(MAXX_BYPASS, 2, 2, npca110p_maxx3d_bypass),
};

/* unit: 0.01dB */
#if 0
/* If you want to use full gain range:-126dB ~ 0dB, you can enable the code */
static const DECLARE_TLV_DB_SCALE(dac1_tlv, -12600, 150, 0);
#else
/* Here we just use -30dB ~ 0dB */
static const DECLARE_TLV_DB_SCALE(dac1_tlv, -3000, 150, 0);
#endif

#if 0
/* If you want to use full gain range:-126dB ~ 0dB, you can enable the code */
static const DECLARE_TLV_DB_SCALE(dac2_tlv, -12600, 150, 0);
#else
/* Here we just use -30dB ~ 0dB */
static const DECLARE_TLV_DB_SCALE(dac2_tlv, -3000, 150, 0);
#endif

static const DECLARE_TLV_DB_SCALE(adc_tlv,  -9600,  150, 0);
static const DECLARE_TLV_DB_SCALE(adc_pga_tlv, -1800,  100, 0);

static const struct snd_kcontrol_new npca110p_snd_controls[] = {
#if 0
	/* If you want to use full gain range:-126dB ~ 0dB, you can enable the code */
	SOC_DOUBLE_TLV("DAC1 Volume", DAC1_VOL, 0, 8, 0x54, 0, dac1_tlv),
#else
	INGENIC_SOC_DOUBLE_SX_TLV("DAC1 Volume", DAC1_VOL, 0, 8, 0x40, 0x54, 0, dac1_tlv),
#endif

#if 0
	/* If you want to use full gain range:-126dB ~ 0dB, you can enable the code */
	SOC_DOUBLE_TLV("DAC2 Volume", DAC2_VOL, 0, 8, 0x54, 0, dac2_tlv),
#else
	INGENIC_SOC_DOUBLE_SX_TLV("DAC2 Volume", DAC2_VOL, 0, 8, 0x40, 0x54, 0, dac2_tlv),
#endif
	SOC_DOUBLE_TLV("ADC Volume",  ADC_VOL,  0, 8, 0x54, 1, adc_tlv),
	SOC_DOUBLE_TLV("ADC PGA Volume", ADC_PGA_VOL,  0, 8, 0x34, 0, adc_pga_tlv),

	SOC_ENUM("ADC Input Pin Select", npca110p_enum[0]),
	SOC_ENUM("ADC Stereo Mode", npca110p_enum[1]),

	SOC_ENUM("Original Bass Switch", npca110p_enum[2]),
	SOC_ENUM("Maxx Bass Switch", npca110p_enum[3]),
	SOC_SINGLE("Maxx Bass Intensity", MAXX_BASS_INTENSITY, 0, 0x7f, 0),
	SOC_ENUM("Maxx Treble Switch", npca110p_enum[4]),
	SOC_SINGLE("Maxx Treble Intensity", MAXX_TREBLE_INTENSITY, 0, 0x7f, 0),
	SOC_ENUM("Maxx 3D Switch", npca110p_enum[5]),
	SOC_SINGLE("Maxx 3D Intensity", MAXX_3D_INTENSITY, 0, 0x7f, 0),
};

/* npca110p dapm widgets */
static const struct snd_soc_dapm_widget npca110p_dapm_widgets[] = {
	/* npca110p dapm route */
	SND_SOC_DAPM_DAC("DAC", "Playback", SND_SOC_NOPM, 0, 0),
	SND_SOC_DAPM_ADC("ADC", "Capture", SND_SOC_NOPM, 0, 0),
	SND_SOC_DAPM_OUTPUT("DAC OUT"),
	SND_SOC_DAPM_INPUT("ADC IN"),
};

static const struct snd_soc_dapm_route npca110p_audio_map[] = {
	/* ADC */
	{"ADC", NULL, "ADC IN"},
	{"DAC OUT", NULL, "DAC"},
};

static int npca110p_set_dai_sysclk(struct snd_soc_dai *codec_dai,
	int clk_id, unsigned int freq, int dir)
{
	npca110p->sysclk = freq;
	return 0;
}

static int npca110p_hw_params(struct snd_pcm_substream *substream,
			    struct snd_pcm_hw_params *params,
			    struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_codec *codec = rtd->codec;
	int rate = params_rate(params);

	/* set fs */

	/* set rate */
	return 0;
}

static int npca110p_mute(struct snd_soc_dai *dai, int mute)
{
	if (!mute){

	} else {

	}
	return 0;
}

static int npca110p_set_bias_level(struct snd_soc_codec *codec,
	enum snd_soc_bias_level level)
{
	/* There is no bias level function in npca110p */
	switch (level) {
	case SND_SOC_BIAS_ON:
		break;
	case SND_SOC_BIAS_PREPARE:
		break;
	case SND_SOC_BIAS_STANDBY:
		break;
	case SND_SOC_BIAS_OFF:
		break;
	}
	codec->dapm.bias_level = level;
	return 0;
}

#ifdef  USE_48000_SAMPLE_RATE
#define NPCA110P_RATES   (SNDRV_PCM_RATE_48000)
#else
#define NPCA110P_RATES   (SNDRV_PCM_RATE_44100)
#endif

#define NPCA110P_FORMATS (SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S24_LE)

static struct snd_soc_dai_ops npca110p_dai_ops = {
	.hw_params	= npca110p_hw_params,
	.digital_mute	= npca110p_mute,
	.set_sysclk	= npca110p_set_dai_sysclk,
};

static struct snd_soc_dai_driver npca110p_dai = {
	.name = "npca110p-dai",
	.playback = {
		.stream_name = "Playback",
		.channels_min = 1,
		.channels_max = 2,
		.rates = NPCA110P_RATES,
		.formats = NPCA110P_FORMATS,
	},
	.capture = {
		.stream_name = "Capture",
		.channels_min = 2,
		.channels_max = 2,
		.rates = NPCA110P_RATES,
		.formats = NPCA110P_FORMATS,
	},
	.ops = &npca110p_dai_ops,
};

static int npca110p_suspend(struct snd_soc_codec *codec, pm_message_t state)
{
	return 0;
}

static int npca110p_resume(struct snd_soc_codec *codec)
{
	return 0;
}

static int npca110p_probe(struct snd_soc_codec *codec)
{
	int ret;
	int i;
	/* npca110p reset */
	ret = codec_reset();
        if (ret < 0) {
                dev_err(codec->dev, "Failed to issue reset\n");
                return ret;
        }

	/* npca110p init */
	for (i = 0; i < sizeof(g_abMaxxDSPCommands)/3 ; i++) {
		ret = npca110p_i2c_write(g_abMaxxDSPCommands + 3*i, 3);
		if (ret < 0) {
			printk("%s i2c write err\n", __func__);
			return ret;
		}
	}

	snd_soc_add_controls(codec, npca110p_snd_controls, ARRAY_SIZE(npca110p_snd_controls));
	return 0;
}

/* power down chip */
static int npca110p_remove(struct snd_soc_codec *codec)
{
	int ret = 0;
	unsigned char data[3] = {0};
	/* Mute AMP pin */

	/* Soft mute process route */
	data[0] = 0x00;
	data[1] = 0xc7;
	data[2] = 0x01;
	ret |= npca110p_i2c_write(data, 3);

	/* Power down */
	data[0] = 0x00;
	data[1] = 0xca;
	data[2] = 0x00;
	ret |= npca110p_i2c_write(data, 3);

	if (ret < 0) {
		printk("%s i2c write err\n", __func__);
		return ret;
	}
	return 0;
}

static struct snd_soc_codec_driver soc_codec_dev_npca110p = {
	.probe =	npca110p_probe,
	.remove =	npca110p_remove,
	.suspend =	npca110p_suspend,
	.resume =	npca110p_resume,
	/* Because npca110p hasnot i2c read register command, we only store the value set before. */
	.read = npca110p_read_reg_cache,
	.write = npca110p_write,
	.set_bias_level = npca110p_set_bias_level,
	/* These registers are virtual, because there are only i2c command for npca110p. */
	.reg_cache_size = NPCA110P_REGNUM,
	.reg_word_size = sizeof(u32),
	.reg_cache_default = npca110p_reg,
	.dapm_widgets = npca110p_dapm_widgets,
	.num_dapm_widgets = ARRAY_SIZE(npca110p_dapm_widgets),
	.dapm_routes = npca110p_audio_map,
	.num_dapm_routes = ARRAY_SIZE(npca110p_audio_map),
};

static __devinit int npca110p_i2c_probe(struct i2c_client *i2c,
				      const struct i2c_device_id *id)
{
	int ret = 0;
        struct device *dev = &i2c->dev;
        struct npca110p_platform_data *npca110p_data = dev->platform_data;

        if (!i2c_check_functionality(i2c->adapter, I2C_FUNC_I2C)) {
                return -ENODEV;
        }

        if (npca110p_data->power->gpio != -1) {
                ret = gpio_request_one(npca110p_data->power->gpio,
                                GPIOF_DIR_OUT, "npca110p_power");
                if (ret != 0) {
                        dev_err(dev, "Can't request power pin = %d\n",
                                        npca110p_data->power->gpio);
                        return ret;
                } else
                        gpio_set_value(npca110p_data->power->gpio, npca110p_data->power->active_level);
        }

        if (npca110p_data->reset->gpio != -1) {
                ret = gpio_request_one(npca110p_data->reset->gpio,
                                GPIOF_DIR_OUT, "npca110p_reset");
                if (ret != 0) {
                        dev_err(dev, "Can't request reset pin = %d\n",
                                        npca110p_data->reset->gpio);
                        return ret;
                }
        }

	npca110p = kzalloc(sizeof(struct npca110p_priv), GFP_KERNEL);
	if (npca110p == NULL)
		return -ENOMEM;

	npca110p->i2c_client   = i2c;
	npca110p->control_type = SND_SOC_I2C;
	npca110p->control_pin  = npca110p_data;

	mutex_init(&npca110p->i2c_access_lock);

	ret = snd_soc_register_codec(&i2c->dev, &soc_codec_dev_npca110p, &npca110p_dai, 1);
	if (ret < 0) {
		kfree(npca110p);
		dev_err(&i2c->dev, "Faild to register codec\n");
	}
	return ret;
}

static __devexit int npca110p_i2c_remove(struct i2c_client *client)
{
	struct device *dev = &client->dev;
        struct npca110p_platform_data *npca110p_data = dev->platform_data;

	if (npca110p_data->power->gpio != -1)
                gpio_free(npca110p_data->power->gpio);

	if (npca110p_data->reset->gpio != -1)
                gpio_free(npca110p_data->reset->gpio);

	snd_soc_unregister_codec(&client->dev);

	kfree(npca110p);
	return 0;
}

static const struct i2c_device_id npca110p_i2c_id[] = {
	{ "npca110p", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, npca110p_i2c_id);

static struct i2c_driver npca110p_i2c_driver = {
	.driver = {
		.name = "npca110p",
		.owner = THIS_MODULE,
	},
	.probe =    npca110p_i2c_probe,
	.remove =   __devexit_p(npca110p_i2c_remove),
	.id_table = npca110p_i2c_id,
};

static int __init npca110p_modinit(void)
{
	int ret = 0;
	ret = i2c_add_driver(&npca110p_i2c_driver);
	if (ret != 0) {
		printk(KERN_ERR "Failed to register NPCA110P I2C driver: %d\n",
		       ret);
	}
	return ret;
}
module_init(npca110p_modinit);

static void __exit npca110p_exit(void)
{
	i2c_del_driver(&npca110p_i2c_driver);
}
module_exit(npca110p_exit);

MODULE_DESCRIPTION("Soc NPCA110P driver");
MODULE_AUTHOR("tjin<tao.jin@ingenic.com>");
MODULE_LICENSE("GPL");
