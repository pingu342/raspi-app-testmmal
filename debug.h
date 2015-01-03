#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "interface/mmal/mmal.h"
#include "interface/mmal/mmal_buffer.h"
#include "interface/mmal/util/mmal_default_components.h"
#include "interface/mmal/util/mmal_util.h"
#include "interface/mmal/util/mmal_util_params.h"
#include "interface/mmal/util/mmal_connection.h"
#include "interface/vcos/vcos.h"

enum {
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR
};

extern void my_log_print     (const char *pFilePath, int line, const char *pFuncName, int level, const char *pFormat, ...);
extern void format_log_print (const char *pFilePath, int line, const char *pFuncName, int level, MMAL_ES_FORMAT_T *pFormat, const char *pFormat0, ...);
extern void bufh_log_print   (const char *pFilePath, int line, const char *pFuncName, int level, MMAL_BUFFER_HEADER_T *pBufh, const char *pFormat, ...);

#define LOGE(...)    my_log_print(__FILE__, __LINE__, __FUNCTION__, LOG_ERROR, __VA_ARGS__)
#define LOGW(...)    my_log_print(__FILE__, __LINE__, __FUNCTION__, LOG_WARNING, __VA_ARGS__)
#define LOGI(...)    my_log_print(__FILE__, __LINE__, __FUNCTION__, LOG_INFO, __VA_ARGS__)
#define LOGD(...)    my_log_print(__FILE__, __LINE__, __FUNCTION__, LOG_DEBUG, __VA_ARGS__)
#define LOG_FMT(...) format_log_print(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__);
#define LOG_BFH(...) bufh_log_print(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__);

extern const char *profile_name(MMAL_VIDEO_PROFILE_T profile);
extern const char *level_name(MMAL_VIDEO_LEVEL_T level);
extern const char *ratecontrol_name(MMAL_VIDEO_RATECONTROL_T control);
extern const char *nalunitformat_names(MMAL_VIDEO_NALUNITFORMAT_T format);

extern void videoparams_log_print(MMAL_PORT_T *pPort);

#endif /*__DEBUG_H__*/
