 /*
 * Copyright (C) 2014 Ingenic Semiconductor Co., Ltd.
 *	http://www.ingenic.com
 * Author: tjin <tao.jin@ingenic.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <sound/soc.h>
#include <sound/jack.h>
#include <linux/gpio.h>
#include "../codec/icdc_d3.h"

#define M1_SPK_GPIO	-1
#define M1_SPK_EN	1

unsigned long codec_sysclk = 24000000;
static int m1_spk_power(struct snd_soc_dapm_widget *w,
				struct snd_kcontrol *kcontrol, int event)
{
	if (SND_SOC_DAPM_EVENT_ON(event)) {
		if (M1_SPK_GPIO != -1){
			gpio_direction_output(M1_SPK_GPIO, M1_SPK_EN);
			printk("gpio speaker enable %d\n", gpio_get_value(M1_SPK_GPIO));
		}
	} else {
		if (M1_SPK_GPIO != -1){
			gpio_direction_output(M1_SPK_GPIO, !M1_SPK_EN);
			printk("gpio speaker disable %d\n", gpio_get_value(M1_SPK_GPIO));
		}
	}
	return 0;
}

void m1_spk_sdown(struct snd_pcm_substream *sps)
{
	if (M1_SPK_GPIO != -1)
		gpio_direction_output(M1_SPK_GPIO, !M1_SPK_EN);
	return;
}

int m1_spk_sup(struct snd_pcm_substream *sps)
{
	if (M1_SPK_GPIO != -1)
		gpio_direction_output(M1_SPK_GPIO, M1_SPK_EN);
	return 0;
}

static struct snd_soc_ops m1_i2s_ops = {
	.startup = m1_spk_sup,
	.shutdown = m1_spk_sdown,
};
static const struct snd_soc_dapm_widget m1_dapm_widgets[] = {
	SND_SOC_DAPM_HP("Headphone Jack", NULL),
	SND_SOC_DAPM_SPK("Speaker", m1_spk_power),
	SND_SOC_DAPM_MIC("Mic", NULL),
};

static struct snd_soc_jack canna_icdc_d3_hp_jack;
static struct snd_soc_jack_pin canna_icdc_d3_hp_jack_pins[] = {
	{
		.pin = "Headphone Jack",
		.mask = SND_JACK_HEADPHONE,
	},
};

static struct snd_soc_jack_gpio canna_icdc_d3_jack_gpio[] = {
	{
		.name = "Headphone detection",
		.report = SND_JACK_HEADPHONE,
		.debounce_time = 150,
	}
};

/* m1 machine audio_map */
static const struct snd_soc_dapm_route audio_map[] = {
	/* ext speaker connected to DAC out */
	{"Speaker", NULL, "DAC OUT"},

	/* mic is connected to ADC in */
	{"Mic", NULL, "ADC IN"},

};

static int m1_dlv_dai_link_init(struct snd_soc_pcm_runtime *rtd)
{
	struct snd_soc_codec *codec = rtd->codec;
	struct snd_soc_dapm_context *dapm = &codec->dapm;
	int err;

	err = snd_soc_dapm_new_controls(dapm, m1_dapm_widgets,
			ARRAY_SIZE(m1_dapm_widgets));
	if (err){
		printk("m1 dai add controls err!!\n");
		return err;
	}

	/* Set up specific audio path audio_mapnects */
	err = snd_soc_dapm_add_routes(dapm, audio_map,
			ARRAY_SIZE(audio_map));
	if (err){
		printk("add m1 dapm routes err!!\n");
		return err;
	}

	snd_soc_jack_new(codec, "Headset Jack", SND_JACK_HEADSET, &canna_icdc_d3_hp_jack);
	snd_soc_jack_add_pins(&canna_icdc_d3_hp_jack,ARRAY_SIZE(canna_icdc_d3_hp_jack_pins),canna_icdc_d3_hp_jack_pins);
#ifdef HAVE_HEADPHONE
	if (gpio_is_valid(DORADO_HP_DET)) {
		canna_icdc_d3_jack_gpio[jack].gpio = PHOENIX_HP_DET;
		canna_icdc_d3_jack_gpio[jack].invert = !PHOENIX_HP_DET_LEVEL;
		snd_soc_jack_add_gpios(&canna_icdc_d3_hp_jack, 1, canna_icdc_d3_jack_gpio);
	}
#else
	snd_soc_dapm_disable_pin(dapm, "Headphone Jack");
#endif

	snd_soc_dapm_force_enable_pin(dapm, "Speaker");
	snd_soc_dapm_force_enable_pin(dapm, "Mic");

	snd_soc_dapm_sync(dapm);
	return 0;
}

static struct snd_soc_dai_link m1_dais[] = {
	[0] = {
		.name = "M1 npca110p",
		.stream_name = "M1 npca110p",
		.platform_name = "jz-asoc-aic-dma",
		.cpu_dai_name = "jz-asoc-aic-i2s",
		.init = m1_dlv_dai_link_init,
		.codec_dai_name = "npca110p-dai",
		.codec_name = "npca110p.2-0073",
		.ops = &m1_i2s_ops,
	},
	[1] = {
		.name = "M1 PCMBT",
		.stream_name = "M1 PCMBT",
		.platform_name = "jz-asoc-pcm-dma",
		.cpu_dai_name = "jz-asoc-pcm",
		.codec_dai_name = "pcm dump dai",
		.codec_name = "pcm dump",
	},
	[2] = {
		.name = "M1 DMIC",
		.stream_name = "M1 DMIC",
		.platform_name = "jz-asoc-dmic-dma",
		.cpu_dai_name = "jz-asoc-aic-dmic",
		.codec_dai_name = "dmic dump dai",
		.codec_name = "dmic dump",
	},

};

static struct snd_soc_card m1 = {
	.name = "m1",
	.owner = THIS_MODULE,
	.dai_link = m1_dais,
	.num_links = ARRAY_SIZE(m1_dais),
};

static struct platform_device *m1_snd_device;

static int m1_init(void)
{
	/*struct jz_aic_gpio_func *gpio_info;*/
	int ret;
	printk("===>%s\n",__func__);
	if (M1_SPK_GPIO != -1) {
		ret = gpio_request(M1_SPK_GPIO, "Speaker_en");
		if (ret)
			return ret;
	}
	m1_snd_device = platform_device_alloc("soc-audio", -1);
	if (!m1_snd_device) {
		if (M1_SPK_GPIO != -1)
			gpio_free(M1_SPK_GPIO);
		return -ENOMEM;
	}

	platform_set_drvdata(m1_snd_device, &m1);
	ret = platform_device_add(m1_snd_device);
	if (ret) {
		platform_device_put(m1_snd_device);
		if (M1_SPK_GPIO != -1)
			gpio_free(M1_SPK_GPIO);
	}
	printk("===>%s,init ok!!!\n",__func__);
	dev_info(&m1_snd_device->dev, "Alsa sound card:m1 init ok!!!\n");
	return ret;
}

static void m1_exit(void)
{
	platform_device_unregister(m1_snd_device);
	if (M1_SPK_GPIO != -1)
		gpio_free(M1_SPK_GPIO);
}

module_init(m1_init);
module_exit(m1_exit);

MODULE_AUTHOR("tjin<tao.jin@ingenic.com>");
MODULE_DESCRIPTION("ALSA SoC M1 Snd Card");
MODULE_LICENSE("GPL");
