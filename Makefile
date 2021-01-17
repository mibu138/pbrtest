CC = gcc
GLC = glslc

CFLAGS = -Wall -Wno-missing-braces -Wno-attributes -fPIC
LDFLAGS = -L/opt/hfs18.0/dsolib -L$(HOME)/lib
INFLAGS = -I$(HOME)/dev
LIBS = -lm -lcoal -ltanto -lvulkan -lxcb -lxcb-keysyms -lfreetype
GLFLAGS = --target-env=vulkan1.2
BIN = bin
LIB = $(HOME)/lib
LIBNAME = pbrtest

O = build
GLSL = shaders
SPV  = shaders/spv

NAME = pbrtest

DEPS =  \
		game.h \
		render.h \
		pbrtest.h \
		common.h \

SHDEPS = \
		shaders/common.glsl

OBJS =  \
		$(O)/game.o \
		$(O)/render.o \
		$(O)/pbrtest.o \

debug: CFLAGS += -g -DVERBOSE=1
debug: all

release: CFLAGS += -DNDEBUG -O3
release: all

all: coal tanto bin lib tags shaders

FRAGS := $(patsubst %.frag,$(SPV)/%-frag.spv,$(notdir $(wildcard $(GLSL)/*.frag)))
VERTS := $(patsubst %.vert,$(SPV)/%-vert.spv,$(notdir $(wildcard $(GLSL)/*.vert)))

shaders: $(FRAGS) $(VERTS)

clean: 
	rm -f $(O)/* $(LIB)/$(LIBNAME) $(BIN)/* $(SPV)/*

tags:
	ctags -R .

.PHONY: tanto
tanto:
	make -C tanto/ 

.PHONY: coal
coal:
	make -C coal

bin: main.c $(OBJS) $(DEPS) shaders
	$(CC) $(CFLAGS) $(INFLAGS) $(LDFLAGS) $(OBJS) $< -o $(BIN)/$(NAME) $(LIBS)

lib: $(OBJS) $(DEPS) shaders
	$(CC) -shared -o $(LIB)/lib$(LIBNAME).so $(OBJS)

staticlib: $(OBJS) $(DEPS) shaders
	ar rcs $(LIB)/lib$(NAME).a $(OBJS)

$(O)/%.o:  %.c $(DEPS)
	$(CC) $(CFLAGS) $(INFLAGS) -c $< -o $@

$(SPV)/%-vert.spv: $(GLSL)/%.vert $(SHDEPS)
	$(GLC) $(GLFLAGS) $< -o $@

$(SPV)/%-frag.spv: $(GLSL)/%.frag $(SHDEPS)
	$(GLC) $(GLFLAGS) $< -o $@

$(SPV)/%-rchit.spv: $(GLSL)/%.rchit $(SHDEPS)
	$(GLC) $(GLFLAGS) $< -o $@

$(SPV)/%-rgen.spv: $(GLSL)/%.rgen $(SHDEPS)
	$(GLC) $(GLFLAGS) $< -o $@

$(SPV)/%-rmiss.spv: $(GLSL)/%.rmiss $(SHDEPS)
	$(GLC) $(GLFLAGS) $< -o $@
