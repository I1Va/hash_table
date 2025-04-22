
# make MODE=DEBUG
# make
# make MODE=DEBUG clean
# make clean

MODE ?= RELEAZE
ROOT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

ifeq ($(MODE),DEBUG)
	CFLAGS = $(CDEBFLAGS)
	OUT_O_DIR = debug
	EXTRA_FLAGS = $(SANITIZER_FLAGS)
endif

ifeq ($(origin CC),default)
	CC = g++
endif


CFLAGS ?= -O2 -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal    \
	-Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch     \
	-Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions                   \
	-Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 \
	-Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor   \
	-Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing \
	-Wstrict-null-sentinel -Wtype-limits -Wwrite-strings

CDEBFLAGS = -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations \
-Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported \
-Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security \
-Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual\
-Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel \
-Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override\
-Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros\
-Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs \
-Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging\
-fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-usage=8192 -pie -fPIE -Werror=vla\

SANITIZER_FLAGS = -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,$\
integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,$\
shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

EXTRA_FLAGS =

OUTFILE_NAME = hash_table.out
OUT_O_DIR = build
COMMONINC = -I./inc
SRC = ./src

ifeq ($(MODE),DEBUG)
	CFLAGS = $(CDEBFLAGS)
	OUT_O_DIR = debug
	EXTRA_FLAGS = $(SANITIZER_FLAGS)
endif



override CFLAGS += $(COMMONINC)

CSRC = src/data_functions.cpp src/hash_functions.cpp src/hash_table.cpp src/measure_funcs.cpp main.cpp

#/---------------------------SUBMODULES--------------------\#
SUBMODULES =
COMMONINC += $(addsuffix /inc,-I./$(SUBMODULES))
CSRC += $(wildcard $(addsuffix /src,$(SUBMODULES))/*.cpp)
#/---------------------------SUBMODULES--------------------\#



COBJ := $(addprefix $(OUT_O_DIR)/,$(CSRC:.cpp=.o))

DEPS = $(COBJ:.o=.d)


.PHONY: all

all: $(OUT_O_DIR)/$(OUTFILE_NAME)


$(OUT_O_DIR)/$(OUTFILE_NAME): $(COBJ)
	@$(CC) $^ -o $@ $(LDFLAGS) $(EXTRA_FLAGS)

$(COBJ) : $(OUT_O_DIR)/%.o : %.cpp
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c $< -o $@

$(DEPS) : $(OUT_O_DIR)/%.d : %.cpp
	@mkdir -p $(@D)
	@$(CC) -E $(CFLAGS) $< -MM -MT $(@:.d=.o) > $@

.PHONY: clean

clean:
	@rm -rf $(COBJ) $(DEPS) $(OUT_O_DIR)/*.out $(OUT_O_DIR)/*.log

NODEPS = clean

ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
include $(DEPS)
endif