#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/input/input.h>

static const struct device *const *longpress_dev = DEVICE_DT_GET(DT_NODELABEL(longpress));
static const struct device *const *button_1_dev = DEVICE_DT_GET(DT_NODELABEL(button_1));
// const struct device *button_2_dev = DEVICE_DT_GET(DT_NODELABEL(button_2));

/* 输入事件回调函数 */
static void input_cb(struct input_event *evt, void *user_data)
{
	switch (evt->code) {
	case INPUT_KEY_A:
		if (evt->value) {
			printk("短按事件 INPUT_KEY_A\n");
		}
		break;
	case INPUT_KEY_X:
		if (evt->value) {
			printk("长按事件 INPUT_KEY_X\n");
		}
		break;

	case INPUT_KEY_B:
		if (evt->value) {
			printk("短按事件 INPUT_KEY_B\n");
		}
		break;
	case INPUT_KEY_Y:
		if (evt->value) {
			printk("长按事件 INPUT_KEY_Y\n");
		}
		break;

	default:
		/* 忽略其他按键事件 */
		break;
	}
}

INPUT_CALLBACK_DEFINE(longpress_dev, input_cb, NULL);

int key_init(void)
{
	// 检查设备是否就绪
	if (!device_is_ready(longpress_dev)) {
		printk("KEY device is not ready\n");
		return -ENODEV;
	}
    return 0;
}
