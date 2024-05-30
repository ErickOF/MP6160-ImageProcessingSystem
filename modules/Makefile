SYSTEMC?=$(HOME)/systemc-2.3.3
TARGET?=test

# Compiler
CC=g++
CFLAGS=-Wall -I. -O3 -g -Wextra -Wunused-function

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

SOURCES := $(wildcard $(SRCDIR)/*.cpp)
INCLUDES := $(wildcard $(INCDIR)/*.hpp)
OBJECTS := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
FV_OBJECTS := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.i)

$(BINDIR)/$(TARGET): $(OBJECTS)
		@$(LD) $(OBJECTS) $(LFLAGS) $(LIBS) $(LIBDIR) -o $@
		@echo "Linked $@ completed!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
		@$(CC) $(CFLAGS) $(INCDIR) -c $< -o $@
		@echo "Compilation $< in $@"

fv_run: $(FV_OBJECTS)
		cbmc $(FV_OBJECTS) --function sc_main

$(FV_OBJECTS): $(OBJDIR)/%.i : $(SRCDIR)/%.cpp
		@$(CC) $(CFLAGS) $(INCDIR) -c $< -o $@
		@echo "FV compilation $< in $@"

.PHONY: clean
clean:
		@rm -f $(OBJECTS)
		@echo "Clean done."

all: $(BINDIR)/$(TARGET)
