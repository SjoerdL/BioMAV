COMPILER=g++

BUILDDIR=../bin/javasrc/

JAVA_CLASS_PACKAGE_PREFIX=nl.ru.ai.projects.parrot.dronecontrol.simulator.
JAVA_CLASS_PATH=../Java/bin/

INCLUDES+=../src

CFLAGS+=

JAVAH?=javah

.SECONDARY: 

$(BUILDDIR)%.o: %.cpp %.hpp
	$(COMPILER) $(CFLAGS) -c -o $@ $(filter %.cpp,$^) $(addprefix -I,$(INCLUDES))

%.hpp: $(JAVA_CLASS_PATH)$(subst .,/,$(JAVA_CLASS_PACKAGE_PREFIX))%.class
	$(JAVAH) -classpath $(JAVA_CLASS_PATH):$(BASE_JAR) -o $@ $(addprefix $(JAVA_CLASS_PACKAGE_PREFIX),$(basename $(notdir $^)))
