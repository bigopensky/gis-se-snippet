#!/usr/bin/perl -w
# -------------------------------------------------------------------------
# Gis Stack Exchange Test Case https://gis.stackexchange.com/q/171279/26213
# (C) 2016 Alexander Weidauer <alex.weidauer@huckfinn.de>
# -------------------------------------------------------------------------
use strict;
use warnings;

use Geo::GDAL;
use Data::Dumper;

# https://raw.githubusercontent.com/mahemoff/geodata/master/cities.geojson
# Set the driver name
my $drvName  = 'GeoJSON';

# Set the json file
my $file   = 'cities.geojson';

# Set the layer name
my $lyrName = 'OGRGeoJSON';

# Set the city column name
my $key_city = 'city';

# Set the geometry column name
my $key_geom = 'geom';

# Set the table name
my $table = 'table'; 

# Set EPSG
my $epsg = '4326';

# print join("\n",Geo::OGR::GetDriverNames),"\n";

# Get the OGR driver
my $driver   = Geo::OGR::Driver($drvName)
   or die "Unknown driver $drvName \n";

# Open the file 
my $dataSource   = $driver->Open($file)
   or die "Unknown file $file \n";

# List the layers to investigate the stuff
# to get all the settings above key_..., epsg
&listLayers($dataSource);

# Open the layer
my $layer = $dataSource->GetLayerByName($lyrName)
    or die "Unknown layer $lyrName \n";

# Iterate over the features
while (my $data = $layer->GetNextFeature) {
    my $geom = $data->Geometry->AsText;
    my $city = $data->{$key_city};
    $city    = &escapeSqlStr($city);
    print "INSERT INTO $table ($key_city, $key_geom) VALUES\n".
          "($city, ST_GeomFromEWKT('SRID=$epsg;$geom');\n\n";
}

# =========================================
# Service routines 
# =========================================

# -----------------------------------------
# A minimal NULL and escaping routine
# Q&D ..better done with DBI
sub escapeSqlStr() {
    my $str = shift;
    return 'NULL' if ! $str;
    $str =~ s/'/\'/s;
    return "'".$str."'";
}

# -----------------------------------------
# List features within the layers
sub listLayers() {
    my $dataSource = shift;
    # Iterate over the layers
    for my $lix (0..$dataSource->GetLayerCount-1) {
        my $layer = $dataSource->GetLayerByIndex($lix);
        my $srs   = $layer->GetSpatialRef;
        $srs = $srs ? $srs->Export('Proj4') : 'UNKNOWN';
        my $gtype = $layer->GeometryType;
        my $numFt = $layer->GetFeatureCount;

        
        # Layer statistics
        print "layer: ",$lix+1,
            " name:", $layer->GetName,
            " features: ", $numFt,
            " type: ", $gtype,
            "\n  spatial-reference: $srs\n";

        # List attributes
        my $layerDefn = $layer->GetLayerDefn();
        for my $fix (0..$layerDefn->GetFieldCount-1) {
            my $fieldDefn = $layerDefn->GetFieldDefn($fix);
            print "  attribute: ", $fix+1,
                " name: ", $fieldDefn->Name,
                " type: ",$fieldDefn->Type,"\n";
        }
        print "\n";
    }
}

# EOF




