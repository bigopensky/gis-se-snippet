### Makefile --- 
## Author:  alex.weidauer@huckfinn.de
## Version: $Id: Makefile,v 0.1 2013/10/02 14:41:26 huckfinn Exp $

LPATH   = -L/usr/lib -L/usr/local/lib -L.
IPATH   = -I/usr/include -I/usr/local/include
CFLAGS  = -std=c99 -pedantic
LLINALG = -lgsl -lblas -lm
LGDAL   = -lgdal

all:	build/tif-cut

clean:
	rm -f build/*

build/tif-cut: tif-cut.c build/alg.o
	gcc tif-cut.c alg.c -o build/tif-cut $(IPATH) $(LPATH) $(LGDAL) $(LLINALG) $(CFLAGS)

build/alg.o: alg.c
	gcc -c alg.c -o build/alg.o $(IPATH) $(LPATH) $(LLINALG) $(CFLAGS)



