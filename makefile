CC := g++

CFLAGS = -D _DEBUG -ggdb3 -std=c++17 -O3 -Wall -Wextra -Weffc++ \
		   -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts 		  \
		   -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal      \
		   -Wformat-nonliteral -Wformat-security -Wformat=2 \
		   -Wnon-virtual-dtor -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self \
		   -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo		  \
		   -Wstrict-overflow=2 \
		   -Wsuggest-override -Wswitch-default -Wswitch-enum 		  \
		   -Wundef -Wunreachable-code -Wunused -Wvariadic-macros   \
		   -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs 			  \
		   -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow 	  \
		   -fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-protector  						  \
		   -fPIE -Werror=vla #-fsanitize=address				  

OUT_O_DIR := build
COMMONINC := -I./include -I./ -I./plugins/pluginLib

LIB_INC   := -isystem/opt/homebrew/Cellar/sfml/2.6.1/include
LIB_LINK  := -L/opt/homebrew/Cellar/sfml/2.6.1/lib -lsfml-graphics -lsfml-window -lsfml-system

LDFLAGS   := $(LIB_LINK) -flat_namespace

PROGRAM_DIR  := $(OUT_O_DIR)/bin
PROGRAM_NAME := ps.out

TESTS = ./Tests

ROOT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

override CFLAGS += $(COMMONINC)
override CFLAGS += $(LIB_INC)

CPPSRC = src/main.cpp
PLUGIN_LIB_NAMES := bars/ps_bar.cpp canvas/canvas.cpp interpolation/src/catmullRom.cpp interpolation/src/interpolator.cpp windows/windows.cpp
PLUGIN_LIB = $(addprefix plugins/pluginLib/, $(PLUGIN_LIB_NAMES))

CPPOBJ := $(addprefix $(OUT_O_DIR)/,$(CPPSRC:.cpp=.o))
DEPS = $(CPPOBJ:.o=.d)


DYLIBS_NAMES = libapi_photoshop.dylib lib_canvas.dylib lib_toolbar.dylib lib_spray.dylib lib_brush.dylib lib_line.dylib lib_ellipse.dylib
DYLIB_DIR = libs
DYLIBS := $(addprefix $(DYLIB_DIR)/,$(DYLIBS_NAMES))
PS_API_LIB := $(DYLIB_DIR)/libapi_photoshop.dylib

.PHONY: all
all: dylibs $(PROGRAM_DIR)/$(PROGRAM_NAME)

dylibs: dylibs_create_path $(DYLIBS)

dylibs_create_path:
	@mkdir -p $(DYLIB_DIR)

$(PROGRAM_DIR)/$(PROGRAM_NAME): $(CPPOBJ) $(PS_API_LIB)
	@mkdir -p $(@D)
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

$(CPPOBJ) : $(OUT_O_DIR)/%.o : %.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(PS_API_LIB): src/api/api_photoshop.cpp src/api/api_sfm.cpp src/api/api_system.cpp src/sfm/sfm_impl.cpp
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LDFLAGS)

$(DYLIB_DIR)/lib_brush.dylib: plugins/brush/brush.cpp $(PLUGIN_LIB) $(PS_API_LIB)
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LDFLAGS)

$(DYLIB_DIR)/lib_canvas.dylib: plugins/canvas/canvas.cpp plugins/canvas/scrollbar.cpp $(PLUGIN_LIB) $(PS_API_LIB)
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LDFLAGS)

$(DYLIB_DIR)/lib_toolbar.dylib: plugins/toolbar/toolbar.cpp $(PLUGIN_LIB) $(PS_API_LIB)
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LDFLAGS)

$(DYLIB_DIR)/lib_spray.dylib: plugins/spray/spray.cpp $(PLUGIN_LIB) $(PS_API_LIB)
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LDFLAGS)

$(DYLIB_DIR)/lib_line.dylib: plugins/line/line.cpp $(PLUGIN_LIB) $(PS_API_LIB)
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LDFLAGS)

$(DYLIB_DIR)/lib_ellipse.dylib: plugins/ellipse/ellipse.cpp $(PLUGIN_LIB) $(PS_API_LIB)
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LDFLAGS)

#
#$(DEPS) : $(OUT_O_DIR)/%.d : %.cpp
#	@mkdir -p $(@D)
#	$(CC) -E $(CFLAGS) $< -MM -MT $(@:.d=.o) > $@

#
#TESTFILES=$(wildcard $(TESTS)/*.dat)
#
#.PHONY: testrun
#testrun: $(TESTFILES)

#.PHONY: $(TESTFILES)
#$(TESTFILES): $(OUT_O_DIR)/LC.x
#	@$(ROOT_DIR)/runtest.sh $@ $(OUT_O_DIR)/LC.x

.PHONY: clean
clean:
	rm -rf bin/ libs/
	#rm -rf $(CPPOBJ) $(DEPS) $(OUT_O_DIR)/*.x $(OUT_O_DIR)/*.log  
	#rm -rf $(DYLIB_DIR)

#NODEPS = clean

#ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
#include $(DEPS)
#endif