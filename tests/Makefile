CC = gcc
CFLAGS = -Wall -Wextra -I../include/
LDFLAGS = -lcunit -lapr-1 -ljansson

all: test_reader

test_reader: test_reader.c ../src/reader.c ../src/hash.c ../src/tlv.c
	$(CC) -DTEST $(CFLAGS) -o test_reader test_reader.c ../src/reader.c ../src/hash.c ../src/tlv.c $(LDFLAGS)

clean:
	rm -f test_reader
