/**
 * @author dingjian@goodix.com
 * @brief 本模块实现了在代码中增加埋点的功能。可以记录埋点的发生的时间。通过计算各个埋点之间的时间差可以算出程序的运行性能。也可以通过埋点得到程序的运行路径。
 * 可以通过">>"和"[["标记进入函数和退出函数。模块会根据这个配对关系计算函数调用时间。
 * 
 * @param DISABLE_EVENT_TRACKING 在编译时，通过该宏可以关闭埋点功能。
 * @param EVENT_TRACKING_POINT_MAX_CNT 最大记录的埋点数量。记录的埋点过多时，会循环覆盖。
 *
 */

#ifndef _EVENT_TRACKING_H_
#define _EVENT_TRACKING_H_

#include <stdio.h>

#define DISABLE_EVENT_TRACKING 1

#ifdef DISABLE_EVENT_TRACKING
#define EVENT_TRACKING_POINT_MAX_CNT 0
#else
#define EVENT_TRACKING_POINT_MAX_CNT 1024
#endif

#if EVENT_TRACKING_POINT_MAX_CNT > 0
#define event_tracking_create_tag(tag) _event_tracking_create_tag(tag)
#else
#define event_tracking_create_tag(tag)
#endif

/**
 * @brief 记录下埋点的时间和标签，方面阅读分析。请使用宏函数 event_tracking_create_tag() 。
 * 
 * @param tag 埋点的标签，必须使用静态字符串，或者生命周期足够长的动态内存。
 */
void _event_tracking_create_tag(const char *tag);

/**
 * @brief 通过打印函数输出存储的埋点。并且自动计算有函数调用的时间。
 * @param p_printf 如果设置为NULL就会采用默认打印函数 printf()
 */
void event_tracking_dump(int (*p_printf)(const char *, ...));

/**
 * @brief 清空存储的埋点数据
 */
void event_tracking_clear(void);

#endif
