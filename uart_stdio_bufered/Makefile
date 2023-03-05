# Copyright (c) 2022 Nestor D. Pereira Neto

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.


TOOLCHAIN = arm-none-eabi-
CC        = $(TOOLCHAIN)gcc
CXX       = $(TOOLCHAIN)g++
AS        = $(TOOLCHAIN)as
LD        = $(TOOLCHAIN)ld
OBJCOPY   = $(TOOLCHAIN)objcopy
AR        = $(TOOLCHAIN)ar
SIZE      = $(TOOLCHAIN)size

# GCC flags
#--------------------
CPUFLAG  = -mthumb -mcpu=cortex-m4
FPUFLAG  = -mfpu=fpv4-sp-d16 -mfloat-abi=hard
WFLAG    = -Wall 
WFLAG   += -Wextra

CFLAGS   = $(CPUFLAG) 
CFLAGS  += $(WFLAG) 
CFLAGS  += $(FPUFLAG)
CFLAGS  += -std=c99 
CFLAGS  += -MD
CFLAGS  += -pedantic
CFLAGS  += -nostdlib
CFLAGS  += -ffunction-sections 
CFLAGS  += -fdata-sections 
CFLAGS  += -specs=nosys.specs
CFLAGS  += -DPART_TM4C123GH6PM
CFLAGS  += -DUART_BUFFERED

DEBFLAG  = -g -DDEBUG

# Directories variables 
#---------------------
OBJ_DIR     = obj/
SRC_DIR     = src/

DRIVERLIB_UTILS_DIR  = $(TIVAWARE_DIR)/utils/
DRIVERLIB_UTILS_OBJS = uartstdio.o

SRC_SOURCES          = $(shell ls $(SRC_DIR)*.c)
SRC_OBJS             = $(patsubst $(SRC_DIR)%,$(OBJ_DIR)%,$(SRC_SOURCES:.c=.o))

OBJS  = $(addprefix $(OBJ_DIR), $(DRIVERLIB_UTILS_OBJS)) 
OBJS  += $(SRC_OBJS)

# Get the location of libgcc.a, libc.a and libm.a from the GCC front-end.
#---------------------
LIBGCC   := ${shell ${CC} ${CFLAGS} -print-libgcc-file-name}
LIBC     := ${shell ${CC} ${CFLAGS} -print-file-name=libc.a}
LIBM     := ${shell ${CC} ${CFLAGS} -print-file-name=libm.a}
LIBNOSYS := ${shell ${CC} ${CFLAGS} -print-file-name=libnosys.a}

# Include paths to be passed to $(CC) where necessary
#---------------------
INC_DIR       = include/
INC_TIVAWARE  = $(TIVAWARE_DIR)/
INC_FLAGS     = -I $(SRC_DIR) -I $(INC_DIR) -I $(INC_TIVAWARE)


# Definition of the linker script and final targets
#---------------------
LINKER_SCRIPT = tm4c123gh6pm.lds
ELF_IMAGE     = image.elf
TARGET_IMAGE  = image.bin

# Make rules:
#---------------------
print-%  : ; @echo $* = $($*)

all : $(TARGET_IMAGE) size

rebuild : clean all

$(TARGET_IMAGE) : $(OBJ_DIR) $(ELF_IMAGE)
	$(OBJCOPY) -O binary $(word 2,$^) $@

$(OBJ_DIR) :
	mkdir -p $@

# Linker
$(ELF_IMAGE) : $(OBJS) $(LINKER_SCRIPT)
	$(LD) -L $(OBJ_DIR) -L $(TIVAWARE_DIR)/driverlib/gcc -T $(LINKER_SCRIPT) $(OBJS) --gc-sections -o $@ -ldriver '$(LIBC)' '$(LIBGCC)' '$(LIBM)' '$(LIBNOSYS)'

debug : _debug_flags all

debug_rebuild : _debug_flags rebuild

_debug_flags :
	$(eval CFLAGS += $(DEBFLAG))

# Tivaware driverlib utils
$(OBJ_DIR)%.o : $(DRIVERLIB_UTILS_DIR)%.c
	$(CC) -c $(CFLAGS) $(INC_FLAGS) $< -o $@

# Main Code
$(OBJ_DIR)%.o : $(SRC_DIR)%.c
	$(CC) -c $(CFLAGS) $(INC_FLAGS) $< -o $@

# Size code:
#---------------------
size : 
	$(SIZE) $(ELF_IMAGE)


# Cleanup directives:
#---------------------
clean_obj :
	$(RM) -r $(OBJ_DIR)

clean_intermediate : clean_obj
	$(RM) *.elf
	$(RM) *.img
	
clean : clean_intermediate
	$(RM) *.bin

# Short help instructions:
#---------------------
help :
	@echo
	@echo Valid targets:
	@echo - all: builds missing dependencies and creates the target image \'$(TARGET_IMAGE)\'.
	@echo - rebuild: rebuilds all dependencies and creates the target image \'$(TARGET_IMAGE)\'.
	@echo - debug: same as \'all\', also includes debugging symbols to \'$(ELF_IMAGE)\'.
	@echo - debug_rebuild: same as \'rebuild\', also includes debugging symbols to \'$(ELF_IMAGE)\'.
	@echo - clean_obj: deletes all object files, only keeps \'$(ELF_IMAGE)\' and \'$(TARGET_IMAGE)\'.
	@echo - clean_intermediate: deletes all intermediate binaries, only keeps the target image \'$(TARGET_IMAGE)\'.
	@echo - clean: deletes all intermediate binaries, incl. the target image \'$(TARGET_IMAGE)\'.
	@echo - help: displays these help instructions.
	@echo


.PHONY :  all rebuild clean clean_intermediate clean_obj debug debug_rebuild _debug_flags help size
