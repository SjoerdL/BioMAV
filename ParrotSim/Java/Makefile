BINDIR:=bin/
SRCDIR:=src/

JAVADEPS:=$(shell cd src ; find -name '*.java')
JAVACOMPILER?=javac

.PHONY: all clean

all: $(addsuffix .class,$(addprefix $(BINDIR),$(basename $(JAVADEPS))))

$(BINDIR)%.class: $(SRCDIR)%.java
	@-mkdir $(BINDIR)
	cd $(BINDIR) && $(JAVACOMPILER) -sourcepath $(realpath $(SRCDIR)) -d $(realpath $(BINDIR)) $(addprefix ../$(SRCDIR),$(JAVADEPS))
	
clean:
	-rm $(addsuffix .class,$(addprefix $(BINDIR),$(basename $(JAVADEPS))))
