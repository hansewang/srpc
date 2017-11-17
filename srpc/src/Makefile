
CC  = gcc
CXX = g++

INC     = -I.
OPT     = -pipe -fno-ident  
LINK    = -lprotobuf -lpthread


CFLAGS += $(OPT) -g
GCC_GTEQ_480 := $(shell expr `gcc -dumpversion | sed -e 's/\.\([0-9][0-9]\)/\1/g' -e 's/\.\([0-9]\)/0\1/g' -e 's/^[0-9]\{3,4\}$$/&00/'` \>= 40800)

ifeq "$(GCC_GTEQ_480)" "1"
CFLAGS += -Wno-unused-but-set-variable
endif

CXXFLAGS := $(CFLAGS)

SRCS += $(wildcard *.cpp *.c *.cc)
OBJS = $(patsubst %.cpp,%.o,$(patsubst %.c ,%.o,$(patsubst %.cc,%.o,$(SRCS) ) ) )

OBJS_DEL = $(patsubst %.cpp,%.o,$(patsubst %.c,%.o,$(patsubst %.cc,%.o,$(SRCS) ) ) )


BIN_NAME = srpc

all: $(BIN_NAME)

$(BIN_NAME): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LINK)

%.o: %.c Makefile
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<
%.o: %.cc Makefile
	$(CXX) $(CXXFLAGS) $(INC) -c -o $@ $<
%.o: %.cpp Makefile
	$(CXX) $(CXXFLAGS) $(INC) -c -o $@ $<
clean:
	rm -rf $(BIN_NAME) $(OBJS_DEL)
