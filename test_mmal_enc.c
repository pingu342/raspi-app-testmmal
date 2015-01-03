#include <stdio.h>
#include "interface/mmal/mmal.h"
#include "interface/mmal/mmal_buffer.h"
#include "interface/mmal/util/mmal_default_components.h"
#include "interface/mmal/util/mmal_util.h"
#include "interface/mmal/util/mmal_util_params.h"
#include "interface/mmal/util/mmal_connection.h"
#include "interface/vcos/vcos.h"
#include "debug.h"

static MMAL_POOL_T *pOutputPool = NULL;
static MMAL_POOL_T *pInputPool = NULL;
static MMAL_COMPONENT_T *pVideoEncoderComponent = NULL;
static FILE *pOutputFile = NULL;
static FILE *pInputFile = NULL;

static void input_callback(MMAL_PORT_T *pPort, MMAL_BUFFER_HEADER_T *pBufh)
{
	mmal_buffer_header_release(pBufh);
}

static void output_callback(MMAL_PORT_T *pPort, MMAL_BUFFER_HEADER_T *pBufh)
{
	MMAL_STATUS_T status;
	MMAL_BUFFER_HEADER_T *pNewBufh;

	if (pBufh->cmd != 0) {
		LOGI("event : %d", pBufh->cmd);
		goto release;
	}
	
	LOGD("Encoded data : %d bytes", pBufh->length);
	//LOG_BFH(LOG_DEBUG, pBufh, "Encoded buffer");
	mmal_buffer_header_mem_lock(pBufh);
	fwrite(pBufh->data, 1, pBufh->length, pOutputFile);
	mmal_buffer_header_mem_unlock(pBufh);

release:
	mmal_buffer_header_release(pBufh);
	pNewBufh = mmal_queue_get(pOutputPool->queue);
	if (pNewBufh) {
		status = mmal_port_send_buffer(pPort, pNewBufh);
	}
}

void prepare_video_encoder_component(void)
{
	MMAL_STATUS_T status;
	MMAL_PORT_T *pVideoEncoderInputPort = NULL, *pVideoEncoderOutputPort = NULL;
	
	status = mmal_component_create(MMAL_COMPONENT_DEFAULT_VIDEO_ENCODER, &pVideoEncoderComponent);
	if (status != MMAL_SUCCESS) {
		LOGE("Failed to create video encoder component.");
		goto error;
	}

	if (!pVideoEncoderComponent->input_num || !pVideoEncoderComponent->output_num) {
		status = MMAL_ENOSYS;
		LOGE("Video encoder doesn't have input/output ports");
		goto error;
	} else {
		LOGD("Camera has %d input ports and %d output ports.", pVideoEncoderComponent->input_num, pVideoEncoderComponent->output_num);
	}

	pVideoEncoderInputPort = pVideoEncoderComponent->input[0];
	pVideoEncoderOutputPort = pVideoEncoderComponent->output[0];

	pVideoEncoderInputPort->format->type = MMAL_ES_TYPE_VIDEO;
	pVideoEncoderInputPort->format->encoding = MMAL_ENCODING_I420;
	pVideoEncoderInputPort->format->encoding_variant = MMAL_ENCODING_I420;
	pVideoEncoderInputPort->format->es->video.width = 640;
	pVideoEncoderInputPort->format->es->video.height = 480;
	pVideoEncoderInputPort->format->es->video.crop.x = 0;
	pVideoEncoderInputPort->format->es->video.crop.y = 0;
	pVideoEncoderInputPort->format->es->video.crop.width = 640;
	pVideoEncoderInputPort->format->es->video.crop.height = 480;
	pVideoEncoderInputPort->format->es->video.frame_rate.num = 15;
	pVideoEncoderInputPort->format->es->video.frame_rate.den = 1;

	status = mmal_port_format_commit(pVideoEncoderInputPort);
	if (status != MMAL_SUCCESS) {
		LOGE("Video encoder input format couldn't be set");
		goto error;
	}

	LOGD("Video encoder's input port : buffer_num_recommended=%d, buffer_size_recommended=%d",
			pVideoEncoderInputPort->buffer_num_recommended, pVideoEncoderInputPort->buffer_size_recommended);

	pVideoEncoderInputPort->buffer_size = pVideoEncoderInputPort->buffer_size_recommended;
	pVideoEncoderInputPort->buffer_num = pVideoEncoderInputPort->buffer_num_recommended;
	if (pVideoEncoderInputPort->buffer_num < 3) {
		pVideoEncoderInputPort->buffer_num = 3;
	}

	pVideoEncoderOutputPort->format->encoding = MMAL_ENCODING_H264;
	pVideoEncoderOutputPort->format->bitrate = 2000000;
	pVideoEncoderOutputPort->format->es->video.frame_rate.num = 0;
	pVideoEncoderOutputPort->format->es->video.frame_rate.den = 1;

	status = mmal_port_format_commit(pVideoEncoderOutputPort);
	if (status != MMAL_SUCCESS) {
		LOGE("Video encoder output format couldn't be set");
		goto error;
	}

	LOGD("Video encoder's output port : buffer_num_recommended=%d, buffer_size_recommended=%d",
			pVideoEncoderOutputPort->buffer_num_recommended, pVideoEncoderOutputPort->buffer_size_recommended);

	pVideoEncoderOutputPort->buffer_size = pVideoEncoderOutputPort->buffer_size_recommended;
	pVideoEncoderOutputPort->buffer_num = pVideoEncoderOutputPort->buffer_num_recommended;
	if (pVideoEncoderOutputPort->buffer_num < 3) {
		pVideoEncoderOutputPort->buffer_num = 3;
	}

	if (mmal_port_parameter_set_boolean(pVideoEncoderInputPort, MMAL_PARAMETER_VIDEO_IMMUTABLE_INPUT, 1) != MMAL_SUCCESS) {
		LOGE("Unable to set immutable input flag");
	}

	status = mmal_component_enable(pVideoEncoderComponent);
	if (status != MMAL_SUCCESS) {
		LOGE("Unable to enable video encoder component");
		goto error;
	}

	return;

error:
	if (pVideoEncoderComponent != NULL) {
		mmal_component_destroy(pVideoEncoderComponent);
		pVideoEncoderComponent = NULL;
	}

}

int main(void)
{
	MMAL_STATUS_T status;
	MMAL_PORT_T *pOutputPort = NULL, *pInputPort = NULL;
	MMAL_BUFFER_HEADER_T *pBufh;
	int frameNo = 0;
	double pts = 0.0;

	pInputFile = fopen("test.yuv", "rb");
	if (pInputFile == NULL) {
		LOGE("Could not open input file.");
		goto error;
	}

	pOutputFile = fopen("test.h264", "wb+");
	if (pOutputFile == NULL) {
		LOGE("Could not open output file.");
		goto error;
	}

	bcm_host_init();
	prepare_video_encoder_component();

	pOutputPort = pVideoEncoderComponent->output[0];
	pInputPort  = pVideoEncoderComponent->input[0];

	LOG_FMT(LOG_DEBUG, pInputPort->format, "Video encoder component input port's format");
	LOG_FMT(LOG_DEBUG, pOutputPort->format, "Video encoder component output port's format");
	videoparams_log_print(pOutputPort);

	status = mmal_port_enable(pInputPort, input_callback);
	if (status != MMAL_SUCCESS) {
		LOGE("Failed to setup encoder input");
		goto error;
	}

	status = mmal_port_enable(pOutputPort, output_callback);
	if (status != MMAL_SUCCESS) {
		LOGE("Failed to setup encoder output");
		goto error;
	}

	pInputPool = mmal_pool_create(pInputPort->buffer_num, pInputPort->buffer_size);

	pOutputPool = mmal_port_pool_create(pOutputPort, pOutputPort->buffer_num, pOutputPort->buffer_size);
	if (!pOutputPool) {
		LOGE("Failed to create buffer header pool for camera video output port %s", pOutputPort->name);
		goto error;
	}

	{
		int num = mmal_queue_length(pOutputPool->queue);
		int q;
		for (q=0; q<num; q++) {
			MMAL_BUFFER_HEADER_T *pBufh = mmal_queue_get(pOutputPool->queue);
			if (!pBufh) {
				LOGE("Unable to get a required buffer %d from pool queue", q);
			}
			if (mmal_port_send_buffer(pOutputPort, pBufh)!= MMAL_SUCCESS) {
				LOGE("Unable to send a buffer to encoder output port (%d)", q);
			}
		}
	}

	while (1) {
		if ((pBufh = mmal_queue_get(pInputPool->queue)) != NULL) {
			pBufh->length = fread(pBufh->data, 1, pInputPort->buffer_size, pInputFile);
			if (pBufh->length <= 0) {
				break;
			}
			pBufh->flags = MMAL_BUFFER_HEADER_FLAG_FRAME;
			pBufh->pts = pBufh->dts = (int64_t)pts;
			LOGI("Frame#%d : %d bytes", frameNo, pBufh->length);
			status = mmal_port_send_buffer(pInputPort, pBufh);
			if (status != MMAL_SUCCESS) {
				LOG_BFH(LOG_ERROR, pBufh, "Unable to send a buffer to encoder input port : error %d", status);
			}
			frameNo++;
			pts += 1000000.0/15.0;
		} else {
			LOGW("No buffer header");
		}

		usleep(10000);
	}

error:
	if (pVideoEncoderComponent != NULL) {
		mmal_component_destroy(pVideoEncoderComponent);
		pVideoEncoderComponent = NULL;
	}

	if (pInputFile != NULL) {
		fclose(pInputFile);
	}

	if (pOutputFile != NULL) {
		fclose(pOutputFile);
	}

	return 0;
}
