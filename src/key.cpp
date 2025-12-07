#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>
#include <zephyr/input/input.h>

LOG_MODULE_REGISTER(key, LOG_LEVEL_INF);

/* 按键事件回调函数 */
static void input_cb(struct input_event *evt, void *user_data) {
    LOG_INF("key evt: %d, %d", evt->code, evt->value);
    if (evt->value == 1) {
        switch (evt->code) {
            case INPUT_KEY_1:
                LOG_INF("press INPUT_KEY_1");
                break;
            case INPUT_KEY_2:
                LOG_INF("press INPUT_KEY_2");
                break;
            default:
                /* 忽略其他按键事件 */
                break;
        }
    } else if (evt->value == 0) {
        switch (evt->code) {
            case INPUT_KEY_1:
                LOG_INF("release INPUT_KEY_1");
                break;
            case INPUT_KEY_2:
                LOG_INF("release INPUT_KEY_2");
                break;
            default:
                /* 忽略其他按键事件 */
                break;
        }
    }
}
// 注册按键回调
INPUT_CALLBACK_DEFINE(NULL, input_cb, NULL);