CC := gcc
CFLAGS := -std=c89 -pedantic -Wall -Wextra -g3 -O0
LDFLAGS :=

all: unpack_packdat3 repack_packdat3

unpack_packdat3: unpack_packdat3.c
	$(CC) $(CFLAGS) $? -o $@ $(LDFLAGS)

repack_packdat3: repack_packdat3.c
	$(CC) $(CFLAGS) $? -o $@ $(LDFLAGS)

clean:
	rm -f unpack_packdat3 unpack_packdat3.exe repack_packdat3 repack_packdat3.exe *.o


.PHONY: clean
