GENERAL_DEPS:=luagl_util.c

LUAGL_DEPS:=$(basename luagl.c $(GENERAL_DEPS))
LUAGLU_DEPS:=$(basename luaglu.c $(GENERAL_DEPS))

INCLUDES=../include /usr/include/lua5.1
INCLUDES+=$(LUA_INCLUDE)

LIBS=lua5.1 GL GLU

COMPILER=gcc
ARCHIVER=ar
CFLAGS=-fPIC

.PHONY: do_all clean
do_all: libluagl.a libluaglu.a
clean: 
	-rm *.o
	-rm libluagl.a
	-rm libluaglu.a

libluagl.a: $(addsuffix .o,$(LUAGL_DEPS))
	$(ARCHIVER) rcs $@ $^
libluaglu.a: $(addsuffix .o,$(LUAGLU_DEPS))
	$(ARCHIVER) rcs $@ $^

%.o: %.c
	$(COMPILER) -c $(CFLAGS) $(addprefix -I,$(INCLUDES)) $(CFLAGS)  $(addprefix -l,$(LIBS)) -o $@ $^
