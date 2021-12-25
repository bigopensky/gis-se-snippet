# GIS Stack Exchange Question

Question by [Joseph](https://gis.stackexchange.com/q/171279/26213).

I am working with GeoJSON files and need to store them in PostgreSQL using perl.  I can do this in 2 steps:

```perl
    open (JSON,">$path/tmp/current.json");
    print JSON $fullJSON;
    close JSON;
```
followed by:

```perl
    `/usr/bin/ogr2ogr -f "PostgreSQL" PG:"dbname=database user=datauser" "$path/tmp/current.json" -nln datatable -append`;
     unlink ("$path/tmp/current.json");
```

But I was hoping for a pure perl solution, without using the ogr2ogr executable.  I was reading about Geo::GDAL, but not sure if this is the right tool.

I was experimenting a bit with some code I found:

```perl
    my $sth = $dbh->prepare("INSERT INTO datatable (geom) VALUES (GeomFromEWKT('SRID=?;POINT(? ? ?)'));")
```

But couldn't quite get the syntax right.  The whole JSON string is:  `$fullJSON`

If anyone has any info or code to perform this task, I'd love to see it.  It seems pretty straight forward.


# Answer 

For me very interesting question, because I like Perl and gave the new
Swig based `Geo::GDAL` bindings to the `GDAL 2.0.2` library a try. The
library is maintained by [Ari Jolma][1].

In short: You should use the professional `ogr2ogr` tool and the
following script is more a how to use `GDAL` with Perl, how to read
the layer info's and how to iterate over the features and create some
SQL statements. I think Perl API is very similar to the python API.
To get an idea how to use `GDAL` in Perl you could use 80% of the
[python cookbook][2] (language bindings cross over) and the [Perl
class reference][3] for the rest.

I download an example [cities.geojson][4], thanks to [Michael
Mahemoff][5]. The quick and dirty coded script called `test-json.pl`
reads GeoJSON in a GDAL like way and print SQL statments.

## Tool and data

The underlaying data set is in the file <cities.geojson>

```json
> head -20 cities.geojson
{
	"type": "FeatureCollection",
	"features": [{
		"geometry": {
			"type": "Point",
			"coordinates": [-176.633, 51.883]
		},
		"type": "Feature",
		"properties": {
			"wikipedia": "Adak,_Alaska",
			"city": "Adak"
		},
		"id": "Adak"
	}, {
		"geometry": {
			"type": "Point",
			"coordinates": [-175.2, -21.133]
		},
		"type": "Feature",
		"properties": {
...
```

The tool [test-json.pl](./test-json.pl)> reads the data structure of
file [cities.geojson](cities.geojson]) and generates postgresql insert
statements as an app example.

```bash
./test-json.pl | head -20
layer: 1 name:OGRGeoJSON features: 661 type: Point
  spatial-reference: +proj=longlat +datum=WGS84 +no_defs 
  attribute: 1 name: wikipedia type: String
  attribute: 2 name: city type: String

INSERT INTO table (city, geom) VALUES
('Adak', ST_GeomFromEWKT('SRID=4326;POINT (-176.633 51.883)');

INSERT INTO table (city, geom) VALUES
('Nuku%CA%BBalofa', ST_GeomFromEWKT('SRID=4326;POINT (-175.2 -21.133)');

INSERT INTO table (city, geom) VALUES
('Apia', ST_GeomFromEWKT('SRID=4326;POINT (-171.833 -13.833)');

INSERT INTO table (city, geom) VALUES
('Pago Pago', ST_GeomFromEWKT('SRID=4326;POINT (-170.7 -14.267)');

INSERT INTO table (city, geom) VALUES
('Alofi', ST_GeomFromEWKT('SRID=4326;POINT (-169.917 -19.05)');
```

## License

COPYRIGHT AND LICENCES

Copyright (C) 2006 by Ari Jolma and GDAL Swig developers.
Copyright (C) 2016 by ALexander Weidauer <alex.weidauer@huckfinn.de>

This library is free software; you can redistribute it and/or modify
it under the same terms as Perl itself, either Perl version 5.8.5 or,
at your option, any later version of Perl 5 you may have available.

  [1]: https://github.com/ajolma
  [2]: https://pcjericks.github.io/py-gdalogr-cookbook/vector_layers.html
  [3]: https://arijolma.org/Geo-GDAL/2.4/
  [4]: https://raw.githubusercontent.com/mahemoff/geodata/master/cities.geojson
  [5]: https://github.com/mahemoff/geodata
