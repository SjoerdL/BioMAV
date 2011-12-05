JAVA_COMPILER ?= javac
JAVADOC_COMPILER ?= javadoc
JAR_COMPRESSOR ?= jar

BIN_DIR ?= bin

DOC_DIR := doc
SRC_DIR := src/Java

JAR_PACKAGE := $(BIN_DIR)/BioMAV_base.jar
DOC_FILE := $(DOC_DIR)/index.html

SRC_FILES := $(shell find . $(SRC_DIR) -type f -name '*.java')

.PHONEY: all doc clean

all: $(JAR_PACKAGE) doc

$(JAR_PACKAGE): $(SRC_FILES)
	-mkdir -p $(BIN_DIR)
	$(JAVA_COMPILER) -d $(BIN_DIR) $(SRC_FILES)
	cd $(BIN_DIR); $(JAR_COMPRESSOR) cf ../$(JAR_PACKAGE) `find -maxdepth 1 -type d | grep -E '.{2,}'`
	
doc: $(DOC_FILE)

$(DOC_FILE): $(SRC_FILES)
	mkdir -p $(DOC_DIR)
	$(JAVADOC_COMPILER) -d $(DOC_DIR) $(SRC_FILES)
	
clean:
	-rm -r $(BIN_DIR)	
	-rm -r $(DOC_DIR)
