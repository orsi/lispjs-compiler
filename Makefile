.PHONEY: build bin clean debug test

all: build

build: bin
	clang ./src/main.c ./src/lex.c ./src/parse.c ./src/evaluate.c ./src/utils.c -o ./bin/roxanne

bin:
	mkdir bin

clean:
	@echo "Cleaning up..."
	rm -rf bin

debug: bin
	@echo "\nDebugging...\n"
	clang -g3 ./src/test.c ./src/lex.c ./src/parse.c ./src/evaluate.c ./src/utils.c -o ./bin/test
