#Makefile to simulate bubble algorithm with SCoPE

CXX = g++
CFLAGS = -g -c -O0
SCOPE_CXX = scope-g++
SCOPE_FLAGS = --scope-method=op-cost #--scope-preserve-files --scope-verbose	--scope-crosscompiler=arm-gp2x-linux-gcc --scope-method=asm-sentences 

LIB_DIR = -L. -L$(SCOPE_HOME)/scope -L$(SYSTEMC)/lib-linux -L$(SCOPE_HOME)/tinyxml
LIB = -lscope -ltinyxml -lsystemc -lpthread -lrt
OUT = bubble.x

LOCAL_INC_DIRS =	$(SCOPE_HOME)/scope 					\
					$(SCOPE_HOME)/scope/hal 				\
					$(SCOPE_HOME)/scope/rtos/api/posix		\
					$(SCOPE_HOME)/scope/rtos/api/ucos 		\
					$(SCOPE_HOME)/scope/rtos/drivers 		\
					$(SCOPE_HOME)/scope/rtos/kernel 		\
					$(SCOPE_HOME)/scope/rtos/low_level 		\
					$(SCOPE_HOME)/scope/rtos/qt_interface	\
					$(SCOPE_HOME)/scope/rtos/utils

INC_DIRS =	\
			$(SCOPE_HOME)/scope/sicosys/SC_Simul \
			$(SYSTEMC)/include \
			$(SCOPE_HOME)/TLM2/include/tlm \
			$(LOCAL_INC_DIRS) \

INC_DIR = $(addprefix -I,$(INC_DIRS))

#Files
SRCS = idle.cpp
OBJS = $(SRCS:.cpp=.o)

#Commands
.PHONY: all clean distclean

all: $(OUT)

$(OUT): $(OBJS) sc_main.o
	$(CXX) $(LIB_DIR) $^ -o $@ $(LIB)

# Implicit rules
%.o: %.cpp
	$(SCOPE_CXX) $(SCOPE_FLAGS) $(CFLAGS) $(INC_DIR) $^ -o $@

# Compile sc_main.cpp with standar g++
sc_main.o : sc_main.cpp
	$(CXX) $(CFLAGS) $(INC_DIR) $^ -o $@

clean:
	rm -f $(OBJS) sc_main.o *.ii prsd_* *.xml *.s uc_header.h

distclean: clean
	rm -f $(OUT)
