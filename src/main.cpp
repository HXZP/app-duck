#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/sys/mem_manage.h>
#include <zephyr/drivers/gpio.h>
#include "led.h"
// #include "key.h"

#define LCD_NODE DT_NODELABEL(lcd)
const struct device *display_dev = DEVICE_DT_GET(LCD_NODE);
#define LCD_LED_NODE DT_NODELABEL(lcd_led)
static const struct gpio_dt_spec lcd_led = GPIO_DT_SPEC_GET(LCD_LED_NODE, gpios);
void st7789v_demo(void)
{
	if (!device_is_ready(display_dev)) {
		printk("ST7789V device not ready!\n");
		return;
	}
	if (!gpio_is_ready_dt(&lcd_led)) {
		printk("lcd_led device is not ready\n");
		return;
	}
	gpio_pin_configure_dt(&lcd_led, GPIO_OUTPUT_ACTIVE);
	// 清屏为黑色
	display_blanking_off(display_dev);

	struct display_buffer_descriptor desc;
	uint16_t width = 135;
	uint16_t height = 100;
	uint16_t color = 0xF800; // 红色，RGB565

	// 填充一块红色区域
	desc.width = width;
	desc.height = height;
	desc.pitch = width;
	desc.buf_size = width * height * 2;

	// 分配缓冲区
	uint8_t *buf = (uint8_t *)k_malloc(desc.buf_size);
	if (!buf) {
		printk("No memory for display buffer!\n");
		return;
	}

	// 填充红色
	for (int i = 0; i < width * height; i++) {
		buf[2 * i] = color >> 8;
		buf[2 * i + 1] = color & 0xFF;
	}

	// 显示缓冲区
	display_write(display_dev, 0, 0, &desc, buf);

	k_free(buf);
	printk("ST7789V demo finished!\n");
}

void lcd_fresh(uint16_t color)
{
	uint16_t width = 135;
	uint16_t height = 100;

	struct display_buffer_descriptor desc;
	desc.width = width;
	desc.height = height;
	desc.pitch = width;
	desc.buf_size = width * height * 2;
	// 分配缓冲区
	uint8_t *buf = (uint8_t *)k_malloc(desc.buf_size);
	if (!buf) {
		printk("No memory for display buffer!\n");
		return;
	}

	// 填充红色
	for (int i = 0; i < width * height; i++) {
		buf[2 * i] = color >> 8;
		buf[2 * i + 1] = color & 0xFF;
	}

	// 显示缓冲区
	display_write(display_dev, 0, 0, &desc, buf);

	k_free(buf);
}

int main()
{
	uint16_t color = 0;
	int ret = led_init();
	if (ret != 0) {
		printk("Failed to initialize LEDs: %d\n", ret);
		return ret;
	}

	printk("main thread started.\n");
	st7789v_demo();
	
	while (true) 
	{
		color++;
		if (color > 0xFFFF)
			color = 0;

		lcd_fresh(color); // 红色
		k_msleep(100);
	}
}
