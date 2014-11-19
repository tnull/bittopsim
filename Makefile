SRCS=src/*.cpp
SRCS+=src/*.h

all: src doc

src:
	cd src; make

doc: $(SRCS) 
	doxygen doxygen.config
clean:
	cd src; make clean
	rm -r doc/*

.PHONY: all src clean doxy
