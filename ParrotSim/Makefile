include Makefile-deps

STANDALONE_SOURCES=main.cpp
JNI_SOURCES=

SOURCEDIR=src/
BUILDDIR=bin/

LUADIR=luapkg/
LUA_OBJ_BUILDDIR=$(BUILDDIR)$(LUADIR)
LUA_OBJS=$(addprefix $(LUA_OBJ_BUILDDIR),$(addsuffix .o,$(basename $(LUA_PACKAGES))))

JAVADIR=javasrc/
JAVA_OBJ_BUILDDIR=$(BUILDDIR)$(JAVADIR)

STANDALONE_TARGET=parrotSimulator
JNI_LIB_TARGET=Java/libParrotSimulator.so

COMPILER=g++

INCLUDES+=luapkg /usr/include/lua5.1/ $(JAVA_BASE_PATH)/include/ $(JAVA_BASE_PATH)/include/linux/
LIBS+= SDL ode GL GLU IL ILUT lua5.1 tolua++5.1 blas boost_thread
LIBPATHS+=


CFLAGS+=-DILUT_USE_OPENGL -fPIC
#-DDEBUG

include Makefile-main
