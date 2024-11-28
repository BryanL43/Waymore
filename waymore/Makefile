targets = brain.c waymoreLib.c senses/ir.c cognition/PID.c control/motors.c control/lib/PCA9685.c control/lib/DEV_Config.c

waymore.o: $(targets)
	sudo gcc $(targets) -o waymore -lbcm2835 -lm -D USE_BCM2835_LIB

run: waymore.o
	sudo ./waymore

clean:
	sudo rm waymore