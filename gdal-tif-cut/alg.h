#ifndef AWE_ALG_H
#define AWE_ALG_H
// =====================================================================
// Serviceroutines
// (c) - 2013 A. Weidauer  alex.weidauer@huckfinn.de
// All rights reserved to A. Weidauer
// =====================================================================
// alg.h is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// alg.h is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with alg,h.  If not, see <http://www.gnu.org/licenses/>.
// =====================================================================



// Maschine ZERO DOUBLE
#define DBL_EPSILON 2.2204460492503131e-16

// Error codes
#define ERR_TRFM_SIZE_EQUAL 3010;
#define ERR_TRFM_3_POINTS   3020;

// ===========================================================================
/**
 * Dynamic double vector array 
 */
typedef struct {
    double * data;
    size_t length;
    size_t mem_size;
} dbl_vector_t;

// ---------------------------------------------------------------
/**
 * Dynamic int vector array 
 */
typedef struct {
    int * data;
    size_t length;
    size_t mem_size;
} int_vector_t;

// =================================================================
/**
 * central error exit point
 * @param code    exit code
 * @param message message template sprintf format
 * @param ...     parameter
 */
void error_exit(int code, const char *message, ...);

// ---------------------------------------------------------------
/**
 * initialize a dynamic double vector
 * @param vec dynamic vector structure
 * @param size initial size
 */
void dbl_vector_init(dbl_vector_t *vec, size_t size);

// ---------------------------------------------------------------
/**
 * add element to a dynamic double vector
 * @param vec dynamic vector structure
 * @param element to add
 */
void dbl_vector_add(dbl_vector_t *vec, double element);

// ---------------------------------------------------------------
/**
 * free a dynamic double vector
 * @param vec the vector
 */
void dbl_vector_free(dbl_vector_t *vec);

// ---------------------------------------------------------------
/**
 * initialize a dynamic integer vector
 * @param vec the vector structure
 * @param size initial size
 */
void int_vector_init(int_vector_t *vec, size_t size);

// ---------------------------------------------------------------
/**
 * add a emlement to a dynamic int vector
 * @param vec
 * @param element
 */
void int_vector_add(int_vector_t *vec, int element);

// ---------------------------------------------------------------
/**
 * free a dynamic int vector
 * @param vec the vector struct
 */
void int_vector_free(int_vector_t *vec);

// -------------------------------------------------------------------
/**
 * transformation from pixel to world coordinates 
 * @param trfm affine transformation array of double entries
 *        x = trfm[0] + trfm[1] * col + trfm[2] * row
 *        y = trfm[3] + trfm[4] * col + trfm[5] * row
 * @param col column in image
 * @param row row in image
 * @param x X-coordinate globale data
 * @param y Y-coordinate globale data
 */
void trfm_pix_geo(double *trfm, double col, double row,
                  double *x , double *y);


// -------------------------------------------------------------------
/**
 * transformation from world to pixel coordinates
 * @param trfm affine transformation array of 6 parameter
 * @param x X-coordinate globale data
 * @param y Y-coordinate globale data
 * @param col column in image
 * @param row row in image
 * @return true if OK
 */
int trfm_geo_pix(double *trfm, double x, double y,
                long *col , long* row);

// ---------------------------------------------------------------
/** calculates a transformation with souce and target coordinates
 *  and minimize the average quadratic error (residuals)
 *  @param src_x - vector x component source coordinate system
 *  @param src_y - vector y component source coordinate system
 *  @param dst_x - vector y component destinatio coordinate system
 *  @param dst_y - vector y component destination coordinate system
 *  @param result - vector for the affine transformation
 *  @returns 1 if succesfull
 *          -1 poor passpoint numbers
 *          -2 vector length mismatch
 */
int trfm_create(dbl_vector_t *src_x,  dbl_vector_t *src_y,
				dbl_vector_t *dst_x,  dbl_vector_t *dst_y,
                double *result);


#endif
