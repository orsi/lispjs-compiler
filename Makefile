CFLAGS += -Wall -Wextra -Wpedantic \
          -Wformat=2 -Wno-unused-parameter -Wshadow \
          -Wwrite-strings -Wstrict-prototypes -Wold-style-definition \
          -Wredundant-decls -Wnested-externs -Wmissing-include-dirs

.PHONEY: build bin clean debug test

all: build

build: bin
	$(CC) $(CFLAGS) ./src/main.c ./src/lex.c ./src/parse.c ./src/evaluate.c ./src/utils.c -o ./bin/roxanne

bin:
	mkdir bin

clean:
	@echo "Cleaning up..."
	rm -rf bin

test: bin
	@echo "\nTesting\n"
	$(CC) $(CFLAGS) -g3 ./src/test.c ./src/lex.c ./src/parse.c ./src/evaluate.c ./src/utils.c -o ./bin/test

debug: bin
	@echo "\nDebugging\n"
	$(CC) $(CFLAGS) -g3 ./src/debug.c -o ./bin/debug
	./bin/debug
