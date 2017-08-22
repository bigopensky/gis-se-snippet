#!/usr/bin/Rscript --vanilla
# ----------------------------------------------
# Test reprojection accuracy of proj.4
# Stack exchange question:
# https://gis.stackexchange.com/questions/180097
# ----------------------------------------------
# (c) 2016-02 huckfinn
# ----------------------------------------------
# test comandline args
args <- commandArgs(trailingOnly = TRUE)
if (length(args)<1) {
    stop("Usage: test-repro.R num-loops:int",
         call.=FALSE)
}

# number of iterations
num = strtoi(args[1])

require('proj4',quiet=TRUE)


# Check the number of iterations
if (num < 10) {
    stop("Please choose a large number like 10000!\n",
         call.=FALSE)
}

# Output digist statistics
DIGIT = 6

# the LON/LAT frame of NAD83 UTM 17
lon = c(-84.00, -78.00, -84.00, -78.00 )
lat = c( 24.00,  24.00,  83.00,  83.00)

# build the projection conform object
ll0 = matrix(c(lon,lat),nrow=4,ncol=2)
xy0 = project(ll0,"+init=epsg:26917",ellps.default='GRS80')

# make a copy
ll1 = ll0
xy1 = xy0

# reproject the stuff num times
for(i in 1:num) {
 # project forward
 xy1 = project(ll1,"+init=epsg:26917", ellps.default='GRS80')
 # project backward
 ll1 = project(xy1,"+init=epsg:26917", inverse=T, ellps.default='GRS80')
}

# build difference table ll
dll = as.data.frame(ll1-ll0)
names(dll) = c('DLON','DLAT')

# print results LON/LAT
cat(paste("# RES.LON/LAT: differences after", num,"loops\n"))
row.names(dll) <- sprintf('DLON.DLAT.%02d:',seq(1,dim(dll)[1]));
print(dll)
cat("\n")

# build difference table xy
dxy = as.data.frame(xy1-xy0)
names(dxy) = c('DX','DY')

# print results X/Y
cat(paste("# RES.X/Y differences after", num,"loops\n"))
row.names(dxy) <- sprintf('DX.DY.%02d:',seq(1,dim(dxy)[1]));
print(dxy)

# create a statistic for the 4 corner points
cat("\n");
cat(paste("# RES.STATS differences after", num,"loops\n"))
dxy$D <- sqrt(dxy$DX^2+dxy$DY^2) 
ds <- data.frame (N = num,
                  MX = round(mean(dxy$DX),DIGIT),
                  SX = round(sd(dxy$DX),  DIGIT),
                  MY = round(mean(dxy$DY),DIGIT),
                  SY = round(sd(dxy$DY),  DIGIT),
                  M  = round(mean(dxy$D),DIGIT),
                  S  = round(sd(dxy$D), DIGIT)
) 
row.names(ds) <- 'STAT:'
print(ds)
                 
# EOF
