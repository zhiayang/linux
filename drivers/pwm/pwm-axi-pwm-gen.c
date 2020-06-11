#include <linux/bits.h>
#include <linux/clk.h>
#include <linux/platform_device.h>
#include <linux/pwm.h>
#include <linux/fpga/adi-axi-common.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/module.h>
#include <linux/err.h>

#define AXI_PWM_GEN_REG_CORE_VERSION	0x00
#define AXI_PWM_GEN_REG_ID		0x01
#define AXI_PWM_GEN_REG_SCRATCHPAD	0x02
#define AXI_PWM_GEN_REG_CORE_MAGIC	0x03
#define AXI_PWM_GEN_REG_CONFIG	0x04
#define AXI_PWM_GEN_REG_PULSE_PERIOD	0x05
#define AXI_PWM_GEN_REG_PULSE_WIDTH	0x06

#define AXI_LOAD_CONFIG			BIT(1)

struct axi_pwm_gen {
	struct pwm_chip		chip;
	struct clk		*clk;
	void __iomem		*base;

	unsigned int 		magic_number;
	unsigned int 		core_version;
	unsigned int		pwm_period;
	unsigned int		pwm_width;
};

static inline
struct axi_pwm_gen *to_axi_pwm_gen(struct pwm_chip *chip)
{
	return container_of(chip, struct axi_pwm_gen, chip);
}

static int axi_pwm_gen_apply(struct pwm_chip *chip, struct pwm_device *device,
			       struct pwm_state *state)
{
	struct axi_pwm_gen *pwmgen;
	unsigned long clk_rate, period_cnt, duty_cnt;

	pwmgen = to_axi_pwm_gen(chip);
	clk_rate = clk_get_rate(pwmgen->clk);

	period_cnt = DIV_ROUND_UP(clk_rate * state->period, NSEC_PER_SEC);
	writel(period_cnt, pwmgen->base + AXI_PWM_GEN_REG_PULSE_PERIOD);
	duty_cnt = DIV_ROUND_UP(clk_rate * state->duty_cycle, NSEC_PER_SEC);
	writel(duty_cnt, pwmgen->base + AXI_PWM_GEN_REG_PULSE_WIDTH);

	writel(AXI_LOAD_CONFIG, pwmgen->base + AXI_PWM_GEN_REG_CONFIG);

	return 0;
}

static void axi_pwm_gen_free(struct pwm_chip *chip, struct pwm_device *dev)
{
	struct axi_pwm_gen *pwmgen = pwm_get_chip_data(dev);

	clk_unprepare(pwmgen->clk);
}

static const struct pwm_ops axi_pwm_gen_pwm_ops = {
	.apply = axi_pwm_gen_apply,
	.free = axi_pwm_gen_free,
	.owner = THIS_MODULE,
};

static const struct of_device_id axi_pwm_gen_ids[] = {
	{
		.compatible = "adi,axi-pwm-gen",
	},
	{ },
};
MODULE_DEVICE_TABLE(of, axi_pwm_gen_ids);

static int axi_pwm_gen_probe(struct platform_device *pdev)
{
	int ret;
	struct axi_pwm_gen *pwmgen;
	struct resource *mem;

	pwmgen = devm_kzalloc(&pdev->dev, sizeof(*pwmgen), GFP_KERNEL);
	if (!pwmgen)
		return -ENOMEM;

	mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	pwmgen->base = devm_ioremap_resource(&pdev->dev, mem);
	if (IS_ERR(pwmgen->base))
		return PTR_ERR(pwmgen->base);

	pwmgen->clk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(pwmgen->clk))
		return PTR_ERR(pwmgen->clk);

	ret = clk_prepare(pwmgen->clk);
	if (ret) {
		return ret;
	}

	pwmgen->chip.dev = &pdev->dev;
	pwmgen->chip.ops = &axi_pwm_gen_pwm_ops;
	pwmgen->chip.npwm = 1;
	pwmgen->chip.base = readl(pwmgen->base +
				    AXI_PWM_GEN_REG_ID);
	pwmgen->core_version = readl(pwmgen->base +
				       AXI_PWM_GEN_REG_CORE_VERSION);

	ret = pwmchip_add(&pwmgen->chip);
	if (ret < 0)
		goto err_clk;

	platform_set_drvdata(pdev, pwmgen);

	return 0;


err_clk:
	clk_unprepare(pwmgen->clk);

	return ret;
}

static int axi_pwm_gen_remove(struct platform_device *pdev)
{
	struct axi_pwm_gen *pwmgen = platform_get_drvdata(pdev);

	return pwmchip_remove(&pwmgen->chip);
}

static struct platform_driver axi_pwm_gen_driver = {
	.driver = {
		.name = "adi-axi-pwm-gen",
		.of_match_table = axi_pwm_gen_ids,
	},
	.probe = axi_pwm_gen_probe,
	.remove = axi_pwm_gen_remove,
};

module_platform_driver(axi_pwm_gen_driver);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Sergiu Cuciurean <sergiu.cuciurean@analog>");
MODULE_DESCRIPTION("Driver for the Analog Devices' AXI PWM generator");
