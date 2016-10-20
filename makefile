CC:=clang
CFLAGS:= -Wall -g -Wpedantic -Werror

all: starLord chopper ahsoka

clean:
	$(RM) starLord chopper ahsoka *.o

starLord: common.o set.o

chopper: common.o

ahsoka: common.o

.PHONY: all clean
