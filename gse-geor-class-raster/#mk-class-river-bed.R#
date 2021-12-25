# mk-class-river-bed.R  -----------------------------------------------------
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

# 1. Data access ------------------------------------------------------------

# Load the raster driver kit
require(rgdal)
require(raster)

# GIS Stack exchange data directory
GSE_DATA <- './data';
GSE_PLOT <- './plot';

# Set the work and data directory
DATA_DIR <- sprintf('%s/%s', getwd(), GSE_DATA);

# Create the file names - 
# source A
file.a <- sprintf('%s/%s', DATA_DIR,'A.tif')

file.b <- sprintf('%s/%s', DATA_DIR,'B.tif')
file.c <- sprintf('%s/%s', DATA_DIR,'C.tif')

# Copy the file A to get the metric for
# resolution CRS etc.  dirty variant
file.copy(file.a, file.c)

# Open the source files
A = GDAL.open(file.a)
B = GDAL.open(file.b)

# Open the result file writeable
C = GDAL.open(file.c, read.only = FALSE)

# 2. Investigate the input ---------------------------------------------

# Get some summary
GDALinfo(file.a)
GDALinfo(file.b)

# Plot the source files
displayDataset(A)
displayDataset(B)

# 3. Extract the raster data and buld the difference  ------------------
mx.A <- getRasterData(A)
mx.B <- getRasterData(B)
mx.C <- getRasterData(C)

# We have the matrix and can
# close the data sources
GDAL.close(A)
GDAL.close(B)

# Build the difference
mx.D <- mx.A-mx.B

# Show the difference
hist(mx.D,
     main = "Difference of the river bed's",
     xlab = "Difference between river bed levels.")

# Show the differences (
# t() is used to transpose mx.D
# ..matrix address vs map cells
plot(t(raster(mx.D)))

# 4. Apply the rule set ------------------------------------------------
# Get the range of the differences to build the threshold
range.dif <- range(mx.D, na.rm = TRUE)
thresh    <- diff(range(mx.D, na.rm=TRUE))/10

# Clear all in data in MX C
mx.C[,] <- 0

# Apply the rules ..not all implemented
# you could use quantiles here
mx.C[ (mx.D > -thresh) & (mx.D < thresh)   ]          <- 10
mx.C[ (mx.A > 0) & (mx.B > 0) & (  mx.D < thresh )  ] <- 20
mx.C[ (mx.A < 0) & (mx.B < 0) & ( -mx.D < thresh )  ] <- 30
mx.C[ (mx.A > 0) & (mx.B < 0) ]       <- 40
mx.C[ (mx.A < 0) & (mx.B > 0) ]       <- 50
mx.C[ (mx.A > 0) & (mx.B > 0) & (mx.A > mx.B) ]       <- 60
mx.C[ (mx.A > 0) & (mx.B > 0) & (mx.A < mx.B) ]       <- 70
# etc. ... many more

# Show the colored raster map
plot(t(raster(mx.C)))

# 5. Put the classified data back to the Geotiff and store the file ----
putRasterData(C, mx.C)

# Show the Geotiff if you want
displayDataset(C)

# Store the Geotiff
saveDataset(C, file.c)

# CLose the resulting raster
GDAL.close(C)

# EOF.SKETCH ------------------------------------------------------------
