#==========================================================================
#	Makefile
#	Author: Juan Castillo, David Quijano
#	Date: lun dic 3 2007
#	Description:
#==========================================================================
#  The following code is derived, directly or indirectly, from SCoPE,
#  released June 30, 2008. Copyright (C) 2006 Design of Systems on Silicon (DS2)
#  The Initial Developer of the Original Code is the University of Cantabria
#  for Design of Systems on Silicon (DS2)
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#  For more information about SCOPE you can visit
#  http://www.teisa.unican.es/scope or write an e-mail to
#  scope@teisa.unican.es or a letter to SCoPE, GIM - TEISA, University
#  of Cantabria, AV. Los Castros s/n, ETSIIT, 39005, Santander, Spain
#==========================================================================

# SCoPE options:
SCOPE_CXX = scope-g++
SCOPE_FLAGS = --scope-cpu=arm926t --scope-method=op-cost
#SCOPE_FLAGS = --scope-cpu=arm926t --scope-crosscompiler=arm-elf-gcc --scope-method=asm-sentences --scope-nodcache
SCOPE_INC_DIR =	-I$(SCOPE_HOME)/scope \
				-I$(SYSTEMC)/include \
				-I$(SCOPE_HOME)/TLM2/include/tlm

# Compiler options:
CXX = g++
DEBUG = -g
OPT = -O0
CFLAGS = $(DEBUG) $(OPT) -c
LIB_DIR = -L$(SCOPE_HOME)/scope -L$(SCOPE_HOME)/tinyxml -L$(SYSTEMC)/lib-linux
LIB = -lscope -ltinyxml -lsystemc -lpthread -lrt

OBJS =  src/b_frame.o \
		src/cabac.o \
		src/decoder.o \
		src/image.o \
		src/loopFilter.o \
		src/mv-search.o \
		src/refbuf.o \
		src/biariencode.o \
		src/configfile.o \
		src/filehandle.o \
		src/leaky_bucket.o \
		src/lencod.o \
		src/macroblock.o \
		src/rdopt.o \
		src/rtp.o \
		src/block.o \
		src/header.o \
		src/mbuffer.o \
		src/rdopt_coding_state.o \
		src/uvlc.o \
		src/memalloc.o \
		src/lencod.o

OUT = coder.x

# Link:
coder: sc_main.o $(OBJS)
	$(CXX) $(LIB_DIR) $^ -o $(OUT) $(LIB)

# Parse and compile software application files:
%.o: %.cpp
	$(SCOPE_CXX) $(SCOPE_FLAGS) $(CFLAGS) $(INC_DIR) $^ -o $@

# Compile sc_main.cpp with standar g++
sc_main.o : sc_main.cpp
	$(CXX) $(CFLAGS) $(SCOPE_INC_DIR) $^ -o $@

# Clean:
.PHONY: clean
.PHONY: distclean
clean:
	rm -rf $(OBJS) sc_main.o

distclean: clean
	rm -rf $(OUT)

# File dependencies:
b_frame.o: b_frame.cpp elements.h b_frame.h refbuf.h
biariencode.o: biariencode.cpp global2.h biariencode.h
block.o: block.cpp contributors.h block.h refbuf.h
cabac.o: cabac.cpp cabac.h
configfile.o: configfile.cpp global2.h configfile.h
decoder.o: decoder.cpp global2.h refbuf.h rdopt.h
filehandle.o: filehandle.cpp contributors.h global2.h header.h rtp.h
header.o: header.cpp elements.h header.h rtp.h mbuffer2.h
image.o: image.cpp contributors.h global2.h image.h refbuf.h mbuffer2.h
leaky_bucket.o: leaky_bucket.cpp contributors.h global2.h
lencod.o: lencod.cpp contributors.h global.h configfile2.h leaky_bucket.h memalloc.h mbuffer.h
loopFilter.o: loopFilter.cpp global2.h
macroblock.o: macroblock.cpp contributors.h elements.h macroblock.h refbuf.h
mbuffer.o: mbuffer.cpp global2.h mbuffer2.h memalloc.h
memalloc.o: memalloc.cpp memalloc.h
mv-search.o: mv-search.cpp contributors.h global2.h mv-search.h refbuf.h
rdopt_coding_state.o: rdopt_coding_state.cpp rdopt_coding_state.h
rdopt.o: rdopt.cpp rdopt.h refbuf.h macroblock.h
refbuf.o: refbuf.cpp refbuf.h
rtp.o: rtp.cpp rtp.h elements.h defines.h header.h global2.h
uvlc.o: uvlc.cpp elements.h macroblock.h
