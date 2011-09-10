DEPS = nl.ru.ai.projects.parrot.dronecontrol.groundstation.GroundStation \
	nl.ru.ai.projects.parrot.dronecontrol.groundstation.SDLDroneJoystick

TARGET = ../javasrc

.PHONY: all

all:
	cd bin; javah -jni -force -d ../$(TARGET) $(DEPS)

