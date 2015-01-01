INC = -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/mmal -I/opt/vc/include/interface/vmcs_host/linux

test_mmal.o: test_mmal.c
	gcc $(INC) -c test_mmal.c

RaspiCamControl.o: RaspiCamControl.c
	gcc $(INC) -c RaspiCamControl.c

RaspiCLI.o: RaspiCLI.c
	gcc $(INC) -c RaspiCLI.c

test_mmal: test_mmal.o RaspiCamControl.o RaspiCLI.o
	gcc -L/opt/vc/lib/ -lvcos -lbcm_host -lmmal -lmmal_core -lmmal_util test_mmal.o RaspiCamControl.o RaspiCLI.o -o test_mmal

