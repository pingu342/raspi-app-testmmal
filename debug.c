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

