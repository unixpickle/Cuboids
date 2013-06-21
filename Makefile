SOURCE_DIRS=representation stickers algebra notation \
	 		search arguments saving input symmetry pieces

all: solver

test: libs
	cd test && $(MAKE)

solver: libs
	cd solver && $(MAKE)

libs:
	for dir in $(SOURCE_DIRS); do \
		cd $$dir && $(MAKE); \
		cd ..; \
	done

clean:
	for dir in $(SOURCE_DIRS); do \
		cd $$dir && $(MAKE) clean; \
		cd ..; \
	done
	cd test && $(MAKE) clean
	cd solver && $(MAKE) clean
