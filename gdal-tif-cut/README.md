# Example to cut georeferenced snipptes from a geotiff using plain C

## Installation

The tool is addressed for a *UX/ *IX environment.
Call the tool <make>, the resulting binary will be in the directory <build>.
You will need [BLAS](http://www.netlib.org/blas/), [GSL](https://www.gnu.org/software/gsl/)
and [GDAL](www.gdal.org).

## Purpose

The tool was created to cut more than on image subset given by a set if XY coordinates
and an ID to strip samples with seabirds form survey images in one step.

The tool gdal_translate is a more versatile tool shipped with the GDAL software,
if you don't need the multicut part.


## Usage

```
 tif-cut IN OUT EXT SZ ID X Y ID X Y ID X Y
```

* IN Infile geotiff without extention assuming .tif
* OUT Outfile geotiff without extention assuming .tif
* EXT geotiff  extention assuming .tif, the filenames for in and out will be pasted
* SZ Integer size of the squared window
* ID an Integer ID like an database key pasted into the outfile name
* X World coordinate of the center point
* Y World coordinate of the center point

## License

 tif-cut.c is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.

 tif-cut.c is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with tif-cut.c.  If not, see <http://www.gnu.org/licenses/>.
