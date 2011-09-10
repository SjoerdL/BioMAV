COMPILER=g++

BUILDDIR=../bin/luapkg/

INCLUDES+=

CFLAGS+=

.SECONDARY: 

$(BUILDDIR)%.o: %.cpp
	$(COMPILER) $(CFLAGS) -c -o $@ $^ $(addprefix -I,$(INCLUDES))

%.cpp: %.pkg
	sh compilePackages.sh $^
