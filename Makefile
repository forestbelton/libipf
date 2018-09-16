ifeq ($(OS),Windows_NT)
	LIBSUFFIX = .dll
	EXESUFFIX = .exe
else
	LIBSUFFIX = .so
	EXESUFFIX
endif

OBJECTS = libipf$(LIBSUFFIX) unipf$(EXESUFFIX) ipf$(EXESUFFIX)

all: libs sample

clean: 
	rm -f $(OBJECTS) *~ *.scc *.vcproj *.vspscc
	-@ cd src ; make clean
	-@ cd samples ; make clean

libs:
	- cd src ; make
	-@ cp ./src/libipf$(LIBSUFFIX) ./
	-@ cp ./src/libipf.a ./

sample:
	- cd samples ; make
	-@ cp ./samples/unipf$(EXESUFFIX) ./
	-@ cp ./samples/ipf$(EXESUFFIX) ./

