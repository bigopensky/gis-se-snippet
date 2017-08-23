// =====================================================================
// Service routines
// (c) - 2013 A. Weidauer  alex.weidauer@huckfinn.de
// =====================================================================
// alg.c is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// alg.c is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with alg.c.  If not, see <http://www.gnu.org/licenses/>.
// =====================================================================
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <gsl/gsl_linalg.h>
#include "alg.h"

// ---------------------------------------------------------------
/**
 * Central exit code
 * @param code Exit code
 * @param message Erro message
 * @param ... Parameter String
 */
void error_exit(int code, const char *message, ...) {
    va_list arglist;
    va_start(arglist,message);
    vfprintf(stderr, message, arglist);
    va_end(arglist);
    exit(code);
}

// ---------------------------------------------------------------
/** Calculates a transformation with souce and target coordinates
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
int trfm_create(dbl_vector_t *src_x,
        dbl_vector_t *src_y,
        dbl_vector_t *dst_x,
        dbl_vector_t *dst_y,
        double *result) {

    // Test sufficient number of values in src_x
    if (src_x->length < 3) return -1;

    // Test vecorlength matches
    if (src_x->length != src_y->length ||
            dst_x->length != dst_y->length ||
            src_x->length != dst_x->length ||
            src_y->length != dst_y->length)
        return -2;

    // Init objects for the equation system
    gsl_matrix *M = gsl_matrix_alloc(3, 3);
    gsl_vector *B1 = gsl_vector_alloc(3);
    gsl_vector *B2 = gsl_vector_alloc(3);
    gsl_vector *CX = gsl_vector_alloc(3);
    gsl_vector *CY = gsl_vector_alloc(3);

    // Init zeros
    gsl_matrix_set_zero(M);
    gsl_vector_set_zero(B1);
    gsl_vector_set_zero(B2);
    gsl_vector_set_zero(CX);
    gsl_vector_set_zero(CY);

    // Build equations
    for (int i = 0; i < src_x->length; i++) {

        // Normalform of source 1 points
        M->data[0 * M->tda + 0] += src_x->data[i] * src_x->data[i];
        M->data[0 * M->tda + 1] += src_x->data[i] * src_y->data[i];
        M->data[0 * M->tda + 2] += src_x->data[i];

        // Normalform of source 2 points
        M->data[1 * M->tda + 0] += src_x->data[i] * src_y->data[i];
        M->data[1 * M->tda + 1] += src_y->data[i] * src_y->data[i];
        M->data[1 * M->tda + 2] += src_y->data[i];

        // Mixed terms 1 source destination points
        B1->data[0 * B1->stride] += dst_x->data[i] * src_x->data[i];
        B1->data[1 * B1->stride] += dst_x->data[i] * src_y->data[i];
        B1->data[2 * B1->stride] += dst_x->data[i];

        // Mixed terms 2 source destination points
        B2->data[0 * B2->stride] += dst_y->data[i] * src_x->data[i];
        B2->data[1 * B2->stride] += dst_y->data[i] * src_y->data[i];
        B2->data[2 * B2->stride] += dst_y->data[i];
    }

    // Set missing comps
    gsl_matrix_set(M, 2, 2, src_x->length);
    gsl_matrix_set(M, 2, 1, gsl_matrix_get(M, 1, 2));
    gsl_matrix_set(M, 2, 0, gsl_matrix_get(M, 0, 2));

    // Solve equation
    int s = 0;
    gsl_permutation * P = gsl_permutation_alloc(3);
    gsl_linalg_LU_decomp(M, P, &s);
    gsl_linalg_LU_solve(M, P, B1, CX);
    gsl_linalg_LU_solve(M, P, B2, CY);

    // Free data
    gsl_matrix_free(M);
    gsl_vector_free(B1);
    gsl_vector_free(B2);
    gsl_permutation_free(P);

    // Set resulting vector
    for (int i = 0; i < 3; ++i) {
        result[i] = gsl_vector_get(CX, i);
        result[i + 3] = gsl_vector_get(CY, i);
    }
    // Free solution vercors
    gsl_vector_free(CX);
    gsl_vector_free(CY);
    return 1;
}

//---------------------------------------------------------------------
/**
 * Calculate new coordinates using an affine transformation
 * @param trfm the transformation
 * @param x source coordinate X
 * @param y source coordinate Y
 * @param tx target coordinate X
 * @param ty target coordinate Y
 */
void trfm_calc(double *trfm, double x, double y, double *tx, double *ty) {
    *tx = trfm[0] * x + trfm[1] * y + trfm[2];
    *ty = trfm[3] * x + trfm[4] * y + trfm[5];
}


// ---------------------------------------------------------------
/**
 * Initialize a dynamic doule vector
 * @param vec dynamic vector structure
 * @param size initial size
 */
void dbl_vector_init(dbl_vector_t *vec, size_t size) {
    vec->data = (double *) malloc(size * sizeof (double));
    vec->length = 0;
    vec->mem_size = size;
}

// ---------------------------------------------------------------
/**
 * Add element to a dynamic doule vector
 * @param vec dynamic vector structure
 * @param element to add
 */
void dbl_vector_add(dbl_vector_t *vec, double element) {
    if (vec->length == vec->mem_size) {
        vec->mem_size += vec->mem_size;
        vec->data = (double *) realloc(vec->data,
                     vec->mem_size * sizeof (double));
    }
    vec->data[vec->length++] = element;
}

// ---------------------------------------------------------------
/**
 * Free a dynamic double vector
 * @param vec the vector
 */
void dbl_vector_free(dbl_vector_t *vec) {
    free(vec->data);
    vec->data = NULL;
    vec->length = vec->mem_size = 0;
}

// ---------------------------------------------------------------
/**
 * Initialize a dynamic integer vector
 * @param vec the vector structure
 * @param size initial size
 */
void int_vector_init(int_vector_t *vec, size_t size) {
    vec->data = (int *) malloc(size * sizeof (int));
    vec->length = 0;
    vec->mem_size = size;
}

// ---------------------------------------------------------------
/**
 * Add a emlement to a dynamic int vector
 * @param vec
 * @param element
 */
void int_vector_add(int_vector_t *vec, int element) {
    if (vec->length == vec->mem_size) {
        vec->mem_size += vec->mem_size;
        vec->data = (int *) realloc(vec->data,
				 vec->mem_size * sizeof (int));
    }
    vec->data[vec->length++] = element;
}

// ---------------------------------------------------------------
/**
 * Free a dynamic int vector
 * @param vec the vector struct
 */
void int_vector_free(int_vector_t *vec) {
    free(vec->data);
    vec->data = NULL;
    vec->length = vec->mem_size = 0;
}


// --- EOF ---------------------------------------------------------
