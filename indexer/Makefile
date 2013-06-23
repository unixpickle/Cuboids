SOURCES=indexer_arguments.c

indexer: resources
	gcc main.c $(wildcard ../*/build/*.o) -I../ -lpthread -o indexer

resources: build $(SOURCES)
	gcc -c $(SOURCES) -I../
	mv *.o build/

build:
	if [ ! -d build ]; then mkdir build; fi

clean:
	rm -rf build/ indexer
