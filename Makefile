all: representation/build stickers/build

test: all
	cd test && $(MAKE)

representation/build:
	cd representation && $(MAKE)

stickers/build:
	cd stickers && $(MAKE)

clean:
	cd representation && $(MAKE) clean
	cd stickers && $(MAKE) clean
	cd test && $(MAKE) clean
