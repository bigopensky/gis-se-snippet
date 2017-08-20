// ==================================================================
// Calculate the average affine transfomation
// (c) - 2012 A. Weidauer  alex.weidauer@huckfinn.de
// All rights reserved to A. Weidauer
// ==================================================================
// gpc-trfm.c is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// gpc-trfm.c is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with gpc-trfm.c.  If not, see <http://www.gnu.org/licenses/>.
// =====================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "alg.h"

// ====================================================================
// Main program
// ====================================================================
int main(int argc, char* argv[]) {

    // Text message template for usage
    char *msg_usage =
            "USAGE: %s [-h -d -s CHAR] -p sx sy dx dy -p sx sy dx dy -p sx sy dx dy [-p ...] -c x y [-c x y ...]\n";

    // Error text template ..use once
    char *error_tmpl_single = "Please use option '%s' not twice!";

    // Error text template numerical error
    char *error_tmpl_num = "Invalid numerical value for %s: %s\n!";

	// Output decoration
    char *FOOTER =
    "# EOF\n\n";
    char *TITLE =
    "# TOOL: GPC TRANSFORM\n";
    char *HEADER =
    "# HEAD: SRC.X SRC.Y TRFM.X TRFM.Y ERROR.X ERROR.Y\n";

    // Output separator default value
    char *sep = " "; int parse_sep = 0;

    // Debug Modus
    int DEBUG = 0;
    int parse_debug = 0;

    // List
    int list_trfm = 0;
    int list_itrfm = 0;

    // Current CLI-Tag
    char *cur_arg = NULL;

    // Init pass point vectors
    dbl_vector_t pass_src_x;
    dbl_vector_init(&pass_src_x, 3);
    dbl_vector_t pass_src_y;
    dbl_vector_init(&pass_src_y, 3);
    dbl_vector_t pass_dst_x;
    dbl_vector_init(&pass_dst_x, 3);
    dbl_vector_t pass_dst_y;
    dbl_vector_init(&pass_dst_y, 3);

    // Init transformation
    double trfm[6] = {0, 0, 0, 0, 0, 0};
    // Init inverse transformation
    double itrfm[6] = {0, 0, 0, 0, 0, 0};

    // Vars for calculations and cli parsing
    double x = 0;
    double y = 0;
    // Transformed data
    double tx = 0;
    double ty = 0;
    // Inverse transformd data
    double itx = 0;
    double ity = 0;

    // Flag first tage was read/parsed
    int parse_coord = 0;

    // Flag invers pasing
    int inverse = 0;
    int parse_inverse = 0;

    // Parameter counter
    int cnt_arg = 0;

    // Result of the pp calculation
    int res = 0;

    // Greetings
    printf(TITLE);

    // Print usage in missing prams
    if (argc < 1) {
        fprintf(stderr, msg_usage, argv[0]);
        exit(1);
    }

    // Read parameter from cli
    while (cnt_arg < argc - 1) {

        // read current tag
        cur_arg = argv[++cnt_arg];

        // control message
        if (DEBUG) {
            printf("# ARG.NUM: %d ARG.VAL: %s ARG.PRM %s\n",
                cnt_arg, cur_arg, argv[cnt_arg + 1]);
        }

        // print help message
        if (strncmp(cur_arg, "-h", 3) == 0 || strncmp(cur_arg, "--help", 7) == 0) {
            printf("Calculation of an affine transformation");
            printf("by a set of passpoint coordinates\n\n");
            printf(msg_usage, "gpc-trfm");
            printf("\nOutput:\n");
            printf("# ... Metadata\n");
            printf("src.x src.x trfm.x trfm.y err.x err.y\n");
            printf("...\n\n");
            printf("Options:\n");
            printf(" -h show help\n");
            printf(" -d debug \n");
            printf(" -i inverse calculation\n");
            printf(" -t list transformations parser friendly\n"); 
            printf(" -s CHAR output column separator\n"); 
            printf(" -p sx sy dx dy -p sx sy dx dy ... passpoints\n");
            printf("    sx sy  source point\n");
            printf("    dx dy  destinatio point\n");
            //  printf(" -m calculate transforms without coordinates\n");
            printf(" -c x y -c x y ...coordinates to transform\n\n");
            printf("Examples:\n");
            printf("  gpc_trfm -s \"|\" -p 1 0 10 0 -p 0 1 0 10 -p 1 1 10 10 -c 1 1 -c 1 0\n");
            printf("  gpc_trfm -i -p 1 0 10 0 -p 0 1 0 10 -p 1 1 10 10 -c 1 1 -c 1 0\n\n");
            printf("(c) - 2008 Alexander Weidauer; alex.weidauer@huckfinn.de\n\n");
            exit(0);

        // Prevent to set debug flag twice
        } else if (strncmp(cur_arg, "-d", 3) == 0 && parse_debug) {
            error_exit(11,error_tmpl_single, "-d");
        }

        //  Set debug flag
        else if (strncmp(cur_arg, "-d", 3) == 0 && !parse_debug) {
            parse_debug = 1;
            DEBUG = 1;
        }

        //  Set list transform flag
        else if (strncmp(cur_arg, "-t", 3) == 0 ) {
            list_trfm = 1;
        }

        // Prevent to set inverse flag twice
        else if (strncmp(cur_arg, "-i", 3) == 0 && parse_inverse) {
            error_exit(11,error_tmpl_single, "-i");
        }

        //  Set inverse transformation flag
        else if (strncmp(cur_arg, "-i", 3) == 0 && !parse_inverse) {
            parse_inverse = 1;
            inverse = 1;

         // Prevent to set teh separator char twice
        } else if (strncmp(cur_arg, "-s", 3) == 0 && parse_sep) {
            error_exit(11, error_tmpl_single, "-s");
        }

        // read the separator -s
        else if (strncmp(cur_arg, "-s", 3) == 0 && !parse_sep) {
            if (cnt_arg < argc) {
                sep = argv[cnt_arg + 1];
            } else {
                error_exit(12, msg_usage, argv[0]);
            }
            cnt_arg++;
            parse_sep = 1;
        } // eof red separator

        // Prevent mixing passpoints an tcoordinates
        else if (strncmp(cur_arg, "-p", 3) == 0 && parse_coord) {
            error_exit(1000 + cnt_arg,
            "Mixing of passpoints and coordinate param is not possible!");
        } else

            // Read passpoints
            if (strncmp(cur_arg, "-p", 3) == 0 && !parse_coord) {

            // enough cli args left?
            if (cnt_arg + 4 >= argc) {
                error_exit(10, msg_usage, argv[0]);
            }

            // parse x source coordinate
            if (!sscanf(argv[cnt_arg + 1], "%lf", &x)) {
              error_exit(1000 + cnt_arg + 1,
                error_tmpl_num,"SRC.X",argv[cnt_arg + 1]);
            }

            // parse y source coordinate
            if (!sscanf(argv[cnt_arg + 2], "%lf", &y)) {
              error_exit(1000 + cnt_arg + 2,
                error_tmpl_num,"SRC.Y",argv[cnt_arg + 2]);
            }

            // controll message
            if (DEBUG) printf("# CLI.POINT %s %s : PASS.SRC %f %f\n",
                    argv[cnt_arg + 1], argv[cnt_arg + 2], x, y);

            // add to source vector
            dbl_vector_add(&pass_src_x, x);
            dbl_vector_add(&pass_src_y, y);

            // parse x destination coordinate
            if (!sscanf(argv[cnt_arg+3], "%lf", &x)) {
              error_exit(1000 + cnt_arg + 3,
                error_tmpl_num,"DST.Y",argv[cnt_arg + 3]);
            }

	    // parse y destination coordinte
            if (!sscanf(argv[cnt_arg+4], "%lf", &y)) {
              error_exit(1000 + cnt_arg + 3,
                error_tmpl_num,"DST.Y",argv[cnt_arg + 4]);
            }

            // control message
            if (DEBUG) printf("# CLI.POINT %s %s : PASS.DST %f %f\n",
                    argv[cnt_arg + 3], argv[cnt_arg + 4], x, y);

	    // add dest vector
            dbl_vector_add(&pass_dst_x, x);
            dbl_vector_add(&pass_dst_y, y);

            // increase param counter
            cnt_arg += 4;

        }// EOF Option -p

        // Parse coordinates
        else if (strncmp(cur_arg, "-c", 3) == 0 ||
                 strncmp(cur_arg, "-m", 3) == 0  ) {

            // Create transformations if not exists
            if (!parse_coord) {

                // forward transform
                res = trfm_create(&pass_src_x, &pass_src_y,
                        &pass_dst_x, &pass_dst_y, trfm);
		if (res != 1) {
		  error_exit(2000,"Invalid %s transformation %d","forward",res);
		}
                // inverse transform
                res = trfm_create(&pass_dst_x, &pass_dst_y,
                        &pass_src_x, &pass_src_y, itrfm);
		if (res != 1) {
		  error_exit(2000,"Invalid %s transformation error %d","inverse",res);
		}

                // control message
                if (! list_trfm) { 
                 printf("# TRFM:  X = %f * x + %f * y + %f\n",
                  trfm[0], trfm[1], trfm[2]);
                 printf("# TRFM:  Y = %f * x + %f * y + %f\n",
                  trfm[3], trfm[4], trfm[5]);
                 printf("# ITRFM: X = %f * x + %f * y + %f\n",
                  itrfm[0], itrfm[1], itrfm[2]);
                 printf("# ITRFM: Y = %f * x + %f * y + %f\n",
                  itrfm[3], itrfm[4], itrfm[5]);
                } else {
                 printf("TRFMX:%s%f%s%f%s%f\n",
                        sep,trfm[0],sep,trfm[1],sep,trfm[2]);
                 printf("TRFMY:%s%f%s%f%s%f\n",
                        sep,trfm[3],sep, trfm[4],sep,trfm[5]);
                 printf("ITRFMX:%s%f%s%f%s%f\n",
                        sep,itrfm[0],sep,itrfm[1],sep,itrfm[2]);
                 printf("ITRFMY:%s%f%s%f%s%f\n",
                        sep,itrfm[3],sep,itrfm[4],sep,itrfm[5]);

                  
                }
                printf(HEADER);

                // Free passpoint vectors
                dbl_vector_free(&pass_src_x);
                dbl_vector_free(&pass_src_y);
                dbl_vector_free(&pass_dst_x);
                dbl_vector_free(&pass_dst_y);
            }

            // Switch to coordinate modus
            parse_coord = 1;

            // Transform coordinates
	    if ( strncmp(cur_arg, "-c", 3) == 0 ) {

	      // Enough agrs left
	      if (cnt_arg + 2 >= argc) {
                error_exit(10, msg_usage, argv[0]);
	      }

	      // parse X-Coordinate
	      if (!sscanf(argv[cnt_arg + 1], "%lf", &x)) {
		error_exit(1000 + cnt_arg + 1,
			   error_tmpl_num,"TRFM.X",argv[cnt_arg + 1]);
	      }

	      // parse Y-Koordinate
	      if (!sscanf(argv[cnt_arg + 2], "%lf", &y)) {
		error_exit(1000 + cnt_arg + 1,
			   error_tmpl_num,"TRFM.Y",argv[cnt_arg + 2]);
	      }

	      // Calc new coordinates inverse or foward
	      if (inverse) {
                trfm_calc(itrfm, x, y, &tx, &ty);
                trfm_calc(trfm, tx, ty, &itx, &ity);
	      } else {
                trfm_calc(trfm, x, y, &tx, &ty);
                trfm_calc(itrfm, tx, ty, &itx, &ity);
	      }

	      // Output
	      printf("COORD:%s%f%s%f%s%f%s%f%s%f%s%f\n",
		     sep,x, sep, y, sep,
		     tx, sep, ty, sep,
		     x - itx, sep, y - ity);

	      // Increase error counter
	      cnt_arg += 2;
            }
        }

        // Unknown switch
        else {
            error_exit(1000 + cnt_arg,
            "Invalid parameter %s\n", cur_arg);
        }
    }
    printf(FOOTER);
    exit(0);
}
// EOF--------------------------------------------------------------------------
