STATIC_LIBS = build/static/libbmp.a
WARNING_FLAGS = -Wall -Wno-pragma-pack
STATIC_LIB_FLAGS = -L./build/static/ -lbmp

.PHONY: clean writer reader
all: bin/writer bin/reader bin/process

process bin/process: build/process.o $(STATIC_LIBS)
	mkdir -p bin
	gcc -O0 $(WARNING_FLAGS) \
		-o ./bin/process $(STATIC_LIB_FLAGS) ./build/process.o

writer bin/writer: build/writer.o $(STATIC_LIBS)
	mkdir -p bin
	gcc -O0 $(WARNING_FLAGS) \
		-o ./bin/writer $(STATIC_LIB_FLAGS) ./build/writer.o

reader bin/reader: build/reader.o $(STATIC_LIBS)
	mkdir -p bin
	gcc -O0 $(WARNING_FLAGS) \
		-o ./bin/reader $(STATIC_LIB_FLAGS) ./build/reader.o

build/process.o: ./src/03_bitmap_processor/main.c
	mkdir -p build
	gcc -O0 $(WARNING_FLAGS) -I./src -c \
		-o ./build/process.o ./src/03_bitmap_processor/main.c

build/writer.o: ./src/02_bitmap_writer/main.c
	mkdir -p build
	gcc -O0 $(WARNING_FLAGS) -I./src -c \
		-o ./build/writer.o ./src/02_bitmap_writer/main.c

build/reader.o: ./src/01_bitmap_reader/main.c
	mkdir -p build
	gcc -O0 $(WARNING_FLAGS) -I./src -c \
		-o ./build/reader.o ./src/01_bitmap_reader/main.c

# static library "bmp.h"
build/static/libbmp.a: $(wildcard src/bmp/*.c)
	mkdir -p build/static
	mkdir -p build/chunks/bmp

	for i in $(wildcard src/bmp/*.c); do \
		gcc -O0 $(WARNING_FLAGS) -I./src/bmp -c \
			-o ./build/chunks/bmp/$$(basename $$i.o) $$i; \
	done

	ar rcs ./build/static/libbmp.a build/chunks/bmp/*.o

clean:
	rm -rf $(wildcard bin/*)
	rm -rf $(wildcard build/*)
	rm -rf $(wildcard build/**/*)
