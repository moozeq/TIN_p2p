# Compiler
CC     = g++

# Include directory
INC = -I./include -L/usr/lib

# Source files directory
SRC_DIR = src

# Compiler flags
CFLAGS = -Wall $(INC) -std=c++11 -pthread

# Linker flags
LIB = -lssl -lcrypto
#
# Project files
#
SRCS = $(SRC_DIR)/TIN_p2p.cpp $(SRC_DIR)/NodeInfo.cpp $(SRC_DIR)/NetMainThread.cpp \
 $(SRC_DIR)/MessageFrames.cpp $(SRC_DIR)/NetUtils.cpp $(SRC_DIR)/TcpMainService.cpp $(SRC_DIR)/FilesTableReceive.cpp \
 $(SRC_DIR)/ReceiveFileTcp.cpp $(SRC_DIR)/AddFile.cpp $(SRC_DIR)/GetFile.cpp $(SRC_DIR)/SendFileTcp.cpp 

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
all: prep debug

#
# Debug rules
#
debug: $(DBGEXE)

$(DBGEXE): $(DBGOBJS)
	$(CC) $(CFLAGS) $(DBGCFLAGS) -o $(DBGEXE) $^ $(LIB)

$(DBGDIR)/%.o: %.cpp
	$(CC) -c $(CFLAGS) $(DBGCFLAGS) -o $@ $<

#
# Release rules
#
release: $(RELEXE)

$(RELEXE): $(RELOBJS)
	$(CC) $(CFLAGS) $(RELCFLAGS) -o $(RELEXE) $^ $(LIB)


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
	