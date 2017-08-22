# Query GeoJSON with GDAL and Perl

Small howto use the GDAL to read GeoJSON via
(Geo::GDAL)[https://github.com/ajolma/Geo-GDAL-2.01] provided by Ari
Jolma.

## Tool and data

The underlaying data set is in the file <cities.geojson>

```
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

The tool <test-json.pl> reads the data structure of <cities.geojson>
and generates postgresql insert statements as an app example.

```
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

COPYRIGHT AND LICENCE

Copyright (C) 2006 by Ari Jolma and GDAL Swig developers.

This library is free software; you can redistribute it and/or modify
it under the same terms as Perl itself, either Perl version 5.8.5 or,
at your option, any later version of Perl 5 you may have available.