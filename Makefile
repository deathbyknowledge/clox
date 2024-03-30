CC = clang
CFLAGS = -Wall -g

BIN = clox

OBJS = $(patsubst src/%.c,src/%.o,$(wildcard src/*.c))

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) src/*.o -o $(BIN)

src/%.o: src/%.c src/%.h
	$(CC) $(CFLAGS) -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(BIN)
	rm -f src/*.o
	rm -f $(PY)
