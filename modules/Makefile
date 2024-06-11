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

SOURCES := $(wildcard $(SRCDIR)/*.cpp)
INCLUDES := $(wildcard $(INCDIR)/*.hpp)
OBJECTS := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

$(BINDIR)/$(TARGET): clean $(OBJECTS)
		@$(LD) $(OBJECTS) $(LFLAGS) $(LIBS) $(LIBDIR) -o $@

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
		@$(CC) $(CFLAGS) $(INCDIR) -c $< -o $@

.PHONY: clean
clean:
		@rm -rf obj
		@rm -f $(TARGET)
		@mkdir -p obj

compile: $(BINDIR)/$(TARGET)

all: compile run
