#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/display.h>
#include <zephyr/sys/mem_manage.h>
#include <zephyr/drivers/gpio.h>
#include "lcd.h"

LOG_MODULE_REGISTER(lcd, LOG_LEVEL_INF);

void lcd_fresh(uint16_t color);
void lcd_ctrl(void *arg1, void *arg2, void *arg3)
{
	uint16_t color = 0;
	while (1) 
	{
		color++;
		if (color > 0xFFFF)
			color = 0;

		lcd_fresh(color);
		k_msleep(10);
	}
}
K_THREAD_STACK_DEFINE(lcd_stack_area, 512); // 根据堆栈大小定义堆栈
struct k_thread lcd_thread_data;            // 线程结构块

#define LCD_NODE DT_NODELABEL(lcd)
const struct device *display_dev = DEVICE_DT_GET(LCD_NODE);

#define LCD_LED_NODE DT_NODELABEL(lcd_led)
static const struct gpio_dt_spec lcd_led = GPIO_DT_SPEC_GET(LCD_LED_NODE, gpios);

uint8_t *lcd_buf;
void lcd_init(void)
{
	if (!device_is_ready(display_dev)) {
		LOG_INF("ST7789V device not ready!");
		return;
	}
	if (!gpio_is_ready_dt(&lcd_led)) {
		printk("lcd_led device is not ready");
		return;
	}
	gpio_pin_configure_dt(&lcd_led, GPIO_OUTPUT_ACTIVE); // 设置LCD背光

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
	lcd_buf = (uint8_t *)k_malloc(desc.buf_size);
	if (!lcd_buf) {
		LOG_INF("No memory for display buffer!");
		return;
	}

	// 填充红色
	for (int i = 0; i < width * height; i++) {
		lcd_buf[2 * i] = color >> 8;
		lcd_buf[2 * i + 1] = color & 0xFF;
	}

	// 显示缓冲区
	display_write(display_dev, 0, 0, &desc, lcd_buf);

	LOG_INF("ST7789V demo finished!");

	k_thread_create(&lcd_thread_data, lcd_stack_area, K_THREAD_STACK_SIZEOF(lcd_stack_area),
			lcd_ctrl, NULL, NULL, NULL, 5, 0, K_NO_WAIT);
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

	// 填充红色
	for (int i = 0; i < width * height; i++) {
		lcd_buf[2 * i] = color >> 8;
		lcd_buf[2 * i + 1] = color & 0xFF;
	}

	// 显示缓冲区
	display_write(display_dev, 0, 0, &desc, lcd_buf);

	// LOG_INF("lcd fresh color: 0x%04X\n", color);
}
