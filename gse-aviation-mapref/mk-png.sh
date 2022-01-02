#!/usr/bin/env bash
/usr/bin/gdal_translate \
     -gcp 099 090 -2097489.33 2412885.46 \
     -gcp 368 137 -2767377.74 1600967.89 \
     -gcp 485 453 -4070468.40 1817399.23 \
     -gcp 312 459 -3749152.78 2411536.10 \
     -a_srs '+proj=stere +lat_0=90 +lat_ts=60' \
     -of PNG \
     -co WORLDFILE=YES \
     data/Latest-gfacn31_cldwx_000.png \
     data/Latest-gfacn31_cldwx_000_trans.png
