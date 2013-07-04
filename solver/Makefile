SOLVERS=standardpl eopl pairpl
SOLVERS_BUILD=standardpl/build eopl/build

solver: solvers
	gcc -O2 $(wildcard *.c) $(wildcard ../*/build/*.o) -I../ -lpthread -o solver

solvers: build
	for dir in $(SOLVERS); do \
		cd $$dir && gcc -c *.c -I../../; \
		mv *.o "$(CURDIR)"/build; cd ..; \
	done

build:
	if [ ! -d build ]; then mkdir build; fi

clean:
	rm -rf build/ solver
