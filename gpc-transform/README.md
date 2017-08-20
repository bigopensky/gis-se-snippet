% GPC-TRFM(1)
% Alexander Weidauer
% Mai 2012

# gpc-trfm Calculate affine transformations

## NAME

gpc-trfm – Build an affine transform and calculate coordinates 

## INSTALLATION

Call make, the resulting binary will be in the directory build.
You will need [BLAS](http://www.netlib.org/blas/) and [GSL](https://www.gnu.org/software/gsl/).

## SYNOPSIS

gpc_trfm [-h -d -i -s CHAR] -p sx sy dx dy -p sx sy dx dy -p sx sy dx dy [-p ...] -c x y [-c x y ...]

## DESCRIPTION

**gpc-trfm** is a simple-minded tool which creates an affine transform
from a set of at least 3 pass points (-p) an calculates the coordinates
given with tag -c forward or innverse (-i).

## GENERAL OPTIONS

* **-h**, **--help** Show elp message
* **-i** calculate the inverse coordinates given with option -c
* **-d** show debug messages debug 
* **-s** CHAR output column separator default [space]
* **-t** list transformations parser friendly
* **-p** sx sy dx dy **-p** sx sy dx dy ... pass points at least 3 with
    sx sy  source point
    dx dy  destination point
* **-c** x y -c x y ...coordinates to transform

## EXAMPLES

>  gpc_trfm -s "|" -p 1 0 10 0 -p 0 1 0 10 -p 1 1 10 10 -c 1 1 -c 1 0

>  gpc_trfm -i -p 1 0 10 0 -p 0 1 0 10 -p 1 1 10 10 -c 1 1 -c 1 0

## OUTPUT

Parser friendly form

> gpc-trfm -t -s "|" -p 1 0 10 0 -p 0 1 0 10 -p 1 1 10 10 -c 1 1 -c 1 0

```
 # TOOL: GPC TRANSFORM
 TRFMX:|10.000000|0.000000|0.000000
 TRFMY:|0.000000|10.000000|0.000000
 ITRFMX:|0.100000|0.000000|0.000000
 ITRFMY:|0.000000|0.100000|0.000000
 # HEAD: SRC.X SRC.Y TRFM.X TRFM.Y ERROR.X ERROR.Y
 COORD:|1.000000|1.000000|10.000000|10.000000|0.000000|0.000000
 COORD:|1.000000|0.000000|10.000000|0.000000|0.000000|0.000000
 # EOF
```

Human friendly form

 > gpc-trfm -p 1 0 10 0 -p 0 1 0 10 -p 1 1 10 10 -c 1 1 -c 1 0

```
 # TOOL: GPC TRANSFORM
 # TRFM:  X = 10.000000 * x + 0.000000 * y + 0.000000
 # TRFM:  Y = 0.000000 * x + 10.000000 * y + 0.000000
 # ITRFM: X = 0.100000 * x + 0.000000 * y + 0.000000
 # ITRFM: Y = 0.000000 * x + 0.100000 * y + 0.000000
 # HEAD: SRC.X SRC.Y TRFM.X TRFM.Y ERROR.X ERROR.Y
 COORD: 1.000000 1.000000 10.000000 10.000000 0.000000 0.000000
 COORD: 1.000000 0.000000 10.000000 0.000000 0.000000 0.000000
 # EOF
```

## License

 gpc-trfm.c is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.

 gpc-trfm.c is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with gpc-trfm.c.  If not, see <http://www.gnu.org/licenses/>.


