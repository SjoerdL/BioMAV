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

INCLUDES+=luapkg /usr/include/lua5.1/ /usr/lib/jvm/java-6-openjdk/include/ /usr/lib/jvm/java-6-openjdk/include/linux/
LIBS+= SDL ode GL GLU IL ILUT lua tolua++ blas boost_thread
LIBPATHS+=


CFLAGS+=-DILUT_USE_OPENGL -fPIC
#-DDEBUG

include Makefile-main
