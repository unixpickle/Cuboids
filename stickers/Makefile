SRC=$(wildcard *.c)

main: $(SRC) build
	gcc -c $(SRC) -Wall -I../
	mv *.o build/

build:
	if [ ! -d build ]; then mkdir build; fi

clean:
	if [ -d build ]; then rm -r build; fi
