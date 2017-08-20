#ifndef AWE_ALG_H
#define AWE_ALG_H
// =====================================================================
// Serviceroutinen
// (c) - 2013 A. Weidauer  alex.weidauer@huckfinn.de
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
// along with alg.h.  If not, see <http://www.gnu.org/licenses/>.
// =====================================================================

// Maschine Null DOUBLE
#define DBL_EPSILON 2.2204460492503131e-16 
#define ERR_TRFM_SIZE_EQUAL 3010;
#define ERR_TRFM_3_POINTS   3020;

// ===========================================================================
/**
 * Structure dynamic double vector
 */
typedef struct {
    double * data;
    size_t length;
    size_t mem_size;
} dbl_vector_t;

// ---------------------------------------------------------------
/**
 * Structure dynamic int vector
 */
typedef struct {
    int * data;
    size_t length;
    size_t mem_size;
} int_vector_t;

// =================================================================
/**
 * Central exit code 
 * @param code Exit code
 * @param message Erro message
 * @param ... Parameter String
 */
void error_exit(int code, const char *message, ...);

// ---------------------------------------------------------------
/**
 * Initialize a dynamic doule vector
 * @param vec dynamic vector structure
 * @param size initial size
 */
void dbl_vector_init(dbl_vector_t *vec, size_t size);

// ---------------------------------------------------------------
/**
 * Add element to a dynamic doule vector
 * @param vec dynamic vector structure
 * @param element to add
 */
void dbl_vector_add(dbl_vector_t *vec, double element);

// ---------------------------------------------------------------
/**
 * Free a dynamic double vector
 * @param vec the vector
 */
void dbl_vector_free(dbl_vector_t *vec);

// -------------------------------------------------------------------
/**
 * Initialize a dynamic integer vector
 * @param vec the vector structure
 * @param size initial size
 */
void int_vector_init(int_vector_t *vec, size_t size);

// ---------------------------------------------------------------
/**
 * Add a emlement to a dynamic int vector
 * @param vec 
 * @param element
 */
void int_vector_add(int_vector_t *vec, int element);

// ---------------------------------------------------------------
/**
 * Free a dynamic int vector
 * @param vec the vector struct
 */
void int_vector_free(int_vector_t *vec);

// ---------------------------------------------------------------
/** Calculates a transformation with souce and target coordinates
 *  and minimize the average quadratic error (residuals)
 *  @param src_x - vector x component source coordinate system
 *  @param src_y - vector y component source coordinate system
 *  @param dst_x - vector y component destinatio coordinate system
 *  @param dst_y - vector y component destination coordinate system
 *  @param result - vecor for the affine transformation
 *  @returns 1 if succesfull
 *          -1 poor passpoint numbers
 *          -2 vector length mismatch
 */
int trfm_create(dbl_vector_t *src_x,  dbl_vector_t *src_y,
				dbl_vector_t *dst_x,  dbl_vector_t *dst_y,
                double *result);

//---------------------------------------------------------------------
/**
 * Calculate new coordinates using an affine transformation
 * @param trfm the transformation
 * @param x source coordinate X
 * @param y source coordinate Y
 * @param tx target coordinate X
 * @param ty target coordinate Y
 */
void trfm_calc(double *trfm, double x, double y, double *tx, double *ty);

#endif
