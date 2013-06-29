TESTS = cuboid_base_test cuboid_htmoves_test cuboid_qtmoves_test \
	mapconversion_test sticker_algebra_test inverse_test \
	notation_parse_test notation_cuboid_test search_boundary_test \
	search_base_test search_cuboid_test arguments_parse_test \
	saving_test symmetry_test edge_orientation_test \
	heuristic_data_list_test index_profile corner_orientation_test

all: test.o
	for test in $(TESTS); do \
		gcc $${test}.c $(wildcard ../*/build/*.o) test.o -o $$test -I../ -lpthread; \
	done

test.o:
	gcc -c test.c

clean:
	rm -f $(TESTS)
	rm -f test.o
