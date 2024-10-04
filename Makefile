OBJECT_FILES = build/main.o build/bmp.o
WARNING_FLAGS = -Wall -Wno-pragma-pack

bin/main: $(OBJECT_FILES)
	mkdir -p bin
	gcc -O0 $(WARNING_FLAGS) -g -o ./bin/main $(OBJECT_FILES) 

build/main.o: $(wildcard src/*.c)
	mkdir -p build
	gcc -O0 $(WARNING_FLAGS) -I ./src -c -o ./build/main.o $(wildcard src/*.c)

build/bmp.o: $(wildcard src/bmp/*.c)
	mkdir -p build
	gcc -O0 $(WARNING_FLAGS) -I ./src/bmp -c -o ./build/bmp.o $(wildcard src/bmp/*.c)

.PHONY: run
run:
	./bin/main

.PHONY: clean
clean:
	rm -rf $(wildcard bin/*)
	rm -rf $(wildcard build/*)
