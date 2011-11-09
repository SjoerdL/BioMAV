BASE_JAR ?= BioMAV_base.jar

JAVA_COMPILER ?= javac
JAVADOC_COMPILER ?= javadoc
JAR_COMPRESSOR ?= jar

BIN_DIR ?= bin

DOC_DIR := doc
SRC_DIR := src

JAR_PACKAGE := $(BIN_DIR)/BioMAV_JavaDroneControl.jar
DOC_FILE := $(DOC_DIR)/index.html

SRC_FILES := $(shell find $(SRC_DIR) -type f -name '*.java')

.PHONEY: all clean

all: $(JAR_PACKAGE)

$(JAR_PACKAGE): $(SRC_FILES)
	-mkdir -p $(BIN_DIR)
	$(JAVA_COMPILER) -d $(BIN_DIR) -cp $(BASE_JAR) $(SRC_FILES)
	cd $(BIN_DIR); $(JAR_COMPRESSOR) cf ../$(JAR_PACKAGE) `find -maxdepth 1 -type d | grep -E '.{2,}'`

clean:
	rm -r $(BIN_DIR)
