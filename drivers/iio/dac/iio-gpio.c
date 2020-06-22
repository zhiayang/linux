// SPDX-License-Identifier: GPL-2.0-only
/*
 * Analog Devices IIO_GPIO
 * Digital to Analog Converters driver
 *
 * Copyright 2019 Analog Devices Inc.
 */

#include <linux/device.h>
#include <linux/module.h>
#include <linux/iio/iio.h>
#include <linux/platform_device.h>
#include <linux/gpio/consumer.h>

#define MAX_CHANNELS 2
struct iio_gpio_state {
	struct platform_device 	*pdev;
	struct gpio_desc	*gpio_in[MAX_CHANNELS];
	struct gpio_desc	*gpio_out[MAX_CHANNELS];
	unsigned int		in_gpio_no;
	const char 		*in_gpio_names[MAX_CHANNELS];
	const char 		*in_gpio_numbers[MAX_CHANNELS];
	unsigned int		out_gpio_no;
	const char 		*out_gpio_names[MAX_CHANNELS];
	const char 		*out_gpio_numbers[MAX_CHANNELS];
};

static int iio_gpio_read_raw(struct iio_dev *indio_dev,
	const struct iio_chan_spec *chan, int *val, int *val2, long info)
{
	struct iio_gpio_state *st = iio_priv(indio_dev);

	
	switch (info) {
	case IIO_CHAN_INFO_RAW:
		*val = gpiod_get_value_cansleep(st->gpio_out[chan->channel]);
		printk("iio_gpio_read_raw val: %d\n", *val);
 		return IIO_VAL_INT;
	default:
		return -EINVAL;
	}
}

static int iio_gpio_write_raw(struct iio_dev *indio_dev,
			    struct iio_chan_spec const *chan,
			    int val,
			    int val2,
			    long info)
{
	struct iio_gpio_state *st = iio_priv(indio_dev);

	printk("iio_gpio_write_raw val: %d val2: %d\n", val, val2);
	
	switch (info) {
	case IIO_CHAN_INFO_RAW:
		gpiod_set_value_cansleep(st->gpio_out[chan->channel], val);
		return 0;
	default:
		return -EINVAL;
	}
}

 #define IIO_GPIO_CHANNEL(idx, out)					\
	{								\
		.type = IIO_VOLTAGE,					\
		.indexed = 1,						\
		.channel = idx,						\
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW),		\
		.output = out,						\
	}

 static const struct iio_chan_spec iio_gpio_channels[MAX_CHANNELS * 2] = {
	IIO_GPIO_CHANNEL(0, 1),
	IIO_GPIO_CHANNEL(1, 1),
	IIO_GPIO_CHANNEL(2, 0),
	IIO_GPIO_CHANNEL(3, 0),
};

static const struct iio_info iio_gpio_info = {
	.read_raw = &iio_gpio_read_raw,
	.write_raw = &iio_gpio_write_raw,
};

 static int iio_gpio_setup(struct iio_gpio_state *st)
{
	return 0;
}

static int iio_gpio_parse_dt(struct iio_gpio_state *st)
{
	int ret;

	ret = device_property_read_string_array(&st->pdev->dev,
					"input-gpio-names",
					st->in_gpio_names,
					2);
	if (ret < 0)
		return ret;
	printk("gpio_names: %s %s\n", st->in_gpio_names[0], st->in_gpio_names[1]);

	ret = device_property_read_string_array(&st->pdev->dev,
					"out-gpio-names",
					st->out_gpio_names,
					2);
	if (ret < 0)
		return ret;
	printk("gpio_names: %s %s\n", st->out_gpio_names[0], st->out_gpio_names[1]);


	ret = device_property_read_string_array(&st->pdev->dev,
					"input-gpio",
					st->in_gpio_numbers,
					2);
	// if (ret < 0)
	// 	return ret;
	printk("gpio_names: %s %s\n", st->in_gpio_numbers[0], st->in_gpio_numbers[1]);

	ret = device_property_read_string_array(&st->pdev->dev,
					"out-gpio",
					st->out_gpio_numbers,
					2);
	// if (ret < 0)
	// 	return ret;
	printk("gpio_names: %s %s\n", st->out_gpio_numbers[0], st->out_gpio_numbers[1]);

	return 0;
}

static int iio_gpio_probe(struct platform_device *pdev)
{
	struct iio_dev *indio_dev;
	struct iio_gpio_state *st;
	int ret, i;

	printk("iio-gpio_probe testn\n");
 	indio_dev = devm_iio_device_alloc(&pdev->dev, sizeof(*st));
	if (!indio_dev)
		return -ENOMEM;

	st = iio_priv(indio_dev);
	st->pdev = pdev;
	indio_dev->dev.parent = &pdev->dev;
	indio_dev->name = "iio-gpio";
	indio_dev->modes = INDIO_DIRECT_MODE;
	indio_dev->channels = iio_gpio_channels;
	indio_dev->num_channels = ARRAY_SIZE(iio_gpio_channels);
	indio_dev->info = &iio_gpio_info;


	// st->gpio_out[0] = devm_gpiod_get_optional(&st->pdev->dev, "out",
	// 					GPIOD_OUT_HIGH);
	// if (IS_ERR(st->gpio_in[0]))
	// 	return PTR_ERR(st->gpio_in[0]);
	st->out_gpio_no = 0;
	for (i = 0; i < MAX_CHANNELS; i++) {
		st->gpio_out[i] = devm_gpiod_get_index_optional(&st->pdev->dev,
					"out", i, GPIOD_OUT_HIGH);
		if (IS_ERR(st->gpio_out[i]))
			break;
			
		st->out_gpio_no++;
	}
	
	st->in_gpio_no = 0;
	for (i = 0; i < MAX_CHANNELS; i++) {
		st->gpio_in[i] = devm_gpiod_get_index_optional(&st->pdev->dev, 
					"in", i, GPIOD_IN);
		if (IS_ERR(st->gpio_in[i]))
			break;

		st->in_gpio_no++;
	}

	ret = iio_gpio_parse_dt(st);
	if (ret < 0)
		return ret;

 	ret = iio_gpio_setup(st);
	if (ret)
		return ret;

	platform_set_drvdata(pdev, indio_dev);
	return devm_iio_device_register(&pdev->dev, indio_dev);
}

static const struct of_device_id iio_gpio_dt_match[] = {
	{ .compatible = "adi,iio-gpio" },
	{},
};

MODULE_DEVICE_TABLE(of, iio_gpio_dt_match);

static struct platform_driver iio_gpio_driver = {
	.driver = {
		.name = "iio-gpio",
		.of_match_table = iio_gpio_dt_match,
	},
	.probe = iio_gpio_probe,
};

module_platform_driver(iio_gpio_driver);

MODULE_AUTHOR("Cristian Pop <cristian.pop@analog.com>");
MODULE_DESCRIPTION("Analog Devices IIO GPIO");
MODULE_LICENSE("GPL v2");
