SYSTEMC?=$(SYSTEMC_HOME)
TARGET?=test

# Compiler
CC=g++
CFLAGS=-Wall -I. -O3 -g -Wextra -Wunused-function

ifdef USE_CPP17
CFLAGS+=-std=c++17
endif

# Target
LD=g++
LFLAGS=-Wall -I. -lm -g
LIBS=-lsystemc -lm

# Source directories
SRCDIR=src
OBJDIR=obj
BINDIR=./
INCDIR=-I. -I./include -I$(SYSTEMC)/include -Ibasic_protocol -I$(SYSTEMC)/include/tlm_core/tlm_2
LIBDIR=-L. -L$(SYSTEMC)/lib-linux64

ifdef USING_TLM_TB_EN
EDGE_DIR=../edge-detector
GRAY_DIR=../rgb2gray
FILTER_DIR=../filter

SRCDIRS=$(SRCDIR) $(EDGE_DIR)/src $(GRAY_DIR)/src $(FILTER_DIR)/src
INCDIR+=-I$(EDGE_DIR)/include -I$(GRAY_DIR)/include -I$(FILTER_DIR)/include
endif # USING_TLM_TB_EN

ifdef INCLUDE_OPENCV
# Target
LIBS+=-lopencv_imgcodecs -lopencv_core -lopencv_highgui -lopencv_imgproc

# Source directories
INCDIR+=-I$(OPENCV_H_DIR)
LIBDIR+=-L$(OPENCV_SO_DIR)
else
ifdef INCLUDE_OPENCV_PKG
CFLAGS += $(shell pkg-config --cflags opencv4)
LFLAGS += $(shell pkg-config --libs opencv4)
endif # INCLUDE_OPENCV_PKG
endif # INCLUDE_OPENCV

ifndef USING_TLM_TB_EN
SOURCES := $(wildcard $(SRCDIR)/*.cpp)
INCLUDES := $(wildcard $(INCDIR)/*.hpp)
OBJECTS := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
else
SOURCES = $(wildcard *.cpp) $(foreach DIR, $(SRCDIRS), $(wildcard $(DIR)/*.cpp))
SOURCES_WITHOUT_PATH = $(notdir $(SOURCES))
INCLUDES := $(wildcard $(INCDIR)/*.hpp)
OBJECTS = $(SOURCES_WITHOUT_PATH:%.cpp=$(OBJDIR)/%.o)

VPATH = $(sort $(dir $(SOURCES)))
endif # USING_TLM_TB_EN

$(BINDIR)/$(TARGET): clean $(OBJECTS)
		@$(LD) $(OBJECTS) $(LFLAGS) $(LIBS) $(LIBDIR) -o $@

ifndef USING_TLM_TB_EN
$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
		@$(CC) $(CFLAGS) $(INCDIR) -c $< -o $@
else
$(OBJECTS): $(OBJDIR)/%.o : %.cpp
		@$(CC) $(CFLAGS) $(INCDIR) -c $< -o $@
endif # USING_TLM_TB_EN

.PHONY: clean
clean:
		@rm -rf obj
		@rm -f $(TARGET)
		@mkdir -p obj

compile: $(BINDIR)/$(TARGET)

all: compile run
