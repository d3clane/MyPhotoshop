CC := g++

CFLAGS = -D _DEBUG -std=c++17 -O3 -Wall -Wextra -Weffc++ \
		   -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts 		  \
		   -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal      \
		   -Wformat-nonliteral -Wformat-security -Wformat=2 \
		   -Wnon-virtual-dtor -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self \
		   -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo		  \
		   -Wstrict-overflow=2 \
		   -Wsuggest-override -Wswitch-default -Wswitch-enum 		  \
		   -Wundef -Wunreachable-code -Wunused -Wvariadic-macros   \
		   -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs 			  \
		   -Wstack-protector -fsized-deallocation -fstack-protector -fstrict-overflow 	  \
		   -fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-protector  						  \
		   -fPIE -Werror=vla #-fsanitize=address				  

OUT_O_DIR := build
COMMONINC := -I./include -I./ -I./plugins

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
PLUGIN_LIB_NAMES := bars/ps_bar.cpp canvas/canvas.cpp interpolation/src/catmullRom.cpp \
					interpolation/src/interpolator.cpp windows/windows.cpp scrollbar/scrollbar.cpp \
					instrumentBar/actions.cpp instrumentBar/instrumentBar.cpp toolbar/toolbarButton.cpp	\
					filters/filters.cpp
PLUGIN_LIB = $(addprefix plugins/pluginLib/, $(PLUGIN_LIB_NAMES))

CPPOBJ := $(addprefix $(OUT_O_DIR)/,$(CPPSRC:.cpp=.o))
DEPS = $(CPPOBJ:.o=.d)


DYLIBS_NAMES = libapi_photoshop.dylib lib_canvas.dylib lib_toolbar.dylib lib_optionsBar.dylib \
			   lib_brush.dylib lib_eraser.dylib \
			   lib_line.dylib lib_ellipse.dylib lib_rectangle.dylib \
			   #lib_negative_filter.dylib lib_blur_filter.dylib
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

$(PS_API_LIB): src/api/api_photoshop.cpp src/api/api_sfm.cpp src/api/api_system.cpp src/sfm/sfm_impl.cpp \
			   src/api/api_actions.cpp src/api/api_bar.cpp plugins/pluginLib/bars/ps_bar.cpp \
			   plugins/pluginLib/windows/windows.cpp
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LDFLAGS)

$(DYLIB_DIR)/lib_brush.dylib: plugins/brush/brush.cpp \
	plugins/pluginLib/interpolation/src/catmullRom.cpp plugins/pluginLib/interpolation/src/interpolator.cpp \
	plugins/pluginLib/windows/windows.cpp plugins/pluginLib/bars/ps_bar.cpp \
	plugins/pluginLib/splineDraw/splineDrawButton.cpp \
	plugins/pluginLib/toolbar/toolbarButton.cpp $(PS_API_LIB)
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LDFLAGS)

$(DYLIB_DIR)/lib_eraser.dylib: plugins/eraser/eraser.cpp \
	plugins/pluginLib/interpolation/src/catmullRom.cpp plugins/pluginLib/interpolation/src/interpolator.cpp \
	plugins/pluginLib/windows/windows.cpp plugins/pluginLib/bars/ps_bar.cpp \
	plugins/pluginLib/splineDraw/splineDrawButton.cpp \
	plugins/pluginLib/toolbar/toolbarButton.cpp $(PS_API_LIB)
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LDFLAGS)

$(DYLIB_DIR)/lib_canvas.dylib: plugins/canvas/canvas.cpp \
	plugins/pluginLib/interpolation/src/catmullRom.cpp plugins/pluginLib/interpolation/src/interpolator.cpp \
	plugins/pluginLib/windows/windows.cpp plugins/pluginLib/scrollbar/scrollbar.cpp $(PS_API_LIB)
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LDFLAGS)

$(DYLIB_DIR)/lib_toolbar.dylib: plugins/toolbar/toolbar.cpp plugins/pluginLib/bars/ps_bar.cpp \
	plugins/pluginLib/windows/windows.cpp $(PS_API_LIB)
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LDFLAGS)

$(DYLIB_DIR)/lib_optionsBar.dylib: plugins/optionsBar/optionsBar.cpp plugins/pluginLib/bars/ps_bar.cpp \
	plugins/pluginLib/windows/windows.cpp $(PS_API_LIB)
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LDFLAGS)

$(DYLIB_DIR)/lib_spray.dylib: plugins/spray/spray.cpp $(PLUGIN_LIB) $(PS_API_LIB)
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LDFLAGS)

$(DYLIB_DIR)/lib_line.dylib: plugins/line/line.cpp \
	plugins/pluginLib/interpolation/src/catmullRom.cpp plugins/pluginLib/interpolation/src/interpolator.cpp \
	plugins/pluginLib/windows/windows.cpp plugins/pluginLib/bars/ps_bar.cpp \
	plugins/pluginLib/canvas/canvas.cpp \
	plugins/pluginLib/toolbar/toolbarButton.cpp $(PS_API_LIB)
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LDFLAGS)

$(DYLIB_DIR)/lib_ellipse.dylib: plugins/ellipse/ellipse.cpp \
	plugins/pluginLib/interpolation/src/catmullRom.cpp plugins/pluginLib/interpolation/src/interpolator.cpp \
	plugins/pluginLib/windows/windows.cpp plugins/pluginLib/bars/ps_bar.cpp \
	plugins/pluginLib/canvas/canvas.cpp \
	plugins/pluginLib/toolbar/toolbarButton.cpp $(PS_API_LIB)
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LDFLAGS)

$(DYLIB_DIR)/lib_rectangle.dylib: plugins/rectangle/rectangle.cpp \
	plugins/pluginLib/interpolation/src/catmullRom.cpp plugins/pluginLib/interpolation/src/interpolator.cpp \
	plugins/pluginLib/windows/windows.cpp plugins/pluginLib/bars/ps_bar.cpp \
	plugins/pluginLib/canvas/canvas.cpp \
	plugins/pluginLib/toolbar/toolbarButton.cpp $(PS_API_LIB)
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LDFLAGS)

$(DYLIB_DIR)/lib_negative_filter.dylib : plugins/negativeFilter/negFilter.cpp $(PLUGIN_LIB) $(PS_API_LIB)
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LDFLAGS)

$(DYLIB_DIR)/lib_blur_filter.dylib : plugins/blurFilter/blurFilter.cpp $(PLUGIN_LIB) $(PS_API_LIB)
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