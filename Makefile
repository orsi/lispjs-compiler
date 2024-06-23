CFLAGS += -Wall -Wextra -Wpedantic \
          -Wformat=2 -Wno-unused-parameter -Wshadow \
          -Wwrite-strings -Wstrict-prototypes -Wold-style-definition \
          -Wredundant-decls -Wnested-externs -Wmissing-include-dirs

.PHONEY: default

default: clean bin build

bin:
	mkdir bin

build: bin
	$(CC) $(CFLAGS) ./src/main.c ./src/lex.c ./src/parse.c ./src/evaluate.c ./src/utils.c -o ./bin/rox

build-debug: bin
	$(CC) $(CFLAGS) -g3 ./src/main.c ./src/lex.c ./src/parse.c ./src/evaluate.c ./src/utils.c -o ./bin/rox

clean:
	rm -rf bin
