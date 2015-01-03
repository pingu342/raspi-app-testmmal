#include <stdio.h>
#include "debug.h"

#define FOURCC_CHARS(f) (f)&0xff,((f)>>8)&0xff,((f)>>16)&0xff,((f)>>24)&0xff

const char logLevel[] = {'D','I','W','E'};

const char *get_filename(const char *pFilePath)
{
	const char *p, *pFileName;

	pFileName = p = pFilePath;
	while ((p = strchr(pFileName, '/')) != NULL) {
		pFileName = p + 1;
	}

	return pFileName;
}

void my_log_print(const char *pFilePath, int line, const char *pFuncName, int level, const char *pFormat, ...)
{
	static char msg[1024];
	va_list arg={0};
	pthread_t th = pthread_self();
	va_start(arg, pFormat);
	vsnprintf(msg, sizeof(msg), pFormat, arg);
	va_end(arg);
	fprintf(stderr, "/%c %08x %s:%d %s: %s\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, msg);
}

void format_log_print(const char *pFilePath, int line, const char *pFuncName, int level, MMAL_ES_FORMAT_T *pFormat, const char *pFormat0, ...)
{
	static char msg[1024];
	va_list arg={0};
	pthread_t th = pthread_self();
	va_start(arg, pFormat0);
	vsnprintf(msg, sizeof(msg), pFormat0, arg);
	va_end(arg);
	fprintf(stderr, "/%c %08x %s:%d %s: %s\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, msg);
	fprintf(stderr, "/%c %08x %s:%d %s:  MMAL_ES_FORMAT_T {\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName);
	fprintf(stderr, "/%c %08x %s:%d %s:  .type=%u\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, pFormat->type);
	fprintf(stderr, "/%c %08x %s:%d %s:  .encoding=%c%c%c%c\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, FOURCC_CHARS(pFormat->encoding));
	fprintf(stderr, "/%c %08x %s:%d %s:  .encoding_variant=%c%c%c%c\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, FOURCC_CHARS(pFormat->encoding_variant));
	fprintf(stderr, "/%c %08x %s:%d %s:  .es=%p\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, pFormat->es);
	if (pFormat->es) {
	fprintf(stderr, "/%c %08x %s:%d %s:   .video.width=%u\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, pFormat->es->video.width);
	fprintf(stderr, "/%c %08x %s:%d %s:   .video.height=%u\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, pFormat->es->video.height);
	fprintf(stderr, "/%c %08x %s:%d %s:   .video.crop.x=%u\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, pFormat->es->video.crop.x);
	fprintf(stderr, "/%c %08x %s:%d %s:   .video.crop.y=%u\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, pFormat->es->video.crop.y);
	fprintf(stderr, "/%c %08x %s:%d %s:   .video.crop.width=%u\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, pFormat->es->video.crop.width);
	fprintf(stderr, "/%c %08x %s:%d %s:   .video.crop.height=%u\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, pFormat->es->video.crop.height);
	fprintf(stderr, "/%c %08x %s:%d %s:   .video.frame_rate.num=%d\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, pFormat->es->video.frame_rate.num);
	fprintf(stderr, "/%c %08x %s:%d %s:   .video.frame_rate.den=%d\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, pFormat->es->video.frame_rate.den);
	fprintf(stderr, "/%c %08x %s:%d %s:   .video.par.num=%d\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, pFormat->es->video.par.num);
	fprintf(stderr, "/%c %08x %s:%d %s:   .video.par.den=%d\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, pFormat->es->video.par.den);
	fprintf(stderr, "/%c %08x %s:%d %s:   .video.color_space=%u\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, pFormat->es->video.color_space);
	}
	fprintf(stderr, "/%c %08x %s:%d %s:  .bitrate=%u\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, pFormat->bitrate);
	fprintf(stderr, "/%c %08x %s:%d %s:  .flags=%#08x\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, pFormat->flags);
	fprintf(stderr, "/%c %08x %s:%d %s:  .extradata_size=%u\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, pFormat->extradata_size);
	fprintf(stderr, "/%c %08x %s:%d %s:  .extradata=%p\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, pFormat->extradata);
	fprintf(stderr, "/%c %08x %s:%d %s:  }\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName);
}

void bufh_log_print(const char *pFilePath, int line, const char *pFuncName, int level, MMAL_BUFFER_HEADER_T *pBufh, const char *pFormat, ...)
{
	static char msg[1024];
	va_list arg={0};
	pthread_t th = pthread_self();
	va_start(arg, pFormat);
	vsnprintf(msg, sizeof(msg), pFormat, arg);
	va_end(arg);
	fprintf(stderr, "/%c %08x %s:%d %s: %s\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, msg);
	fprintf(stderr, "/%c %08x %s:%d %s:  MMAL_BUFFER_HEADER_T {\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, pBufh);
	fprintf(stderr, "/%c %08x %s:%d %s:  .cmd=%d\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, pBufh->cmd);
	fprintf(stderr, "/%c %08x %s:%d %s:  .data=%p\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, pBufh->data);
	fprintf(stderr, "/%c %08x %s:%d %s:  .alloc_size=%u\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, pBufh->alloc_size);
	fprintf(stderr, "/%c %08x %s:%d %s:  .length=%u\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, pBufh->length);
	fprintf(stderr, "/%c %08x %s:%d %s:  .offset=%u\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, pBufh->offset);
	fprintf(stderr, "/%c %08x %s:%d %s:  .flags=%#08x\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, pBufh->flags);
	fprintf(stderr, "/%c %08x %s:%d %s:  .pts=%lld\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, pBufh->pts);
	fprintf(stderr, "/%c %08x %s:%d %s:  .dts=%lld\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, pBufh->dts);
	fprintf(stderr, "/%c %08x %s:%d %s:  .type=%p\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, pBufh->type);
	if (pBufh->type) {
	fprintf(stderr, "/%c %08x %s:%d %s:   .video.planes=%u\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, pBufh->type->video.planes);
	fprintf(stderr, "/%c %08x %s:%d %s:   .video.offset={%u,%u,%u,%u}\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, pBufh->type->video.offset[0], pBufh->type->video.offset[1], pBufh->type->video.offset[2], pBufh->type->video.offset[3]);
	fprintf(stderr, "/%c %08x %s:%d %s:   .video.pitch={%u,%u,%u,%u}\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, pBufh->type->video.pitch[0], pBufh->type->video.pitch[1], pBufh->type->video.pitch[2], pBufh->type->video.pitch[3]);
	fprintf(stderr, "/%c %08x %s:%d %s:   .video.flags=%#08x\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, pBufh->type->video.flags);
	}
	fprintf(stderr, "/%c %08x %s:%d %s:  .user_data=%p\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName, pBufh->user_data);
	fprintf(stderr, "/%c %08x %s:%d %s:  }\n", logLevel[level], th, get_filename(pFilePath), line, pFuncName);
}

const char *profile_name(MMAL_VIDEO_PROFILE_T profile)
{
	const char *profile_names[] = {
		"H263_BASELINE",
		"H263_H320CODING",
		"H263_BACKWARDCOMPATIBLE",
		"H263_ISWV2",
		"H263_ISWV3",
		"H263_HIGHCOMPRESSION",
		"H263_INTERNET",
		"H263_INTERLACE",
		"H263_HIGHLATENCY",
		"MP4V_SIMPLE",
		"MP4V_SIMPLESCALABLE",
		"MP4V_CORE",
		"MP4V_MAIN",
		"MP4V_NBIT",
		"MP4V_SCALABLETEXTURE",
		"MP4V_SIMPLEFACE",
		"MP4V_SIMPLEFBA",
		"MP4V_BASICANIMATED",
		"MP4V_HYBRID",
		"MP4V_ADVANCEDREALTIME",
		"MP4V_CORESCALABLE",
		"MP4V_ADVANCEDCODING",
		"MP4V_ADVANCEDCORE",
		"MP4V_ADVANCEDSCALABLE",
		"MP4V_ADVANCEDSIMPLE",
		"H264_BASELINE",
		"H264_MAIN",
		"H264_EXTENDED",
		"H264_HIGH",
		"H264_HIGH10",
		"H264_HIGH422",
		"H264_HIGH444",
		"H264_CONSTRAINED_BASELINE"
	};
	if (0 <= profile && profile < (sizeof(profile_names) / sizeof(char*))) {
		return profile_names[profile];
	} else if (profile == MMAL_VIDEO_PROFILE_DUMMY) {
		return "DUMMY";
	}
	return "?";
}

const char *level_name(MMAL_VIDEO_LEVEL_T level)
{
	const char *level_names[] = {
		"H263_10",
		"H263_20",
		"H263_30",
		"H263_40",
		"H263_45",
		"H263_50",
		"H263_60",
		"H263_70",
		"MP4V_0",
		"MP4V_0b",
		"MP4V_1",
		"MP4V_2",
		"MP4V_3",
		"MP4V_4",
		"MP4V_4a",
		"MP4V_5",
		"MP4V_6",
		"H264_1",
		"H264_1b",
		"H264_11",
		"H264_12",
		"H264_13",
		"H264_2",
		"H264_21",
		"H264_22",
		"H264_3",
		"H264_31",
		"H264_32",
		"H264_4",
		"H264_41",
		"H264_42",
		"H264_5",
		"H264_51",
		"DUMMY"
	};
	if (0 <= level && level < (sizeof(level_names) / sizeof(char*))) {
		return level_names[level];
	} else if (level == MMAL_VIDEO_LEVEL_DUMMY) {
		return "DUMMY";
	}
	return "?";
}

const char *ratecontrol_name(MMAL_VIDEO_RATECONTROL_T control)
{
	const char *ratecontrol_names[] = {
		"DEFAULT",
		"VARIABLE",
		"CONSTANT",
		"VARIABLE_SKIP_FRAMES",
		"CONSTANT_SKIP_FRAMES"
	};
	if (0 <= control && control < (sizeof(ratecontrol_names) / sizeof(char*))) {
		return ratecontrol_names[control];
	} else if (control == MMAL_VIDEO_RATECONTROL_DUMMY) {
		return "DUMMY";
	}
	return "?";
}

const char *nalunitformat_names(MMAL_VIDEO_NALUNITFORMAT_T format)
{
	int i;
	const struct {
		MMAL_VIDEO_NALUNITFORMAT_T format;
		const char *name;
	} nalunitformat_names[] = {
		{MMAL_VIDEO_NALUNITFORMAT_STARTCODES, "STARTCODES"},
		{MMAL_VIDEO_NALUNITFORMAT_NALUNITPERBUFFER, "NALUNITPERBUFFER"},
		{MMAL_VIDEO_NALUNITFORMAT_ONEBYTEINTERLEAVELENGTH, "ONEBYTEINTERLEAVELENGTH"},
		{MMAL_VIDEO_NALUNITFORMAT_TWOBYTEINTERLEAVELENGTH, "TWOBYTEINTERLEAVELENGTH"},
		{MMAL_VIDEO_NALUNITFORMAT_FOURBYTEINTERLEAVELENGTH, "FOURBYTEINTERLEAVELENGTH"},
		{MMAL_VIDEO_NALUNITFORMAT_DUMMY, "DUMMY"},
		{0, NULL}
	};
	for (i=0; ;i++) {
		if (nalunitformat_names[i].name == NULL) {
			break;
		} else if (nalunitformat_names[i].format == format) {
			return nalunitformat_names[i].name;
		}
	}
	return "?";
}

void videoparams_log_print(MMAL_PORT_T *pPort)
{
	MMAL_STATUS_T status;

	/* MMAL_PARAMETER_SUPPORTED_PROFILES */
	{
		int i, num;
		struct {
			MMAL_PARAMETER_HEADER_T hdr;
			struct {
				MMAL_VIDEO_PROFILE_T profile;
				MMAL_VIDEO_LEVEL_T level;
			} profile[8];
		} param = {
			{MMAL_PARAMETER_SUPPORTED_PROFILES, sizeof(param)},
			{0}
		};
		LOGD("MMAL_PARAMETER_SUPPORTED_PROFILES :");
		status = mmal_port_parameter_get(pPort, (MMAL_PARAMETER_HEADER_T*)&param);
		if (status != MMAL_SUCCESS) {
			LOGE(" Failed to get parameter");
		} else {
			num = (param.hdr.size - sizeof(param.hdr)) / sizeof(param.profile[0]);
			for (i=0; i<num && i<8; i++) {
				LOGD(" .profile[%d]", i);
				LOGD("  .profile=%s", profile_name(param.profile[i].profile));
				LOGD("  .level=%s", level_name(param.profile[i].level));
				if (param.profile[i].profile==0 && param.profile[i].level==0) {
					break;
				}
			}
		}
	}

	/* MMAL_PARAMETER_PROFILE */
	{
		MMAL_PARAMETER_VIDEO_PROFILE_T param = {
			{MMAL_PARAMETER_PROFILE, sizeof(param)},
			{0}
		};
		LOGD("MMAL_PARAMETER_PROFILE :");
		status = mmal_port_parameter_get(pPort, (MMAL_PARAMETER_HEADER_T*)&param);
		if (status != MMAL_SUCCESS) {
			LOGE(" Failed to get parameter");
		} else {
			LOGD(" .profile=%s", profile_name(param.profile[0].profile));
			LOGD(" .level=%s", level_name(param.profile[0].level));
		}
	}

	/* MMAL_PARAMETER_INTRAPERIOD */
	{
		MMAL_PARAMETER_UINT32_T param = {
			{MMAL_PARAMETER_INTRAPERIOD, sizeof(param)},
			0
		};
		LOGD("MMAL_PARAMETER_INTRAPERIOD :");
		status = mmal_port_parameter_get(pPort, (MMAL_PARAMETER_HEADER_T*)&param);
		if (status != MMAL_SUCCESS) {
			LOGE(" Failed to get parameter");
		} else {
			LOGD(" .value=%u", param.value);
		}
	}

	/* MMAL_PARAMETER_RATECONTROL */
	{
		MMAL_PARAMETER_VIDEO_RATECONTROL_T param = {
			{MMAL_PARAMETER_RATECONTROL, sizeof(param)},
			0
		};
		LOGD("MMAL_PARAMETER_RATECONTROL :");
		status = mmal_port_parameter_get(pPort, (MMAL_PARAMETER_HEADER_T*)&param);
		if (status != MMAL_SUCCESS) {
			LOGE(" Failed to get parameter");
		} else {
			LOGD(" .control=%s", ratecontrol_name(param.control));
		}
	}

	/* MMAL_PARAMETER_NALUNITFORMAT */
	{
		MMAL_PARAMETER_VIDEO_NALUNITFORMAT_T param = {
			{MMAL_PARAMETER_NALUNITFORMAT, sizeof(param)},
			0
		};
		LOGD("MMAL_PARAMETER_NALUNITFORMAT :");
		status = mmal_port_parameter_get(pPort, (MMAL_PARAMETER_HEADER_T*)&param);
		if (status != MMAL_SUCCESS) {
			LOGE(" Failed to get parameter");
		} else {
			LOGD(" .format=%s", nalunitformat_names(param.format));
		}
	}

	/* MMAL_PARAMETER_VIDEO_BIT_RATE */
	{
		MMAL_PARAMETER_UINT32_T param = {
			{MMAL_PARAMETER_VIDEO_BIT_RATE, sizeof(param)},
			0
		};
		LOGD("MMAL_PARAMETER_VIDEO_BIT_RATE :");
		status = mmal_port_parameter_get(pPort, (MMAL_PARAMETER_HEADER_T*)&param);
		if (status != MMAL_SUCCESS) {
			LOGE(" Failed to get parameter");
		} else {
			LOGD(" .value=%u", param.value);
		}
	}

	/* MMAL_PARAMETER_VIDEO_IMMUTABLE_INPUT */
	{
		MMAL_PARAMETER_BOOLEAN_T param = {
			{MMAL_PARAMETER_VIDEO_IMMUTABLE_INPUT, sizeof(param)},
			0
		};
		LOGD("MMAL_PARAMETER_VIDEO_IMMUTABLE_INPUT :");
		status = mmal_port_parameter_get(pPort, (MMAL_PARAMETER_HEADER_T*)&param);
		if (status != MMAL_SUCCESS) {
			LOGE(" Failed to get parameter");
		} else {
			LOGD(" .enable=%u", param.enable);
		}
	}
}

