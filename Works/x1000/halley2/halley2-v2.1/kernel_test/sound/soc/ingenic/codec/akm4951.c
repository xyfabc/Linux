/*
 * akm4951.c  --  AKM4951 ALSA Soc Audio driver
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

#include "akm4951.h"

//#define  USE_ALL_GAIN_RANGE	1

/* codec private data */
struct akm4951_priv {
	unsigned int sysclk;
	struct i2c_client * i2c_client;
	struct mutex i2c_access_lock;
	enum snd_soc_control_type control_type;
	struct akm4951_platform_data *control_pin;
}* akm4951;

static unsigned char power_reg0;
static unsigned char power_reg1;
static unsigned long user_replay_rate = 48000;

//static const u8 akm4951_reg[] = { };

static int akm4951_i2c_write_regs(unsigned char reg, unsigned char* data, unsigned int len)
{
	int ret = -1;
	int i = 0;
	unsigned char buf[80] = {0};
	struct i2c_client *client = akm4951->i2c_client;

	if (client == NULL) {
		printk("===>NOTE: %s error\n", __func__);
		return ret;
	}

	if(!data || len <= 0){
		printk("===>ERROR: %s\n", __func__);
		return ret;
	}

	mutex_lock(&akm4951->i2c_access_lock);
	buf[0] = reg;
	for(; i < len; i++) {
		buf[i+1] = *data;
		data++;
	}

	ret = i2c_master_send(client, buf, len+1);
	if (ret < len+1)
		printk("%s 0x%02x err %d!\n", __func__, reg, ret);
	mutex_unlock(&akm4951->i2c_access_lock);

	return ret < len+1 ? ret : 0;
}

static int akm4951_i2c_read_reg(unsigned char reg, unsigned char* data, unsigned int len)
{
        int ret = -1;
        struct i2c_client *client = akm4951->i2c_client;

        if (client == NULL) {
                printk("===>NOTE: %s error\n", __func__);
                return ret;
        }

        if(!data || len <= 0){
                printk("===>ERROR: %s\n", __func__);
                return ret;
        }

        mutex_lock(&akm4951->i2c_access_lock);
        ret = i2c_master_send(client, &reg, 1);
        if (ret < 1) {
                printk("%s 0x%02x err\n", __func__, reg);
                mutex_unlock(&akm4951->i2c_access_lock);
                return -1;
        }

        ret = i2c_master_recv(client, data, len);
        if (ret < 0)
                printk("%s 0x%02x err\n", __func__, reg);
        mutex_unlock(&akm4951->i2c_access_lock);

	return ret < len ? ret : 0;
}

/*
 * read akm4951 register cache
 */
static inline unsigned int akm4951_read_reg_cache(struct snd_soc_codec *codec,
							unsigned int reg)
{
	u8 *cache = codec->reg_cache;

        if (reg >= AKM4951_REGNUM)
                return 0;

        return cache[reg];
}

/*
 * write akm4951 register cache
 */
static inline void akm4951_write_reg_cache(struct snd_soc_codec *codec,
	unsigned int reg, unsigned char value)
{
	u8 *cache = codec->reg_cache;

        if (reg >= AKM4951_REGNUM)
                return;

        cache[reg] = value;
	return;
}

/*
 * read akm4951 real register
 */
static inline unsigned int akm4951_i2c_read(struct snd_soc_codec *codec,
							unsigned int reg)
{
	int ret = -1;
	unsigned char data;
	unsigned char reg_addr = (unsigned char)reg;

        if (reg >= AKM4951_REGNUM) {
		printk("===>NOTE: %s reg error\n", __func__);
                return 0;
	}

	ret = akm4951_i2c_read_reg(reg_addr, &data, 1);

	return ret != 0 ? 0 : data;
}

/*
 * write akm4951 register
 */
static int akm4951_i2c_write(struct snd_soc_codec *codec, unsigned int reg,
	unsigned int value)
{
	int ret =0;
	unsigned char data;
	data = (unsigned char)value;

	ret = akm4951_i2c_write_regs((unsigned char)reg, &data, 1);
	if (ret != 0)
		printk("%s i2c error \n", __func__);
	else
		akm4951_write_reg_cache(codec, reg, data);

	return 0;
}

static int codec_reset(void)
{
        int ret = 0;

        if (akm4951->control_pin->pdn->gpio != -1) {
		gpio_set_value(akm4951->control_pin->pdn->gpio, akm4951->control_pin->pdn->active_level);
		mdelay(20);
		gpio_set_value(akm4951->control_pin->pdn->gpio,!akm4951->control_pin->pdn->active_level);
		mdelay(1);
	}

        return ret;
}

static int ingenic_snd_soc_info_volsw_2r(struct snd_kcontrol *kcontrol,
		struct snd_ctl_elem_info *uinfo)
{
	struct soc_mixer_control *mc =
		(struct soc_mixer_control *)kcontrol->private_value;
	int platform_max;
	int min = mc->min;

	if (!mc->platform_max)
		mc->platform_max = mc->max;
	platform_max = mc->platform_max;

	if (platform_max == 1 && !strstr(kcontrol->id.name, " Volume"))
		uinfo->type = SNDRV_CTL_ELEM_TYPE_BOOLEAN;
	else
		uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;

	uinfo->count = 2;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = platform_max - min;
	return 0;
}

static int ingenic_snd_soc_get_volsw_2r(struct snd_kcontrol *kcontrol,
		struct snd_ctl_elem_value *ucontrol)
{
	struct soc_mixer_control *mc =
		(struct soc_mixer_control *)kcontrol->private_value;
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	unsigned int reg = mc->reg;
	unsigned int reg2 = mc->rreg;
	unsigned int shift = mc->shift;
	int max = mc->max;
	int min = mc->min;
	unsigned int mask = (1 << fls(max)) - 1;
	unsigned int invert = mc->invert;

	ucontrol->value.integer.value[0] =
		((snd_soc_read(codec, reg) >> shift) & mask) - min;
	ucontrol->value.integer.value[1] =
		((snd_soc_read(codec, reg2) >> shift) & mask) - min;
	if (invert) {
		ucontrol->value.integer.value[0] =
			max - ucontrol->value.integer.value[0] - min;
		ucontrol->value.integer.value[1] =
			max - ucontrol->value.integer.value[1] - min;
	}

	return 0;
}

static int ingenic_snd_soc_put_volsw_2r(struct snd_kcontrol *kcontrol,
        struct snd_ctl_elem_value *ucontrol)
{
	struct soc_mixer_control *mc =
		(struct soc_mixer_control *)kcontrol->private_value;
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	unsigned int reg = mc->reg;
	unsigned int reg2 = mc->rreg;
	unsigned int shift = mc->shift;
	int max = mc->max;
	int min = mc->min;
	unsigned int mask = (1 << fls(max)) - 1;
	unsigned int invert = mc->invert;
	int err;
	unsigned int val, val2, val_mask;

	val_mask = mask << shift;
	val = ((ucontrol->value.integer.value[0] + min) & mask);
	val2 = ((ucontrol->value.integer.value[1] + min) & mask);

	if (invert) {
		val = max - (val - min);
		val2 = max - (val2 - min);
	}

	val = val << shift;
	val2 = val2 << shift;

	err = snd_soc_update_bits_locked(codec, reg, val_mask, val);
	if (err < 0)
		return err;

	err = snd_soc_update_bits_locked(codec, reg2, val_mask, val2);
	return err;
}

#define INGENIC_SOC_DOUBLE_R_TLV(xname, reg_left, reg_right, xshift, xmin, xmax, xinvert, tlv_array) \
{       .iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = (xname),\
        .access = SNDRV_CTL_ELEM_ACCESS_TLV_READ |\
                 SNDRV_CTL_ELEM_ACCESS_READWRITE,\
        .tlv.p = (tlv_array), \
        .info = ingenic_snd_soc_info_volsw_2r, \
        .get = ingenic_snd_soc_get_volsw_2r, .put = ingenic_snd_soc_put_volsw_2r, \
        .private_value = (unsigned long)&(struct soc_mixer_control) \
                {.reg = reg_left, .rreg = reg_right, .shift = xshift, \
                .min = xmin, .max = xmax, .platform_max = xmax, .invert = xinvert} }

static const char *akm4951_program_filter_input_select[] = {"I2S IN", "LINE IN(ADC IN)"};
static const char *akm4951_dac_input_select[] = {"I2S DIN", "PFVOL IN", "(I2S + PFVOL)/2"};
static const char *akm4951_alc_switch[] = {"ALC DISABLE", "ALC ENABLE"};
static const char *akm4951_eq1_switch[] = {"DISABLE", "ENABLE"};
static const char *akm4951_eq2_switch[] = {"DISABLE", "ENABLE"};
static const char *akm4951_eq3_switch[] = {"DISABLE", "ENABLE"};
static const char *akm4951_eq4_switch[] = {"DISABLE", "ENABLE"};
static const char *akm4951_eq5_switch[] = {"DISABLE", "ENABLE"};
static const char *akm4951_wind_noise_reduct_switch[] = {"DISABLE", "ENABLE"};

static const struct soc_enum akm4951_enum[] = {
	SOC_ENUM_SINGLE(ALC_MODE_CONTROL, 5, 2, akm4951_alc_switch),
	SOC_ENUM_SINGLE(DIGITAL_FILTER_MODE, 1, 2, akm4951_program_filter_input_select),
	SOC_ENUM_SINGLE(DIGITAL_FILTER_MODE, 2, 3, akm4951_dac_input_select),
	SOC_ENUM_SINGLE(DIGITAL_FILTER_SELECT, 0, 2, akm4951_eq1_switch),
	SOC_ENUM_SINGLE(DIGITAL_FILTER_SELECT, 1, 2, akm4951_eq2_switch),
	SOC_ENUM_SINGLE(DIGITAL_FILTER_SELECT, 2, 2, akm4951_eq3_switch),
	SOC_ENUM_SINGLE(DIGITAL_FILTER_SELECT, 3, 2, akm4951_eq4_switch),
	SOC_ENUM_SINGLE(DIGITAL_FILTER_SELECT, 4, 2, akm4951_eq5_switch),
	SOC_ENUM_SINGLE(AUTO_HPF_CONTROL, 5, 2, akm4951_wind_noise_reduct_switch),
};

/* unit: 0.01dB */
#ifdef USE_ALL_GAIN_RANGE
/* If you want to use full gain range:-90dB ~ +12dB, you can enable the code */
static const DECLARE_TLV_DB_SCALE(dac_tlv, -9000, 50, 1);
#else
/* Here we just use -30dB ~ 0dB */
static const DECLARE_TLV_DB_SCALE(dac_tlv, -3000, 50, 0);
#endif

/* We use full gain range:-52.5dB ~ +36dB, but the real step is 0.375dB */
static const DECLARE_TLV_DB_SCALE(adc_tlv, -5250, 37, 1);

static const struct snd_kcontrol_new akm4951_snd_controls[] = {
#ifdef USE_ALL_GAIN_RANGE
	/* If you want to use full gain range:-90dB ~ +12dB, you can enable the code */
	SOC_DOUBLE_R_TLV("Master Playback Volume", LCH_DIGITAL_VOLUME, RCH_DIGITAL_VOLUME, 0, 0xcc, 1, dac_tlv),
#else
	INGENIC_SOC_DOUBLE_R_TLV("Master Playback Volume", LCH_DIGITAL_VOLUME, RCH_DIGITAL_VOLUME, 0, 0x18, 0x54, 1, dac_tlv),
#endif
	INGENIC_SOC_DOUBLE_R_TLV("Adc Volume", LCH_INPUT_VOLUME, RCH_INPUT_VOLUME, 0, 0x04, 0xf1, 0, adc_tlv),
	SOC_SINGLE("Digital Playback mute", MODE_CONTROL_3, 5, 1, 0),
	SOC_ENUM("Alc Switch", akm4951_enum[0]),
	SOC_ENUM("I2s And Linein Select", akm4951_enum[1]),
	SOC_ENUM("Eq1 Switch", akm4951_enum[3]),
	SOC_ENUM("Eq2 Switch", akm4951_enum[4]),
	SOC_ENUM("Eq3 Switch", akm4951_enum[5]),
	SOC_ENUM("Eq4 Switch", akm4951_enum[6]),
	SOC_ENUM("Eq5 Switch", akm4951_enum[7]),
	SOC_ENUM("Wind Noise Filter Switch", akm4951_enum[8]),
	//SOC_ENUM("Dac Input Signal Select", akm4951_enum[2]),
};

/* akm4951 dapm widgets */
static const struct snd_soc_dapm_widget akm4951_dapm_widgets[] = {
	/* akm4951 dapm route */
	SND_SOC_DAPM_DAC("DAC", "Playback", SND_SOC_NOPM, 0, 0),
	SND_SOC_DAPM_ADC("ADC", "Capture", SND_SOC_NOPM, 0, 0),
	SND_SOC_DAPM_OUTPUT("DAC OUT"),
	SND_SOC_DAPM_INPUT("ADC IN"),
};

static const struct snd_soc_dapm_route akm4951_audio_map[] = {
	/* ADC */
	{"ADC", NULL, "ADC IN"},
	{"DAC OUT", NULL, "DAC"},
};

static int akm4951_set_dai_sysclk(struct snd_soc_dai *codec_dai,
	int clk_id, unsigned int freq, int dir)
{
	akm4951->sysclk = freq;
	return 0;
}

static int akm4951_hw_params(struct snd_pcm_substream *substream,
			    struct snd_pcm_hw_params *params,
			    struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_codec *codec = rtd->codec;
	int rate = params_rate(params);
	int i;
        unsigned char data;
        unsigned long mrate[9] = {
                8000, 12000, 16000, 24000, 11025,
                22050, 32000, 48000, 44100
        };
        unsigned char reg[9] = {
                0, 1, 2, 9, 5,
                7, 10,11,15
        };
	/* set rate */
        for (i = 0; i < 9; i++)
                if (rate == mrate[i])
                        break;

	if (i == 9){
                printk("%d is not support, fix it to 48000\n", rate);
                rate = 48000;
                i = 7;
        }

        user_replay_rate = rate;
        akm4951_i2c_read_reg(0x06, &data, 1);
        data &= 0xf0;
        data |= reg[i];
        akm4951_i2c_write_regs(0x06, &data, 1);
        msleep(50);            // wait for akm4951 i2s clk stable.

	/* set fs */

	return 0;
}

static int akm4951_mute(struct snd_soc_dai *dai, int mute)
{
	if (!mute){

	} else {

	}
	return 0;
}

static int akm4951_set_bias_level(struct snd_soc_codec *codec,
	enum snd_soc_bias_level level)
{
	/* There is no bias level function in akm4951 */
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

#ifdef CONFIG_SND_ASOC_AKM4951_AEC_MODE
#define AKM4951_RATES   (SNDRV_PCM_RATE_48000)
#else
#define AKM4951_RATES   (SNDRV_PCM_RATE_8000_48000)
#endif
#define AKM4951_FORMATS (SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S24_LE)

static int akm4951_startup(struct snd_pcm_substream *substream, struct snd_soc_dai *dai)
{
	return 0;
}
static void akm4951_shutdown(struct snd_pcm_substream *substream, struct snd_soc_dai *dai)
{

}

static struct snd_soc_dai_ops akm4951_dai_ops = {
	.hw_params	= akm4951_hw_params,
	.digital_mute	= akm4951_mute,
	.set_sysclk	= akm4951_set_dai_sysclk,
	.startup	= akm4951_startup,
	.shutdown	= akm4951_shutdown,
};

static struct snd_soc_dai_driver akm4951_dai = {
	.name = "akm4951-dai",
	.playback = {
		.stream_name = "Playback",
		.channels_min = 1,
		.channels_max = 2,
		.rates = AKM4951_RATES,
		.formats = AKM4951_FORMATS,
	},
	.capture = {
		.stream_name = "Capture",
		.channels_min = 2,
		.channels_max = 2,
		.rates = AKM4951_RATES,
		.formats = AKM4951_FORMATS,
	},
	.ops = &akm4951_dai_ops,
};

static int akm4951_suspend(struct snd_soc_codec *codec, pm_message_t state)
{
	int ret = 0;
        unsigned char data;
        akm4951_i2c_read_reg(0x00, &data, 1);
        power_reg0 = data;
        data &= 0x40;
        ret |= akm4951_i2c_write_regs(0x00, &data, 1);
        msleep(5);
        akm4951_i2c_read_reg(0x01, &data, 1);
        power_reg1 = data;
        data &= 0xcd;
        ret |= akm4951_i2c_write_regs(0x01, &data, 1);
	return 0;
}

static int akm4951_resume(struct snd_soc_codec *codec)
{
	int ret = 0;
        unsigned char data;
        data = power_reg0;
        ret |= akm4951_i2c_write_regs(0x00, &data, 1);
        msleep(5);
        data = power_reg1;
        ret |= akm4951_i2c_write_regs(0x01, &data, 1);
        msleep(50);
        return 0;
}

static int codec_dac_setup(void)
{
	int i;
	int ret = 0;
	char data = 0x0;

	/* Init 0x02 ~ 0x4F registers */
	for (i = 0; i < sizeof(akm4951_registers) / sizeof(akm4951_registers[0]); i++){
		ret |= akm4951_i2c_write_regs(akm4951_registers[i][0], &akm4951_registers[i][1], 1);
	}

	/* Power on DAC ADC DSP */
	data = 0xc7;
	ret |= akm4951_i2c_write_regs(0x00, &data, 1);
	data = 0xbc;
	ret |= akm4951_i2c_write_regs(0x01, &data, 1);

	if (ret)
		printk("===>ERROR: akm4951 init error!\n");
	return ret;
}

static int codec_clk_setup(void)
{
	int ret = 0;
	unsigned char data = 0x0;
	/* I2S clk setup */
	data = 0x08;
	ret |= akm4951_i2c_write_regs(0x01, &data, 1);
	data = 0x7b;
	ret |= akm4951_i2c_write_regs(0x05, &data, 1);
	data = 0x0b;
	ret |= akm4951_i2c_write_regs(0x06, &data, 1);
	data = 0x40;
	ret |= akm4951_i2c_write_regs(0x00, &data, 1);
	mdelay(2);
	data = 0x0c;
	ret |= akm4951_i2c_write_regs(0x01, &data, 1);
	mdelay(5);

	return ret;
}

static int akm4951_probe(struct snd_soc_codec *codec)
{
	int ret = 0;
	/* akm4951 pdn pin reset */
	ret = codec_reset();
        if (ret < 0) {
                dev_err(codec->dev, "Failed to issue reset\n");
                return ret;
        }

	/* clk setup */
	ret |= codec_clk_setup();

	/* DAC outputs setup */
	ret |= codec_dac_setup();

	return ret;
}

/* power down chip */
static int akm4951_remove(struct snd_soc_codec *codec)
{
	int ret = 0;
	unsigned char data;
	/* Mute AMP pin */

	/* Power down akm4951 */
	akm4951_i2c_read_reg(0x01, &data, 1);
	data &= 0xfb;
	ret |= akm4951_i2c_write_regs(0x01, &data, 1);

	if (akm4951->control_pin->pdn->gpio != -1){
		gpio_set_value(akm4951->control_pin->pdn->gpio, akm4951->control_pin->pdn->active_level);
	}

	if (ret < 0) {
		printk("%s i2c write err\n", __func__);
		return ret;
	}
	return 0;
}

static struct snd_soc_codec_driver soc_codec_dev_akm4951 = {
	.probe =	akm4951_probe,
	.remove =	akm4951_remove,
	.suspend =	akm4951_suspend,
	.resume =	akm4951_resume,
	//.read = akm4951_read_reg_cache,
	.read =  akm4951_i2c_read,
	.write = akm4951_i2c_write,
	.set_bias_level = akm4951_set_bias_level,
	.reg_cache_size = AKM4951_REGNUM,
	.reg_word_size = sizeof(u8),
	//.reg_cache_default = akm4951_reg,
	.controls =     akm4951_snd_controls,
        .num_controls = ARRAY_SIZE(akm4951_snd_controls),
	.dapm_widgets = akm4951_dapm_widgets,
	.num_dapm_widgets = ARRAY_SIZE(akm4951_dapm_widgets),
	.dapm_routes = akm4951_audio_map,
	.num_dapm_routes = ARRAY_SIZE(akm4951_audio_map),
};

static __devinit int akm4951_i2c_probe(struct i2c_client *i2c,
				      const struct i2c_device_id *id)
{
	int ret = 0;
        struct device *dev = &i2c->dev;
        struct akm4951_platform_data *akm4951_data = dev->platform_data;

        if (!i2c_check_functionality(i2c->adapter, I2C_FUNC_I2C)) {
                return -ENODEV;
        }

        if (akm4951_data->pdn->gpio != -1) {
                ret = gpio_request_one(akm4951_data->pdn->gpio,
                                GPIOF_DIR_OUT, "akm4951_pdn");
                if (ret != 0) {
                        dev_err(dev, "Can't request pdn pin = %d\n",
                                        akm4951_data->pdn->gpio);
                        return ret;
                } else
			gpio_set_value(akm4951_data->pdn->gpio, akm4951_data->pdn->active_level);
        }

	akm4951 = kzalloc(sizeof(struct akm4951_priv), GFP_KERNEL);
	if (akm4951 == NULL)
		return -ENOMEM;

	akm4951->i2c_client   = i2c;
	akm4951->control_type = SND_SOC_I2C;
	akm4951->control_pin  = akm4951_data;

	mutex_init(&akm4951->i2c_access_lock);

	ret = snd_soc_register_codec(&i2c->dev, &soc_codec_dev_akm4951, &akm4951_dai, 1);
	if (ret < 0) {
		kfree(akm4951);
		dev_err(&i2c->dev, "Faild to register codec\n");
	}
	return ret;
}

static __devexit int akm4951_i2c_remove(struct i2c_client *client)
{
	struct device *dev = &client->dev;
        struct akm4951_platform_data *akm4951_data = dev->platform_data;

	if (akm4951_data->pdn->gpio != -1)
                gpio_free(akm4951_data->pdn->gpio);

	snd_soc_unregister_codec(&client->dev);

	kfree(akm4951);
	return 0;
}

static const struct i2c_device_id akm4951_i2c_id[] = {
	{ "akm4951", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, akm4951_i2c_id);

static struct i2c_driver akm4951_i2c_driver = {
	.driver = {
		.name = "akm4951",
		.owner = THIS_MODULE,
	},
	.probe =    akm4951_i2c_probe,
	.remove =   __devexit_p(akm4951_i2c_remove),
	.id_table = akm4951_i2c_id,
};

static int __init akm4951_modinit(void)
{
	int ret = 0;
	ret = i2c_add_driver(&akm4951_i2c_driver);
	if (ret != 0) {
		printk(KERN_ERR "Failed to register AKM4951 I2C driver: %d\n",
		       ret);
	}
	return ret;
}
module_init(akm4951_modinit);

static void __exit akm4951_exit(void)
{
	i2c_del_driver(&akm4951_i2c_driver);
}
module_exit(akm4951_exit);

MODULE_DESCRIPTION("Soc AKM4951 driver");
MODULE_AUTHOR("tjin<tao.jin@ingenic.com>");
MODULE_LICENSE("GPL");
