main: cuboid_base.o

test: main
	$(MAKE) -C test

cuboid_base.o: symmetry3.o build
	gcc -c cuboid_base.c -o build/cuboid_base.o

symmetry3.o: build
	gcc -c symmetry3.c -o build/symmetry3.o

build:
	if [ ! -d build ]; then mkdir build; fi

clean:
	cd test && $(MAKE) clean
	if [ -d build ]; then rm -r build; fi
