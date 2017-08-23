// =====================================================================
// Cut von sub samples (small images) from a geotiff
// (c) - 2013 A. Weidauer  alex.weidauer@huckfinn.de
// All rights reserved to A. Weidauer
// =====================================================================
// tif-cut.c is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// tif-cut.c is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with tif-cut.c.  If not, see <http://www.gnu.org/licenses/>.
// =====================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <gdal.h>
#include <ogr_srs_api.h>
#include <cpl_conv.h>
#include <cpl_string.h>
#include "alg.h"

// -------------------------------------------------------------------
int main(int argc, char **argv)
{

  char *ERROR_PRM = "Invalid numeric value %s: %s\n!";

  // register GDAL drivers
  GDALAllRegister();

  // set format GTiff with error handling 
  const char *format = "GTiff";
  GDALDriverH h_drv = GDALGetDriverByName( format );
  if( h_drv == NULL ) {
    error_exit(10,"Driver %s not avialable!" ,format);
  }

  // Test to write the format
  char **test_meta;
  test_meta = GDALGetMetadata( h_drv, NULL );
  if( ! CSLFetchBoolean( test_meta, GDAL_DCAP_CREATE, FALSE ) ) {
    error_exit(10,"Format %s is not writeable" ,format);
  }

  // Check the minimum of cmd params
  if (argc<6) {
    error_exit(10,
	"Missing parameter at least 6\nUsage %s IN OUT EXT SZ ID X Y ID X Y ID X Y....!\n",
	argv[0]);
  }

  // Read infile pattern from cli
  char *ifile = argv[1];

  // Read outfile pattern from cli
  char *ofile = argv[2];

  // var for the resulting filename
  char cfile[1024];

  // Read file extension from cli
  char *ext   = argv[3];

  // Set default window size and try to read from cli
  int size = 64;
  if (! sscanf(argv[4],"%d",&size) ) {
    error_exit(1000+3,ERROR_PRM,"SZ",argv[4]);
  }

  // Init affine transfomation vector 
  double trfm[] ={0,0,0,0,0,0};

  // init vectors for id's and positions
  int_vector_t id;
  int_vector_init(&id, 10);
  dbl_vector_t pos_x;
  dbl_vector_init(&pos_x, 10);
  dbl_vector_t pos_y;
  dbl_vector_init(&pos_y, 10);

  // Read center positions of the window from cli
  int a = 5; double dbl; int pk;
  while( a < argc-2 ) {

    // parse id coordinate
    if (! sscanf(argv[a],"%d",&pk) ) {
      error_exit(1000+a,ERROR_PRM,"ID", argv[a]);
    }
    int_vector_add(&id,pk);

    // parse x coordinate
    if (! sscanf(argv[a+1],"%lf",&dbl) ) {
      error_exit(1000+a+1,ERROR_PRM,"X", argv[a+1]);
    }
    dbl_vector_add(&pos_x,dbl);

    // parse y coordinate
    if (! sscanf(argv[a+2],"%lf",&dbl) ) {
      error_exit(1000+a+2,ERROR_PRM,"Y", argv[a+2]);
    }
    dbl_vector_add(&pos_y,dbl);

    a+=3;
  }

  // open geotiff and handle error
  printf("# IN FILE:  %s\n", ifile);
  GDALDatasetH h_dsrc = GDALOpen( ifile, GA_ReadOnly);
  if( h_dsrc == NULL ) {
    error_exit(10, "File %s is not accessible!\n", ifile);
  }
  printf("# OUT FILE: %s\n", ofile);
  printf("# EXTENTION: %s\n", ext);

  // Read transformation and handle error
  if( GDALGetGeoTransform( h_dsrc, trfm ) == CE_None ) {
    printf("# TRANSFORM: \n");
    printf("#  X = %.6f + %.6f * COL + %.6f * ROW\n",
	   trfm[0], trfm[1], trfm[2] );
    printf("#  Y = %.6f + %.6f * COL + %.6f * ROW\n# EOF:\n",
	   trfm[3], trfm[4], trfm[5] );
  } else {
    error_exit(10, "Transformation not avialable in TIFF!\n");
  }
  // Read the spatial reference system and/or handel error
  const char* h_ssrs = GDALGetProjectionRef(h_dsrc);
  if (h_ssrs == NULL) {
    error_exit(10, "Missing spatial reference system in TIFF!\n");
  }
  OGRSpatialReferenceH h_dsrs = OSRNewSpatialReference(h_ssrs);

  // Get image dimensions and numer of bands
  int img_width  = GDALGetRasterXSize( h_dsrc );
  int img_height = GDALGetRasterYSize( h_dsrc );
  int num_bands  = GDALGetRasterCount (h_dsrc );

  // Get band infos 
  GDALRasterBandH h_band[num_bands];
  GDALDataType    h_type[num_bands];
  int             h_tsize[num_bands];
  for(int b=0 ; b<num_bands; b++) {
    h_band[b]  = GDALGetRasterBand( h_dsrc, b+1 );
    h_type[b]  = GDALGetRasterDataType(h_band[b]);
    h_tsize[b] = GDALGetDataTypeSize(h_type[b]);
  }

  // Create sub images 
  for (int c=0; c< pos_x.length; c++ ) {

    // Transform cut position (world) to image positions
    long icol = -1; long irow = -1;
    trfm_geo_pix(trfm, pos_x.data[c], pos_y.data[c],
		 &icol , &irow);

    // Create filename from patter id and extention
    sprintf(cfile,"%s.%d%s",ofile, id.data[c],ext);

    // Test if the window is inside the image and
    // skip the stuff if outside 
    if (icol-size/2<=0 ||
	irow-size/2<=0 ||
	icol+size/2>=img_width ||
	irow+size/2>=img_height){
      printf ("IGN %d %s\n",id.data[c],ofile);
      continue;
    }

    // cut the sub image
    printf ("ADD %d %s\n",id.data[c],ofile);

    int ioffs_col = icol-size/2;
    int ioffs_row = irow-size/2;

    // create the tiff
    // char **options = NULL;
    GDALDatasetH h_ddst = GDALCreate( h_drv,
		 cfile, size, size, num_bands, h_type[0], NULL);

    // Set transformation  
    double foffx = icol-size/2.0;
    double foffy = irow-size/2.0; 
    double goffx = 0;
    double goffy = 0; 
    trfm_pix_geo( trfm, (int) round(foffx), (int) round(foffy), &goffx, &goffy); 
    double trfm_dst[6] = { goffx, trfm[1], trfm[2], goffy, trfm[4], trfm[5] };

    // Copy spatial reference system 
    char *dsrs_wkt = NULL; 
    OSRExportToWkt( h_dsrs, &dsrs_wkt );
    GDALSetGeoTransform( h_ddst, trfm_dst);
    GDALSetProjection( h_ddst, dsrs_wkt);
    CPLFree( dsrs_wkt );

    // Iteration over all bands
    for (int b=0; b < num_bands; b++) {

      // alloc io buffer for the copy @todo static
      void *io_buffer = CPLMalloc(h_tsize[0] * size * size);

      // copy pixels
      GDALRasterIO( h_band[b], GF_Read,
		    ioffs_col, ioffs_row, size, size,
		    io_buffer, size, size, h_type[b], 0, 0 );
      // address pixel to band handle of and copy the io buffer
      GDALRasterBandH h_band_out = GDALGetRasterBand(h_ddst, b+1);
      GDALRasterIO( h_band_out, GF_Write, 0, 0, size, size,
		    io_buffer, size, size, h_type[b], 0, 0 );
      // free io buffer
      CPLFree(io_buffer);
    }
    // write the tif and flush the stuff
    GDALClose( h_ddst );

  } // EOF positions

  // Close source image
  GDALClose( h_dsrc);
  return 0;
}

// --- EOF -----------------------------------------------------------

