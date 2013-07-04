SOURCES=indexer_arguments.c heuristic_index.c

indexer: resources
	gcc main.c -O2 $(wildcard ../*/build/*.o) -I../ -lpthread -o indexer

resources: build $(SOURCES)
	gcc -c -O2 $(SOURCES) -I../
	mv *.o build/

build:
	if [ ! -d build ]; then mkdir build; fi

clean:
	rm -rf build/ indexer
