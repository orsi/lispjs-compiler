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
	@echo "\Debugging...\n"
	clang ./src/debug.c -o ./bin/debug
	@./bin/debug

test: bin
	@echo "\nTesting...\n"
	clang ./src/test.c -o ./bin/test
	@./bin/test
