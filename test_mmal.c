#include <stdio.h>
#include "interface/mmal/mmal.h"
#include "interface/mmal/mmal_buffer.h"
#include "interface/mmal/util/mmal_default_components.h"
#include "interface/mmal/util/mmal_util.h"
#include "interface/mmal/util/mmal_util_params.h"
#include "interface/mmal/util/mmal_connection.h"
#include "interface/vcos/vcos.h"
#include "debug.h"

typedef struct {
	int enable;       /// Turn colourFX on or off
	int u,v;          /// U and V to use
} MMAL_PARAM_COLOURFX_T;

typedef struct {
	int enable;
	int width,height;
	int quality;
} MMAL_PARAM_THUMBNAIL_CONFIG_T;

typedef struct {
	double x;
	double y;
	double w;
	double h;
} PARAM_FLOAT_RECT_T;

typedef struct {
	int cameraNum;				/// Camera number.
	int sensorMode;				/// Sensor mode. 0=auto.
	int width;
	int height;
	int fps;
} CAMERA_STATE;

#define MMAL_CAMERA_PREVIEW_PORT 0
#define MMAL_CAMERA_VIDEO_PORT 1
#define MMAL_CAMERA_CAPTURE_PORT 2

static CAMERA_STATE cameraState;
static MMAL_POOL_T *pPool = NULL;
static MMAL_COMPONENT_T *pCameraComponent = NULL;
static MMAL_COMPONENT_T *pPreviewComponent = NULL;
static FILE *pFile = NULL;

void init_camera_state(CAMERA_STATE *pCameraState)
{
	pCameraState->cameraNum = 0;
	pCameraState->sensorMode = 0;
	pCameraState->width = 640;
	pCameraState->height = 480;
	pCameraState->fps = 15;
}

static void camera_control_callback(MMAL_PORT_T *pPort, MMAL_BUFFER_HEADER_T *pBufh)
{
	switch (pBufh->cmd) {
	case MMAL_EVENT_PARAMETER_CHANGED:
		LOGI("Parameter changed");
		break;
	case MMAL_EVENT_FORMAT_CHANGED:
		LOGI("Format changed");
		break;
	case MMAL_EVENT_ERROR:
		LOGI("Error");
		break;
	case MMAL_EVENT_EOS:
		LOGI("EOS");
		break;
	default:
		LOGI("Unknown event : 0x%08x", pBufh->cmd);
	}
	mmal_buffer_header_release(pBufh);
}

static void camera_video_callback(MMAL_PORT_T *pPort, MMAL_BUFFER_HEADER_T *pBufh)
{
	MMAL_STATUS_T status;
	MMAL_BUFFER_HEADER_T *pNewBufh;
	static int frameNo = 0;

	if (pBufh->cmd != 0 || pBufh->length == 0) {
		goto release;
	}
	
	LOG_BFH(LOG_DEBUG, pBufh, "Frame#%d Camera video output", frameNo);
	mmal_buffer_header_mem_lock(pBufh);
	fwrite(pBufh->data, 1, pBufh->length, pFile);
	mmal_buffer_header_mem_unlock(pBufh);
	frameNo++;

release:
	mmal_buffer_header_release(pBufh);
	pNewBufh = mmal_queue_get(pPool->queue);
	if (pNewBufh) {
		status = mmal_port_send_buffer(pPort, pNewBufh);
	}
}

void prepare_camera_component(CAMERA_STATE *pCameraState)
{
	MMAL_STATUS_T status;
	MMAL_PORT_T *pPreviewOutputPort = NULL, *pVideOutputPort = NULL, *pStillOutputPort = NULL;
	MMAL_ES_FORMAT_T *pFormat = NULL;

	status = mmal_component_create(MMAL_COMPONENT_DEFAULT_CAMERA, &pCameraComponent);
	if (status != MMAL_SUCCESS) {
		LOGE("Failed to create camera component.");
		goto error;
	}

	{
		MMAL_PARAMETER_INT32_T param = {{MMAL_PARAMETER_CAMERA_NUM, sizeof(param)}, pCameraState->cameraNum};
		status = mmal_port_parameter_set(pCameraComponent->control, &param.hdr);
		if (status != MMAL_SUCCESS) {
			LOGE("Could not select camera : error %d", status);
			goto error;
		}
	}

	if (!pCameraComponent->output_num) {
		status = MMAL_ENOSYS;
		LOGE("Camera doesn't have output ports");
		goto error;
	} else {
		LOGD("Camera has %d ports", pCameraComponent->output_num);
	}

	status = mmal_port_parameter_set_uint32(pCameraComponent->control, MMAL_PARAMETER_CAMERA_CUSTOM_SENSOR_CONFIG, pCameraState->sensorMode);
	if (status != MMAL_SUCCESS) {
		LOGE("Could not set sensor mode : error %d", status);
		goto error;
	}

	pPreviewOutputPort = pCameraComponent->output[MMAL_CAMERA_PREVIEW_PORT];
	pVideOutputPort = pCameraComponent->output[MMAL_CAMERA_VIDEO_PORT];
	pStillOutputPort = pCameraComponent->output[MMAL_CAMERA_CAPTURE_PORT];

	status = mmal_port_enable(pCameraComponent->control, camera_control_callback);
	if (status != MMAL_SUCCESS) {
		LOGE("Unable to enable control port : error %d", status);
		goto error;
	}

	{
		MMAL_PARAMETER_CAMERA_CONFIG_T param = {
			{ MMAL_PARAMETER_CAMERA_CONFIG, sizeof(param) },
			.max_stills_w = pCameraState->width,
			.max_stills_h = pCameraState->height,
			.stills_yuv422 = 0,
			.one_shot_stills = 0,
			.max_preview_video_w = pCameraState->width,
			.max_preview_video_h = pCameraState->height,
			.num_preview_video_frames = 3,
			.stills_capture_circular_buffer_height = 0,
			.fast_preview_resume = 0,
			.use_stc_timestamp = MMAL_PARAM_TIMESTAMP_MODE_RESET_STC
		};
		mmal_port_parameter_set(pCameraComponent->control, &param.hdr);
	}

	pFormat = pPreviewOutputPort->format;
	pFormat->encoding = MMAL_ENCODING_OPAQUE;
	pFormat->encoding_variant = MMAL_ENCODING_I420;
	pFormat->es->video.width = VCOS_ALIGN_UP(pCameraState->width, 32);
	pFormat->es->video.height = VCOS_ALIGN_UP(pCameraState->height, 16);
	pFormat->es->video.crop.x = 0;
	pFormat->es->video.crop.y = 0;
	pFormat->es->video.crop.width = pCameraState->width;
	pFormat->es->video.crop.height = pCameraState->height;
	pFormat->es->video.frame_rate.num = 0;
	pFormat->es->video.frame_rate.den = 1;

	status = mmal_port_format_commit(pPreviewOutputPort);
	if (status != MMAL_SUCCESS) {
		LOGE("camera viewfinder format couldn't be set");
		goto error;
	}

	pFormat = pVideOutputPort->format;
	pFormat->encoding = MMAL_ENCODING_I420;
	pFormat->encoding_variant = MMAL_ENCODING_I420;
	pFormat->es->video.width = VCOS_ALIGN_UP(pCameraState->width, 32);
	pFormat->es->video.height = VCOS_ALIGN_UP(pCameraState->height, 16);
	pFormat->es->video.crop.x = 0;
	pFormat->es->video.crop.y = 0;
	pFormat->es->video.crop.width = pCameraState->width;
	pFormat->es->video.crop.height = pCameraState->height;
	pFormat->es->video.frame_rate.num = pCameraState->fps;
	pFormat->es->video.frame_rate.den = 1;

	status = mmal_port_format_commit(pVideOutputPort);
	if (status != MMAL_SUCCESS) {
		LOGE("camera video format couldn't be set");
		goto error;
	}

	LOGD("Camera's video output port : buffer_num_recommended=%d, buffer_size_recommended=%d",
			pVideOutputPort->buffer_num_recommended, pVideOutputPort->buffer_size_recommended);

	pVideOutputPort->buffer_size = pVideOutputPort->buffer_size_recommended;
	pVideOutputPort->buffer_num = pVideOutputPort->buffer_num_recommended;
	if (pVideOutputPort->buffer_num < 3) {
		pVideOutputPort->buffer_num = 3;
	}

	pFormat = pStillOutputPort->format;
	pFormat->encoding = MMAL_ENCODING_I420;
	pFormat->encoding_variant = MMAL_ENCODING_I420;
	pFormat->es->video.width = VCOS_ALIGN_UP(pCameraState->width, 32);
	pFormat->es->video.height = VCOS_ALIGN_UP(pCameraState->height, 16);
	pFormat->es->video.crop.x = 0;
	pFormat->es->video.crop.y = 0;
	pFormat->es->video.crop.width = pCameraState->width;
	pFormat->es->video.crop.height = pCameraState->height;
	pFormat->es->video.frame_rate.num = 0;
	pFormat->es->video.frame_rate.den = 1;

	status = mmal_port_format_commit(pStillOutputPort);
	if (status != MMAL_SUCCESS) {
		LOGE("camera still format couldn't be set");
		goto error;
	}

	status = mmal_component_enable(pCameraComponent);
	if (status != MMAL_SUCCESS) {
		LOGE("camera component couldn't be enabled");
		goto error;
	}

	{
		int result;
		result  = raspicamcontrol_set_saturation(pCameraComponent, 0);
		result += raspicamcontrol_set_sharpness(pCameraComponent, 0);
		result += raspicamcontrol_set_contrast(pCameraComponent, 0);
		result += raspicamcontrol_set_brightness(pCameraComponent, 50);
		result += raspicamcontrol_set_ISO(pCameraComponent, 0);
		result += raspicamcontrol_set_video_stabilisation(pCameraComponent, 0);
		result += raspicamcontrol_set_exposure_compensation(pCameraComponent, 0);
		result += raspicamcontrol_set_exposure_mode(pCameraComponent, MMAL_PARAM_EXPOSUREMODE_AUTO);
		result += raspicamcontrol_set_metering_mode(pCameraComponent, MMAL_PARAM_EXPOSUREMETERINGMODE_AVERAGE);
		result += raspicamcontrol_set_awb_mode(pCameraComponent, MMAL_PARAM_AWBMODE_AUTO);
		result += raspicamcontrol_set_awb_gains(pCameraComponent, 0, 0);
		result += raspicamcontrol_set_imageFX(pCameraComponent, MMAL_PARAM_IMAGEFX_NONE);
		{
			MMAL_PARAM_COLOURFX_T param = {.enable=0, .u=128, .v=128};
			result += raspicamcontrol_set_colourFX(pCameraComponent, &param);
		}
		//result += raspicamcontrol_set_thumbnail_parameters(pCameraComponent, &params->thumbnailConfig);  TODO Not working for some reason
		result += raspicamcontrol_set_rotation(pCameraComponent, 0);
		result += raspicamcontrol_set_flips(pCameraComponent, 0, 0);
		{
			PARAM_FLOAT_RECT_T rect = {.x=0.0, .y=0.0, .w=1.0, .h=1.0};
			result += raspicamcontrol_set_ROI(pCameraComponent, rect);
		}
		result += raspicamcontrol_set_shutter_speed(pCameraComponent, 0);
		result += raspicamcontrol_set_DRC(pCameraComponent, MMAL_PARAMETER_DRC_STRENGTH_OFF);
		result += raspicamcontrol_set_stats_pass(pCameraComponent, MMAL_FALSE);
	}

	return;

error:
	if (pCameraComponent != NULL) {
		mmal_component_destroy(pCameraComponent);
		pCameraComponent = NULL;
	}
}

void prepare_preview_component(void)
{
	MMAL_STATUS_T status;

	status = mmal_component_create("vc.null_sink", &pPreviewComponent);
	if (status != MMAL_SUCCESS) {
		LOGE("Unable to create null sink component");
	}

	status = mmal_component_enable(pPreviewComponent);
	if (status != MMAL_SUCCESS) {
		LOGE("preview component couldn't be enabled");
	}
}

static MMAL_STATUS_T connect_ports(MMAL_PORT_T *pOutputPort, MMAL_PORT_T *pInputPort, MMAL_CONNECTION_T **ppConnection)
{
	MMAL_STATUS_T status;

	status = mmal_connection_create(ppConnection, pOutputPort, pInputPort, MMAL_CONNECTION_FLAG_TUNNELLING | MMAL_CONNECTION_FLAG_ALLOCATION_ON_INPUT);
	if (status == MMAL_SUCCESS) {
		status = mmal_connection_enable(*ppConnection);
		if (status != MMAL_SUCCESS) {
			mmal_connection_destroy(*ppConnection);
			LOGE("Failed to enable connection : error %d", status);
		}
	} else {
		LOGE("Failed to create connection : error %d", status);
	}

	return status;
}

int main(void)
{
	MMAL_STATUS_T status;
	MMAL_PORT_T *pCameraPreviewOutputPort = NULL;
	MMAL_PORT_T *pCameraVideoOutputPort = NULL;
	MMAL_PORT_T *pPreviewInputPort = NULL;
	MMAL_CONNECTION_T *pConnectionPreview = NULL;

	pFile = fopen("test.yuv", "wb+");
	if (pFile == NULL) {
		LOGE("Could not open output file.");
		goto error;
	}

	bcm_host_init();
	init_camera_state(&cameraState);
	prepare_camera_component(&cameraState);
	prepare_preview_component();

	pCameraPreviewOutputPort = pCameraComponent->output[MMAL_CAMERA_PREVIEW_PORT];
	pCameraVideoOutputPort = pCameraComponent->output[MMAL_CAMERA_VIDEO_PORT];
	pPreviewInputPort = pPreviewComponent->input[0];

	connect_ports(pCameraPreviewOutputPort, pPreviewInputPort, &pConnectionPreview);

	LOG_FMT(LOG_DEBUG, pCameraVideoOutputPort->format, "Camera video output port's format");
	
	status = mmal_port_enable(pCameraVideoOutputPort, camera_video_callback);
	if (status != MMAL_SUCCESS) {
		LOGE("Failed to setup encoder output");
		goto error;
	}

	pPool = mmal_port_pool_create(pCameraVideoOutputPort, pCameraVideoOutputPort->buffer_num, pCameraVideoOutputPort->buffer_size);
	if (!pPool) {
		LOGE("Failed to create buffer header pool for camera video output port %s", pCameraVideoOutputPort->name);
		goto error;
	}

	{
		int num = mmal_queue_length(pPool->queue);
		int q;
		for (q=0; q<num; q++) {
			MMAL_BUFFER_HEADER_T *pBufh = mmal_queue_get(pPool->queue);
			if (!pBufh) {
				LOGE("Unable to get a required buffer %d from pool queue", q);
			}
			if (mmal_port_send_buffer(pCameraVideoOutputPort, pBufh)!= MMAL_SUCCESS) {
				LOGE("Unable to send a buffer to encoder output port (%d)", q);
			}
		}
	}

	status = mmal_port_parameter_set_boolean(pCameraVideoOutputPort, MMAL_PARAMETER_CAPTURE, 1);
	if (status != MMAL_SUCCESS) {
		LOGE("Failed to enable camera video output port : error %d", status);
	}

	LOGI("Camera ready");
	
	while (!getchar()) {
	}

error:
	if (pCameraComponent != NULL) {
		mmal_component_destroy(pCameraComponent);
		pCameraComponent = NULL;
	}

	if (pPreviewComponent != NULL) {
		mmal_component_destroy(pPreviewComponent);
		pPreviewComponent = NULL;
	}

	if (pFile != NULL) {
		fclose(pFile);
	}

	return 0;
}
