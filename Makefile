CFLAGS += -Wall -Wextra -Wpedantic \
          -Wformat=2 -Wno-unused-parameter -Wshadow \
          -Wwrite-strings -Wstrict-prototypes -Wold-style-definition \
          -Wredundant-decls -Wnested-externs -Wmissing-include-dirs

.PHONEY: all

all: build

build: bin
	$(CC) $(CFLAGS) ./src/main.c ./src/lex.c ./src/parse.c ./src/evaluate.c ./src/utils.c -o ./bin/roxanne

bin:
	mkdir bin

clean:
	rm -rf bin

build-test: bin
	$(CC) $(CFLAGS) -g3 ./src/test.c ./src/lex.c ./src/parse.c ./src/evaluate.c ./src/utils.c -o ./bin/test
	
test: build-test
	./bin/test

build-debug: bin
	$(CC) $(CFLAGS) -g3 ./src/debug.c -o ./bin/debug

debug: build-debug
	./bin/debug
