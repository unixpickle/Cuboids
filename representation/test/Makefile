all: cuboid_base_test cuboid_moves_test
	
cuboid_base_test:
	gcc cuboid_base_test.c $(wildcard ../build/*.o) -o cuboid_base_test -I../

cuboid_moves_test:
	gcc cuboid_moves_test.c $(wildcard ../build/*.o) -o cuboid_moves_test -I../

clean:
	rm -f cuboid_base_test cuboid_moves_test
