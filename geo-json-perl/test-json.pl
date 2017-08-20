#!/usr/bin/perl -w
# -----------------------------------------------
# Gis stack exchange test case
# -----------------------------------------------
use strict;
use Geo::OGR;
# https://raw.githubusercontent.com/mahemoff/geodata/master/cities.geojson
# Set the driver name
my $dname  = 'GeoJSON';

# Set the json file
my $file   = 'cities.geojson';

# Set the layer name
my $lname = 'OGRGeoJSON';

# Set the city column name
my $key_city = 'city';

# Set the geometry column name
my $key_geom = 'geom';

# Set the table name
my $table = 'table'; 

# Set EPSG
my $epsg = '4326';

# Get the driver
my $driver    = Geo::OGR::GetDriverByName($dname)
   or die "Unkown driver $dname \n";

# Open the file 
my $dsource   = $driver->Open($file)
   or die "Unkown file $file \n";

# List the layers to investigate the stuff
# to get all the settings above key_..., epsg
&listLayers($dsource);

# Open the layer
my $layer = $dsource->GetLayerByName($lname)
    or die "Unkown layer $lname \n";

# Iterate over the features
while (my $data = $layer->GetNextFeature) {
    my $geom = $data->Geometry->AsText;
    my $city = $data->{$key_city};
    $city = &escapeSqlStr($city);
    print "INSERT INTO $table ($key_city, $key_geom) VALUES\n".
        "($city, ST_GeomFromEWKT('SRID=$epsg;$geom');\n\n";
}

# -----------------------------------------
# a minimal NULL and escaping routine
# better done with DBI
sub escapeSqlStr() {
    my $str = shift;
    return 'NULL' if ! $str;
    $str =~ s/'/\'/s;
    return "'".$str."'";
}

# -----------------------------------------
# GetNextFeature the layers and attributes
# @param is a dataSource
sub listLayers() {
    my $dataSource = shift;
    # Iterate over the layers
    for my $lix (0..$dataSource->GetLayerCount-1) {
        my $layer = $dataSource->GetLayerByIndex($lix);
        my $srs   = $layer->GetSpatialRef;
        my $gtype = $layer->GeometryType;
        my $numFt = $layer->GetFeatureCount;
        # Layer statistics
        print "layer: ",$lix+1,
            " name:", $layer->GetName,
            " features: ", $numFt,
            " type: ", $gtype,
            "\n  spatial-reference: ", $srs->Export('Proj4'),"\n";

        # List attrbutes
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





