SOURCE_DIRS=representation stickers algebra

all:
	for dir in $(SOURCE_DIRS); do \
		cd $$dir && $(MAKE); \
		cd ..; \
	done

test: all
	cd test && $(MAKE)

clean:
	for dir in $(SOURCE_DIRS); do \
		cd $$dir && $(MAKE) clean; \
		cd ..; \
	done
	cd test && $(MAKE) clean
