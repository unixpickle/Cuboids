SRC=$(wildcard *.c)

main: $(SRC) build
	gcc -O2 -c $(SRC) -Wall
	mv *.o build/

build:
	if [ ! -d build ]; then mkdir build; fi

clean:
	if [ -d build ]; then rm -r build; fi
