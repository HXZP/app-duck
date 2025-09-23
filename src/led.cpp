#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include "led.h"

#define LED1_NODE DT_NODELABEL(led_1)
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);

#define LED2_NODE DT_NODELABEL(led_2)
static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(LED2_NODE, gpios);

#define LED3_NODE DT_NODELABEL(led_3)
static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(LED3_NODE, gpios);

void led_ctrl(void *arg1, void *arg2, void *arg3)
{
	while (1) {
		led_toggle(1);
		k_msleep(100);

		led_toggle(2);
		k_msleep(100);

		led_toggle(3);
		k_msleep(100);
	}
}
K_THREAD_STACK_DEFINE(led_stack_area, 512); // 根据堆栈大小定义堆栈
struct k_thread led_thread_data;            // 线程结构块


int led_init(void)
{
	// 检查设备是否就绪
	if (!gpio_is_ready_dt(&led1)) {
		printk("LED device is not ready\n");
		return -ENODEV;
	}
	// 检查设备是否就绪
	if (!gpio_is_ready_dt(&led2)) {
		printk("LED device is not ready\n");
		return -ENODEV;
	}
	// 检查设备是否就绪
	if (!gpio_is_ready_dt(&led3)) {
		printk("LED device is not ready\n");
		return -ENODEV;
	}

	// 配置GPIO引脚
	int ret = gpio_pin_configure_dt(&led1, GPIO_OUTPUT_ACTIVE);
	ret = gpio_pin_configure_dt(&led2, GPIO_OUTPUT_ACTIVE);
	ret = gpio_pin_configure_dt(&led3, GPIO_OUTPUT_ACTIVE);

	if (ret != 0) {
		printk("Failed to configure LED GPIO pin: %d\n", ret);
		return ret;
	}

	// 创建线程
	k_thread_create(&led_thread_data, led_stack_area, K_THREAD_STACK_SIZEOF(led_stack_area),
			led_ctrl, NULL, NULL, NULL, 5, 0, K_NO_WAIT);

	return ret;
}

void led_toggle(int led_num)
{
    switch (led_num) {
        case 1:
            gpio_pin_toggle_dt(&led1);
            break;
        case 2:
            gpio_pin_toggle_dt(&led2);
            break;
        case 3:
            gpio_pin_toggle_dt(&led3);
            break;
        default:
            printk("Invalid LED number: %d\n", led_num);
            break;
    }
}



