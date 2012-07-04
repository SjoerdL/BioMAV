## Makefile for vision testing
PROGRAM = libPoleDetector.so

## libraries
LIBS=GL GLU glut highgui boost_thread
LDFLAGS=

## source files
include dependencies
srcMain+=jni/Jni.cpp

#-- add directory structure
SRCMAIN=$(addprefix ,$(srcMain))
SRCINPUT=$(addprefix input/,$(srcInput))
SRCMISCELLANEOUS=$(addprefix miscellaneous/,$(srcMiscellaneous))
SRCPREPROCESSING=$(addprefix preprocessing/,$(srcPreprocessing))
SRCPROCESSING=$(addprefix processing/,$(srcProcessing))
SRCRECOGNITION=$(addprefix recognition/,$(srcRecognition))
SRCOUTPUT=$(addprefix output/,$(srcOutput))
#-- gather source and objects
srcAll=$(SRCMAIN) $(SRCINPUT) $(SRCMISCELLANEOUS) $(SRCPREPROCESSING) $(SRCPROCESSING) $(SRCRECOGNITION) $(SRCOUTPUT)
CCSOURCES=$(addprefix src/,$(srcAll))
CCOBJECTS=$(CCSOURCES:.cpp=.o)
#-- something
CCFLAGS=-Wall -fPIC -fopenmp `pkg-config --cflags opencv` -I/usr/lib/jvm/java-6-openjdk/include/ -I/usr/lib/jvm/java-6-openjdk/include/linux/ $(CFLAGS)
CC=g++

.PHONY: depend clean

all: $(PROGRAM)

$(PROGRAM): $(CCOBJECTS)
	$(CC) $(CCFLAGS) -shared -o $(PROGRAM) $(CCOBJECTS) $(addprefix -l,$(LIBS))

.cpp.o:
	$(CXX) $(CCFLAGS) $(addprefix -l,$(LIBS)) -c $< -o $@

clean:
	$(RM) -f $(CCOBJECTS) $(PROGRAM)

run:
	./$(PROGRAM)

depend: $(SRCS)
	makedepend $(INCLUDES) $^
