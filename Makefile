CC=g++
#CFLAGS=-Wall -Wextra -IInc -std=c++17 -pthread -Wno-unused-parameter -O0 -g  -fsanitize=address
#LDFLAGS=-lpthread -lssl -lcrypto -g  -fsanitize=address
CFLAGS=-Wall -Wextra -IInc -std=c++17 -pthread -Wno-unused-parameter -O3
LDFLAGS=-lpthread -lssl -lcrypto 
SRC=$(wildcard Src/*.cpp)
OBJ=$(SRC:Src/%.cpp=Build/%.o)
EXEC=Bin/pokerplanning

# PowerPC compiler and flags
POWERPC_CC=powerpc-linux-gnu-g++
POWERPC_CFLAGS=-Wall -Wextra -IInc -mcpu=powerpc -mabi=altivec

# Object files for PowerPC
POWERPC_OBJ=$(SRC:Src/%.cpp=Build/powerpc/%.o)

# Executable for PowerPC
POWERPC_EXEC=Bin/pokerplanning

all: $(EXEC)

$(EXEC): $(OBJ)
	@ echo [x86] Linking $@
	@ $(CC) $(OBJ) $(LDFLAGS)  -o $@

Build/%.o: Src/%.cpp
	@ echo [x86] Compiling $< to $@
	@ $(CC) $(CFLAGS) -c $< -o $@

# PowerPC rule
##powerpc: $(POWERPC_EXEC)
#
#$(POWERPC_EXEC): $(POWERPC_OBJ)
#	@ echo [PPC] Linking $@
#	@ $(POWERPC_CC) $(POWERPC_CFLAGS) $(LDFLAGS) $(POWERPC_OBJ) -o $@
#
#Build/powerpc/%.o: Src/%.cpp
#	@ echo [PPC] Compiling $< to $@
#	@ $(POWERPC_CC) $(POWERPC_CFLAGS) -c $< -o $@
#
clean:
	rm -rf Build/*
	rm -f $(EXEC)
	rm -f $(POWERPC_EXEC)