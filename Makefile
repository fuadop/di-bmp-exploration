OBJECT_FILES = build/bmp.o
WARNING_FLAGS = -Wall -Wno-pragma-pack

.PHONY: clean writer reader
all: bin/writer bin/reader

writer bin/writer: build/writer.o $(OBJECT_FILES)
	mkdir -p bin
	gcc -O0 $(WARNING_FLAGS) -o ./bin/writer build/writer.o $(OBJECT_FILES)

reader bin/reader: build/reader.o $(OBJECT_FILES)
	mkdir -p bin
	gcc -O0 $(WARNING_FLAGS) -o ./bin/reader build/reader.o $(OBJECT_FILES)

build/writer.o: $(wildcard src/01_bitmap_writer/*.c)
	mkdir -p build
	gcc -O0 $(WARNING_FLAGS) -I./src -c -o ./build/writer.o $(wildcard src/02_bitmap_writer/*.c)

build/reader.o: $(wildcard src/01_bitmap_reader/*.c)
	mkdir -p build
	gcc -O0 $(WARNING_FLAGS) -I./src -c -o ./build/reader.o $(wildcard src/01_bitmap_reader/*.c)

build/bmp.o: $(wildcard src/bmp/*.c)
	mkdir -p build
	gcc -O0 $(WARNING_FLAGS) -I./src/bmp -c -o ./build/bmp.o $(wildcard src/bmp/*.c)

clean:
	rm -rf $(wildcard bin/*)
	rm -rf $(wildcard build/*)
