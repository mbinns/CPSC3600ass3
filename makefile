CC:=clang
CFLAGS:=-Wall -g -Wpedantic -Werror -lcurl

all: starLord chopper ahsoka

clean:
	$(RM) starLord chopper ahsoka *.o

starLord: 
	$(CC) $(CFLAGS) -o starLord starLord.c set.c common.c

chopper: 
	$(CC) $(CFLAGS) -o chopper chopper.c common.c

ahsoka:
	$(CC) $(CFLAGS) -o ahsoka ahsoka.c common.c

.PHONY: all clean
