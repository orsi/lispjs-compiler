.PHONEY: build bin clean debug test

all: build

build: bin
	clang ./src/*.c -o ./bin/roxanne

bin:
	mkdir bin

clean:
	@echo "Cleaning up..."
	rm -rf bin

debug: bin
	@echo "\nDebugging...\n"
	clang -g3 /Users/orsi/Code/roxanne/src/test.c -o ./bin/test
