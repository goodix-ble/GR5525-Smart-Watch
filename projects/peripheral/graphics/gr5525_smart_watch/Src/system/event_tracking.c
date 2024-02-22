#include "event_tracking.h"

#if EVENT_TRACKING_POINT_MAX_CNT > 0
#include "gr55xx_hal_def.h"
#include "gr55xx_ll_rtc.h"
#include "system_gr55xx.h"
#include "gr55xx_hal.h"
// #include "app_io.h"

static struct event_tracking_t
{
    int disabled;
    int cnt;
    int idx;
    struct tracking_t
    {
        uint32_t tick;
        const char *p_tag;
    } trackings[EVENT_TRACKING_POINT_MAX_CNT];
} __s_event_tracking;

/**
 * @brief 96MHz主频下，该函数需要1.9us
 *
 * @param tag
 */
__attribute__((section("RAM_CODE"))) void _event_tracking_create_tag(const char *tag)
{
    // app_io_init_t io_init =
    // {
    //     .pin = APP_IO_PIN_8,
    //     .mode = APP_IO_MODE_OUT_PUT,
    //     .pull = APP_IO_PULLUP,
    //     .mux = APP_IO_MUX_7,
    // };
    // app_io_init(APP_IO_TYPE_GPIOA, &io_init);
    // app_io_write_pin(APP_IO_TYPE_GPIOA, APP_IO_PIN_8, APP_IO_PIN_SET);

    int i;
    int disabled;

    // 临界区保护
    GLOBAL_EXCEPTION_DISABLE();
    disabled = __s_event_tracking.disabled;
    if (disabled == 0)
    {
        i = __s_event_tracking.idx;
        __s_event_tracking.idx = i + 1;
        if (__s_event_tracking.idx >= EVENT_TRACKING_POINT_MAX_CNT)
        {
            __s_event_tracking.idx = 0;
        }
        int cnt = __s_event_tracking.cnt;
        if (cnt < EVENT_TRACKING_POINT_MAX_CNT)
        {
            __s_event_tracking.cnt = cnt + 1;
        }
    }
    GLOBAL_EXCEPTION_ENABLE();

    if (disabled)
    {
        return;
    }

    // 获取时间
    uint32_t counter = ll_rtc_get_read_counter(RTC1);

    // 存储数据
    __s_event_tracking.trackings[i].tick = counter;
    __s_event_tracking.trackings[i].p_tag = tag;
    // app_io_write_pin(APP_IO_TYPE_GPIOA, APP_IO_PIN_8, APP_IO_PIN_RESET);
}

#endif

void event_tracking_dump(int (*p_printf)(const char *, ...))
{
    if (p_printf == NULL)
    {
        p_printf = printf;
        if (p_printf == NULL)
        {
            return;
        }
    }

    p_printf("\n-----DUMP-START-----\n");

#if EVENT_TRACKING_POINT_MAX_CNT > 0

    // 先禁止新的数据写入
    __s_event_tracking.disabled = 1;

    // 算出从哪个位置开始输出
    int cnt = __s_event_tracking.cnt;
    int idx = __s_event_tracking.idx;
    if (idx < cnt)
    {
        idx = idx + EVENT_TRACKING_POINT_MAX_CNT - cnt;
    }
    else
    {
        idx = idx - cnt;
    }

    // 输出时钟的分辨率
    uint32_t freq = SystemSlowClock;
    p_printf("Tick Freq(Hz), %d\n", freq);
    p_printf("Count, %d\n", __s_event_tracking.cnt);

#if 0
    p_printf("     TICK,    DELTA,  DELTA(us), TAG\n");
    // 导出数据，简单计算时间差
    uint32_t tick = __s_event_tracking.trackings[idx].tick;
    float unit = 1000000.0f / freq;
    for (int i = cnt; i != 0; i--)
    {
        struct tracking_t *p_tracking = &__s_event_tracking.trackings[idx];

        uint32_t delta = p_tracking->tick - tick;
        float delta_us = unit * delta;
        tick = p_tracking->tick;

        p_printf("%8d, %8d, %10.3f, %s\n", tick, delta, delta_us, p_tracking->p_tag);

        idx++;
        if (idx >= EVENT_TRACKING_POINT_MAX_CNT)
        {
            idx = 0;
        }
    }
#else
    p_printf("    TICK,    DELTA,  DELTA(us),   CALL(us), TAG\n");
    // 导出数据，简单计算时间差，然后统计函数的调用情况，支持配对。
    uint32_t tick = __s_event_tracking.trackings[idx].tick; // 可用于记录上一次的时间，以便计算当前记录和上一条记录的时间差

    float unit = 1000000.0f / freq;

    const int MAX_STACK = 20;
    const int TAB_SIZE = 2;
    struct tracking_t *stack[MAX_STACK];
    char spaces[MAX_STACK * TAB_SIZE + 1];
    int stack_pos = 0;
    spaces[0] = 0; // 先确保缩进为空
    for (int i = cnt; i != 0; i--)
    {
        struct tracking_t *p_tracking = &__s_event_tracking.trackings[idx];

        uint32_t delta = p_tracking->tick - tick;
        float delta_us = unit * delta;
        const char *p_tag = p_tracking->p_tag;
        tick = p_tracking->tick;

        unsigned short header = *((unsigned short *)p_tag);
        float call_delta_us = 0.0f;
        if (header == 0x3E3E) // ">>"
        {
            // 提级前先打印
            p_printf("%8d, %8d, %10.3f, %10.3f, %s%s\n", tick, delta, delta_us, call_delta_us, spaces, p_tag);
            if (stack_pos < MAX_STACK)
            {
                stack[stack_pos++] = p_tracking;
                int pos = stack_pos * TAB_SIZE;
                spaces[pos] = 0;
                for (int i = TAB_SIZE; i != 0; i--)
                {
                    pos--;
                    spaces[pos] = ' ';
                }
            }
        }
        else if (header == 0x5B5B) // "[["
        {
            if (stack_pos > 0)
            {
                stack_pos--;
                call_delta_us = unit * (tick - stack[stack_pos]->tick); // 算出与之前的级别匹配的点的时间差
                int pos = stack_pos * TAB_SIZE;
                spaces[pos] = 0;
                for (int i = TAB_SIZE; i != 0 && pos > 0; i--)
                {
                    pos--;
                    spaces[pos] = ' ';
                }
            }
            // 降级后再打印
            p_printf("%8d, %8d, %10.3f, %10.3f, %s%s\n", tick, delta, delta_us, call_delta_us, spaces, p_tag);
        }
        else
        {
            p_printf("%8d, %8d, %10.3f, %10.3f, %s%s\n", tick, delta, delta_us, call_delta_us, spaces, p_tag);
        }

        idx++;
        if (idx >= EVENT_TRACKING_POINT_MAX_CNT)
        {
            idx = 0;
        }
    }
#endif

    // 恢复记录功能
    __s_event_tracking.disabled = 0;
#endif
    p_printf("-----DUMP-FINISH-----\n\n");
}

void event_tracking_clear()
{
#if EVENT_TRACKING_POINT_MAX_CNT > 0
    __s_event_tracking.idx = 0;
    __s_event_tracking.cnt = 0;
#endif
}
