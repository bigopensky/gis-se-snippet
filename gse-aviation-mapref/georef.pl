#!/usr/bin/env perl
# -------------------------------------------------------------------------
# GIS Stack Exchange Question https://gis.stackexchange.com/q/419683/26213
# (C) 2016 Alexander Weidauer <alex.weidauer@huckfinn.de>
# -------------------------------------------------------------------------
use strict;
use warnings;
use Geo::Proj4;
use Data::Dumper;

# Find the tools in the operation system
my $toolTrans = `which gdal_translate`;
my $toolWarp  = `which gdalwarp`;
chomp($toolTrans);
chomp($toolWarp);

# Incoming file
my $inFile  = 'data/Latest-gfacn31_cldwx_000.png';

# Temporary file
my $outTemp = $inFile;

# Strip extension
$outTemp =~ s/\.png$//;

# Tanslate step to set the srs
my $outTrans = "${outTemp}_trans.tif";

# Warp step to oversample the stuff
my $outWarp = "${outTemp}_warp.tif";

# Source coordinate system
my $srcCrs='+proj=stere +lat_0=90 +lat_ts=60';

# Final coordinate system
my $dstCrs='+init=epsg:5937';


# Destination resolution
my $gsd =2000; # 2 km

# Set the psspoint input as text
my $GCP_LL='
# ID  PY    WLON  WLAT
GPC1 099 090 -139.00 60.00
GPC2 368 137 -120.05 60.00
GPC3 485 453 -114.06 49.00
GPC4 312 459 -122.75 49.00';

# Create the projection
my $proj = Geo::Proj4->new($srcCrs)
    or die "parameter error: ".Geo::Proj4->error. "\n";

# Read the coordinate system lines
my @coords = split(/\n/, $GCP_LL);
my @gcps;

for my $coord (@coords) {

    # Trim data string
    $coord =~ s/^\s+//;
    $coord =~ s/\s+$//;

    # Next line if empty
    next if $coord eq '';

    # Next line if comment
    next if $coord =~/^#/;

    # Read coordinates
    my ($id, $px, $py, $wlon, $wlat) = split(/\s+/, $coord);

    # Transform lon lat to sphere x and y
    my ($wx, $wy) = $proj->forward($wlat, $wlon);
    printf "PIX: %3d %3d LON.LAT: %7.2f %6.2f SHP: %2.2f %2.2f\n",
       $px, $py, $wlon, $wlat, $wx, $wy;

    push (@gcps,"-gcp $px $py $wx $wy");
}

# Translate GCP int parameter strings
my $gcpstr = join(" ", @gcps);
my ($cmd, $res);

# Set the reference system and ground control points
$cmd ="$toolTrans $gcpstr -a_srs '$srcCrs' -expand rgb -of GTiff $inFile $outTrans";
print "SET SRS COMMAND: $cmd\n";
$res = `$cmd`;

# Resample the image
$cmd = "$toolWarp -overwrite -t_srs '$dstCrs' -tr $gsd $gsd -r lanczos $outTrans $outWarp"  ;
print "RESAMPLE COMMAND: $cmd\n";
$res = `$cmd`;

# EOF
