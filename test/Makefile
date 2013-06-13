all: cuboid_base_test cuboid_qtmoves_test cuboid_htmoves_test mapconversion_test

cuboid_base_test:
	gcc cuboid_base_test.c $(wildcard ../representation/build/*.o) -o cuboid_base_test -I../

cuboid_qtmoves_test:
	gcc cuboid_qtmoves_test.c $(wildcard ../representation/build/*.o) -o cuboid_qtmoves_test -I../

cuboid_htmoves_test:
	gcc cuboid_htmoves_test.c $(wildcard ../representation/build/*.o) -o cuboid_htmoves_test -I../

mapconversion_test:
	gcc mapconversion_test.c $(wildcard ../*/build/*.o) -o mapconversion_test -I../

clean:
	rm -f cuboid_base_test cuboid_qtmoves_test cuboid_htmoves_test mapconversion_test
