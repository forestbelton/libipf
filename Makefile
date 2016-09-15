
OBJECTS = libipf.so unipf ipf

all: libs sample

clean: 
	rm -f $(OBJECTS) *~ *.scc *.vcproj *.vspscc
	-@ cd src ; make clean
	-@ cd samples ; make clean

libs:
	- cd src ; make
	-@ cp ./src/libipf.so ./

sample:
	- cd samples ; make
	-@ cp ./samples/ipf ./
	-@ cp ./samples/unipf ./

