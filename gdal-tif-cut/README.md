# Example to cut georeferenced snipptes from a geotiff using plain C

## Installation

The tool is addressed for a *UX/ *IX environment.  Call the tool <make>,
the resulting binary will be in the directory <build>.  You will need
[BLAS](http://www.netlib.org/blas/),
[GSL](https://www.gnu.org/software/gsl/) and [GDAL](www.gdal.org).

## Purpose

The tool was created to cut more than on image subset given by a set if
XY coordinates and an ID to strip samples with seabirds form survey
images in one step.

The tool `gdal_translate` is a more versatile tool shipped with the GDAL
software, if you don't need the multicut part.


## Usage

```
 tif-cut IN OUT EXT SZ ID X Y ID X Y ID X Y
```

* IN Infile GeoTiff

* OUT Outfile GeoTiff without extention assuming '.tif'

* EXT geotiff extention assuming .tif, the filenames for the output file
  will be pasted together with the ID and the extension

* SZ Integer size of the squared window placed around the center point

* ID an Integer ID like an database key pasted into the output file name

* X World coordinate of the center point

* Y World coordinate of the center point

## Example

```bash
DPATH='../data'
APATH='./build'
$APATH/tif-cut $DPATH/dem.v2.3d.tif $DPATH/test .tif 200 1 399261.9 6039631.7
display $PATH/test.1.tif
```

## License

 `tif-cut.c` is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.

 `tif-cut.c` is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with `tif-cut.c`.  If not, see <http://www.gnu.org/licenses/>.
