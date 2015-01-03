INC = -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/mmal -I/opt/vc/include/interface/vmcs_host/linux

all: test_mmal test_mmal_enc

debug.o: debug.c
	gcc $(INC) -c debug.c

test_mmal.o: test_mmal.c
	gcc $(INC) -c test_mmal.c

test_mmal_enc.o: test_mmal_enc.c
	gcc $(INC) -c test_mmal_enc.c

RaspiCamControl.o: RaspiCamControl.c
	gcc $(INC) -c RaspiCamControl.c

RaspiCLI.o: RaspiCLI.c
	gcc $(INC) -c RaspiCLI.c

test_mmal: debug.o test_mmal.o RaspiCamControl.o RaspiCLI.o
	gcc -L/opt/vc/lib/ -lvcos -lbcm_host -lmmal -lmmal_core -lmmal_util debug.o test_mmal.o RaspiCamControl.o RaspiCLI.o -o test_mmal

test_mmal_enc: debug.o test_mmal_enc.o RaspiCamControl.o RaspiCLI.o
	gcc -L/opt/vc/lib/ -lvcos -lbcm_host -lmmal -lmmal_core -lmmal_util debug.o test_mmal_enc.o RaspiCamControl.o RaspiCLI.o -o test_mmal_enc

clean:
	rm -f *.o test.yuv test.h264 test_mmal test_mmale_enc
