### Makefile --- 
## Author:  alex.weidauer@huckfinn.de
## Version: $Id: Makefile,v 0.1 2013/10/02 14:41:26 huckfinn Exp $

LPATH   = -L/usr/lib -L/usr/local/lib -L.
IPATH   = -I/usr/include/ -I/usr/local/include
CFLAGS  = -std=c99 -pedantic
LLINALG = -lgsl -lblas -lm
LGDAL   = -lgdal

all:	build/tif-cut 

clean:
	rm build/*

build/tif-cut: tif-cut.c alg.c
	gcc  $(IPATH) $(LPATH) $(LGDAL) $(LLINALG) $(CFLAGS) -o build/tif-cut tif-cut.c alg.c

alg.o: alg.c
	gcc $(IPATH) $(LPATH) $(LLINALG) $(CFLAGS) -c alg.c 



