/* 文件：main.c */
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/can.h>
#include <zephyr/input/input.h>

#include "led.h"
#include "key.h"
#include "lcd.h"


LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

const struct device *const can_dev = DEVICE_DT_GET(DT_NODELABEL(can1));

/* CAN 接收回调函数 */
static void can_rx_callback(const struct device *dev, struct can_frame *frame, void *user_data) {
    LOG_INF("rx ID: 0x%x, len: %d", frame->id, frame->dlc);
    for (int i = 0; i < frame->dlc; i++) {
        LOG_INF("data[%d]: 0x%x", i, frame->data[i]);
    }
}

int main() {


	led_init();
	// key_init();
	lcd_init();

    /* 1. 检查CAN设备是否就绪 */
    if (!device_is_ready(can_dev)) {
        return 0;
    }

    /* 2. 配置CAN位时序（1Mbps示例，基于36MHz APB1时钟） */
    struct can_timing timing;
    timing.sjw = 1;//CAN_SJW_1TQ这个参数是错误的，实际值是0，不在最小和最大值的范围内
    timing.prop_seg = 0;
    timing.phase_seg1 = 6;  // 对于1Mbps，APB1=36MHz
    timing.phase_seg2 = 2;
    timing.prescaler = 8;
    
	// const struct can_timing *min = can_get_timing_min(can_dev);
	// const struct can_timing *max = can_get_timing_max(can_dev);
    // LOG_INF("CAN timing: sjw=%d, prop_seg=%d, phase_seg1=%d, phase_seg2=%d, prescaler=%d",
    //     min->sjw, min->prop_seg, min->phase_seg1, min->phase_seg2, min->prescaler);

    // LOG_INF("CAN timing: sjw=%d, prop_seg=%d, phase_seg1=%d, phase_seg2=%d, prescaler=%d",
    //     max->sjw, max->prop_seg, max->phase_seg1, max->phase_seg2, max->prescaler);

    int ret = can_set_timing(can_dev, &timing);
    if(ret)
    {
        LOG_ERR("can_set_timing failed, %d",ret);
    }
    can_set_mode(can_dev, CAN_MODE_NORMAL); // CAN_MODE_NORMAL
    can_start(can_dev);

    /* 3. 设置CAN接收过滤器（匹配所有标准帧） */
    struct can_filter filter = {
        .id = 0,
        .mask = 0,
        .flags = 0,
    };
    can_add_rx_filter(can_dev, can_rx_callback, NULL, &filter);

    /* 4. 定义要发送的CAN帧 */
    struct can_frame frame = {
        .id = 0x201,
        .dlc = 4,
        .flags = 0,
        .data = {0x01, 0x02, 0x03, 0x04}
    };

    LOG_INF("main thread started.");
    while (1) {
        /* 循环递增帧ID */
        if (frame.id < 0x300) {
            frame.id++;
        } else {
            frame.id = 0x201;
        }

        /* 发送CAN帧 */
        can_send(can_dev, &frame, K_NO_WAIT, NULL, NULL);
        k_msleep(1000);
    }
}