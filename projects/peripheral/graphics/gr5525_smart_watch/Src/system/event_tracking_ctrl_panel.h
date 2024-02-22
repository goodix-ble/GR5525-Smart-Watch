/**
 * GLOBAL
 */
#define DISABLE_ALL_EVENT_TRACKING 1



#include "event_tracking.h"

#if DISABLE_ALL_EVENT_TRACKING
#include "event_tracking_passby.h"
#else

#if defined(LV_OBJ_C) && 0
#include "event_tracking_passby.h"
#endif

#if defined(LV_OBJ_DRAW_C) && 0
#include "event_tracking_passby.h"
#endif

#if defined(LV_OBJ_SCROLL_C) && 0
#include "event_tracking_passby.h"
#endif

#if defined(LV_OBJ_POS_C) && 0
#include "event_tracking_passby.h"
#endif

#if defined(LV_REFR_C) && 0
#include "event_tracking_passby.h"
#endif

#if defined(LV_IMG_GXIMG_C) && 0
#include "event_tracking_passby.h"
#endif

#if defined(LV_IMG_RGB565_C) && 0
#include "event_tracking_passby.h"
#endif

#if defined(LV_PORT_GXIMG_C) && 0
#include "event_tracking_passby.h"
#endif

#if defined(LV_PORT_DISP_C) && 0
#include "event_tracking_passby.h"
#endif

#if defined(LV_DRAW_LABEL_C) && 0
#include "event_tracking_passby.h"
#endif

#if defined(LV_DRAW_SW_BLEND_C) && 0
#include "event_tracking_passby.h"
#endif

#endif // DISABLE_ALL_EVENT_TRACKING
