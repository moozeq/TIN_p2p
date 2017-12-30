#CXXFLAGS =	-O2 -g -Wall -fmessage-length=0
#
#OBJS =		TIN_p2p.o
#
#LIBS =
#
#TARGET =	TIN_p2p
#
#$(TARGET):	$(OBJS)
#	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)
#
#all:	$(TARGET)
#
#clean:
#	rm -f $(OBJS) $(TARGET)


# Compiler
CC     = g++

# Include directory
INC = -I./include

# Source files directory
SRC_DIR = src

# Compiler flags
CFLAGS = -Wall $(INC) -std=c++11

#
# Project files
#
SRCS = $(SRC_DIR)/TIN_p2p.cpp

#
# Object files
#
OBJS = $(SRCS:.cpp=.o)
EXE = p2p

#
# Debug build settings
#
DBGDIR = debug
DBGEXE = $(DBGDIR)/$(EXE)
DBGOBJS = $(addprefix $(DBGDIR)/, $(OBJS))
DBGCFLAGS = -g -O0 -DDEBUG

#
# Release build settings
#
RELDIR = release
RELEXE = $(RELDIR)/$(EXE)
RELOBJS = $(addprefix $(RELDIR)/, $(OBJS))
RELCFLAGS = -O3 -DNDEBUG

.PHONY: all clean debug prep release remake

# Default build
all: prep release

#
# Debug rules
#
debug: $(DBGEXE)

$(DBGEXE): $(DBGOBJS)
	$(CC) $(CFLAGS) $(DBGCFLAGS) -o $(DBGEXE) $^

$(DBGDIR)/%.o: %.cpp
	$(CC) -c $(CFLAGS) $(DBGCFLAGS) -o $@ $<

#
# Release rules
#
release: $(RELEXE)

$(RELEXE): $(RELOBJS)
	$(CC) $(CFLAGS) $(RELCFLAGS) -o $(RELEXE) $^


$(RELDIR)/%.o: %.cpp
	$(CC) -c $(CFLAGS) $(RELCFLAGS) -o $@ $<

#
# Other rules
#
prep:
	@mkdir -p $(DBGDIR) $(RELDIR) $(DBGDIR)/src $(RELDIR)/src

remake: clean all

clean:
	rm -f $(RELOBJS) $(RELEXE) $(DBGOBJS) $(DBGEXE)
	