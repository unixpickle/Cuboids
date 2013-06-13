SRC=$(wildcard *.c)

main: $(SRC) build
	gcc -c $(SRC) -Wall
	mv *.o build/

test: main
	$(MAKE) -C test

build:
	if [ ! -d build ]; then mkdir build; fi

clean:
	cd test && $(MAKE) clean
	if [ -d build ]; then rm -r build; fi
