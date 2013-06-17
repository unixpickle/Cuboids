TESTS = cuboid_base_test cuboid_htmoves_test cuboid_qtmoves_test \
	mapconversion_test sticker_algebra_test inverse_test \
	notation_parse_test notation_cuboid_test search_boundary_test \
	search_base_test

all: test.o
	for test in $(TESTS); do \
		gcc $${test}.c $(wildcard ../*/build/*.o) test.o -o $$test -I../; \
	done

test.o:
	gcc -c test.c

clean:
	rm -f $(TESTS)
	rm -f test.o
