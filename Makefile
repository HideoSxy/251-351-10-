all: build/.exe

create_build:
	mkdir -p build

build/.exe: create_build
	g++ main.cpp -o build/exe