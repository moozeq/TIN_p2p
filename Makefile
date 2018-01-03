# Compiler
CC     = g++

# Include directory
INC = -I./include

# Source files directory
SRC_DIR = src

# Compiler flags
CFLAGS = -Wall $(INC) -std=c++11 -pthread

#
# Project files
#
SRCS = $(SRC_DIR)/TIN_p2p.cpp $(SRC_DIR)/NodeInfo.cpp $(SRC_DIR)/FileTransfer.cpp $(SRC_DIR)/AddNode.cpp $(SRC_DIR)/NetMainThread.cpp $(SRC_DIR)/common.cpp

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
	