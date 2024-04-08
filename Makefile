.PHONEY: build bin clean test

all: build

test:
	@echo "\nTesting...\n"
	clang ./src/test.c -o ./bin/test
	@./bin/test

build: bin
	clang ./src/*.c -o ./bin/roxanne

bin:
	mkdir bin

clean:
	@echo "Cleaning up..."
	rm -rf bin
