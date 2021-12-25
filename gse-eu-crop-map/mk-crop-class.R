# DESCRIPTION --------------------------------------------------------------
#
# GSE RIVER BED CALCULATION
#
# The sketch is related to the gis.stackexchange.com question:
# "Comparing two raster, based on a complex set of rules"
# URL: https://gis.stackexchange.com/q/419445/26213
#
# The sketch demonstrates how to use the rgdal driver and the raster
# capabilities to:
# 1. calculate the difference between two raster sets,
# 2. create a resulting data set by
#    classify the the incoming data (preconditions)
#    by a set of rules
# 3. Display and store the results

# COPYRIGHT -----------------------------------------------------------------
# (C) 2021 A. Weidauer <alex.weidauer@huckfinn.de> 2021
#     and is licensed by the 3-clause BSD license

# CLEANUP --------------------------------------------------------------------
# Clean up a previous session
rm(list=ls())

# Load the raster and vector driver kit
require(rgdal)
require(raster)
require(sf)
require(tictoc)

# 1. Global settings --------------------------------------------------------
# Set the work and data directory my images are in data
DATA_DIR <- sprintf('%s/%s', getwd(), 'data');

# Settings 250 x 250 m
PATCH.SIZE <- 250

# Eurocrop file 
EUROPE  <- 'EUCROPMAP_2018'

# Country Luxemburg
COUNTRY <- 'LUX'

# The selected crop types
# sort(unique(tab.country$tp))
CROP.SELECT <- c(211, 212, 213, 214, 215, 216, 217)

# 2. Data access as proposed ------------------------------------------------
# Create the file names
file.large   <- sprintf('%s/%s.tif', DATA_DIR, EUROPE)
file.country <- sprintf('%s/%s_%s.tif',
                        DATA_DIR, EUROPE, COUNTRY)
file.country.250 <- sprintf('%s/%s_%s_250.tif',
                        DATA_DIR, EUROPE, COUNTRY)

# Download the file if it not exists (Large 6.2 GiB
if ( ! file.exists(file.large) {
  download.file('https://jeodpp.jrc.ec.europa.eu/ftp/jrc-opendata/EUCROPMAP/2018/EUCROPMAP_2018.tif',
                file.large);
}

# Open the large raster file
rst.large  <- raster(file.large)

# Open a layer to partition the large set here the country polygon
poly.country.orig <- st_as_sf(getData(name = "GADM",
                                      country = COUNTRY, level = 2))

# Use the projection of the raster
poly.country <- st_transform(poly.country.orig, crs(rst.large))

# Create a subset for the country
rst.country <- crop(rst.large, poly.country)
tab.country <- as.data.frame(rst.country, xy=TRUE)

# Shorten the column names
names(tab.country)  <- c('x','y','tp')

# Aggregation size == 250x250 m
tab.country$ix <- trunc(tab.country$x/PATCH.SIZE)*PATCH.SIZE
tab.country$iy <- trunc(tab.country$y/PATCH.SIZE)*PATCH.SIZE

# Prepare the counting for the crops
tab.country$crop <- 0
tab.country$crop[ tab.country$tp %in% CROP.SELECT ] <- 1

# Aggregate the crop type using sums in the larger cell conglomerate
# My device needs for the aggregation in larger cells about
# 171.379 seconds for Luxembourg
tic('aggregation in larger cells')
tab.250 <- aggregate( crop ~ ix + iy, data = tab.country, FUN = sum)
toc()

# Calculate the norm ..25x25 = 625 to ge the right percentage
# TODO NA values are not considered yet
res.country <- res(rst.country)
norm.cells  <- prod( c(PATCH.SIZE, PATCH.SIZE)/ res.country)

# Create the average .. IMO it is a percentage of crop cells
tab.250$crop <- tab.250$crop/norm.cells

# Translate the table back to an raster
rst.250 <- raster(
              ncol = length(unique(tab.250$ix)),
              nrow = length(unique(tab.250$iy)),
              xmn  = min(tab.250$ix),
              xmx  = max(tab.250$ix),
              ymn  = min(tab.250$iy),
              ymx  = max(tab.250$iy),
              crs  = crs(rst.large))

cells <- cellFromXY(rst.250, tab.250[,c('ix','iy')])
rst.250[cells] <- tab.250$crop

# Show a summary
summary(rst.250)
hist(tab.250$crop)

# Plot the raster
plot(rst.250)
plot(poly.country$geometry, add = T)

# Store the resulting raster ...
writeRaster(rst.250, file.country.250)

# EOF.
