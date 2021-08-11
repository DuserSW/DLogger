# Simple Makefile for small library


# Shell commands
RM := rm -rf
AR := ar rcs


# Pretty print functions
define print_cc
	$(if $(Q), @echo "[CC]        $(1)")
endef

define print_bin
	$(if $(Q), @echo "[BIN]       $(1)")
endef

define print_rm
	$(if $(Q), @echo "[RM]        $(1)")
endef

define print_ar
	$(if $(Q), @echo "[AR]        $(1)")
endef


# (Quiet or Verbose mode) type make V=1 to enable verbose mode
ifeq ("$(origin V)", "command line")
	Q :=
else
	Q ?= @
endif


# Directories
SDIR := ./src
IDIR := ./inc
TDIR := ./test
SCRIPT_DIR := ./scripts


# Files
SRC := $(wildcard $(SDIR)/*.c)

ASRC := $(SRC) $(wildcard $(ADIR)/*.c)
TSRC := $(SRC) $(wildcard $(TDIR)/*.c)

LOBJ := $(ASRC:%.c=%.o)
TOBJ := $(TSRC:%.c=%.o)
OBJ := $(LOBJ) $(TOBJ)


#Exernal libraries
LIB := pthread


# Binary files
TEXEC := test_dlogger.out
LIB_NAME := libdlogger.a


# Compiler options
CC ?= gcc

C_STD := -std=gnu17
C_OPT := -O3
C_WARN :=
GGDB :=

ifeq ($(CC),clang)
	C_WARN += -Weverything -Wno-padded -Wno-vla
else ifneq (, $(filter $(CC), cc gcc))
	C_WARN += -Wall -Wextra -pedantic -Wcast-align \
			  -Winit-self -Wlogical-op -Wmissing-include-dirs \
			  -Wredundant-decls -Wshadow -Wstrict-overflow=5 -Wundef  \
			  -Wwrite-strings -Wpointer-arith -Wmissing-declarations \
			  -Wuninitialized -Wold-style-definition -Wstrict-prototypes \
			  -Wmissing-prototypes -Wswitch-default -Wbad-function-cast \
			  -Wnested-externs -Wconversion -Wunreachable-code
endif

# (Compilation for GDB) type make DEBUG=1 to enable gdb build mode
ifeq ("$(origin DEBUG)", "command line")
	GGDB := -ggdb3
else
	GGDB :=
endif

C_FLAGS = $(C_STD) $(C_OPT) $(C_WARN) $(GGDB)


# Path for installation script
INSTALL_PATH = 

ifeq ("$(origin P)", "command line")
	INSTALL_PATH = $(P)
endif 


# Headers and library for linker
H_INC := $(foreach d, $(IDIR), -I$d)
L_INC := $(foreach l, $(LIB), -l$l)


# Main dependency tree of Makefile
all: lib test

lib: $(LIB_NAME)

$(LIB_NAME): $(LOBJ)
	$(call print_ar,$@)
	$(Q)$(AR) $@ $^

test: $(TEXEC)

install:
	$(Q)$(SCRIPT_DIR)/install_dlogger.sh $(INSTALL_PATH)

$(TEXEC): $(TOBJ)
	$(call print_bin,$@)
	$(Q)$(CC) $(C_FLAGS) $(H_INC) $(TOBJ) -o $@ $(L_INC)

%.o:%.c
	$(call print_cc,$<)
	$(Q)$(CC) $(C_FLAGS) $(H_INC) -c $< -o $@

clean:
	$(call print_rm,EXEC)
	$(Q)$(RM) $(TEXEC)
	$(Q)$(RM) $(LIB_NAME)
	$(call print_rm,OBJ)
	$(Q)$(RM) $(OBJ)

help:
	@echo "***************************************************************"
	@echo "* DLogger Makefile options:                                   *"
	@echo "*                                                             *"
	@echo "*    all     - build dlogger with tests as examples           *"
	@echo "*    lib     - build only dlogger library                     *"
	@echo "*    test    - build only test as examples                    *"
	@echo "*    install - install DLogger on default or specified path   *"
	@echo "*    clean   - remove all necessary files                     *"
	@echo "*                                                             *"
	@echo "* Makefile supports Verbose mode when V=1                     *"
	@echo "* Makefile supports Debug mode when DEBUG=1                   *"
	@echo "* Makefile support two compilers: gcc and clang               *"
	@echo "* To change compiler, type CC variable (e.g. export CC=clang) *"
	@echo "***************************************************************"