main: cuboid_qtmoves.o

test: main
	$(MAKE) -C test

cuboid_qtmoves.o: cuboid_base.o cuboid_center_turns.o build
	gcc -c cuboid_qtmoves.c -o build/cuboid_qtmoves.o

cuboid_base.o: symmetry3.o build
	gcc -c cuboid_base.c -o build/cuboid_base.o

cuboid_center_turns.o: build
	gcc -c cuboid_center_turns.c -o build/cuboid_center_turns.o

symmetry3.o: build
	gcc -c symmetry3.c -o build/symmetry3.o

build:
	if [ ! -d build ]; then mkdir build; fi

clean:
	cd test && $(MAKE) clean
	if [ -d build ]; then rm -r build; fi
