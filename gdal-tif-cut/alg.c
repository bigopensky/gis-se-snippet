// =====================================================================
// Serviceroutines
// (c) - 2013 A. Weidauer  alex.weidauer@huckfinn.de
// All rights reserved to A. Weidauer
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
 * central error exit point
 * @param code    exit code
 * @param message message template sprintf format
 * @param ...     parameter
 */
void error_exit(int code, const char *message, ...) {
    va_list arglist;
    va_start(arglist,message);
    vfprintf(stderr, message, arglist);
    va_end(arglist);
    exit(code);
}

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
int trfm_create(dbl_vector_t *src_x,
        dbl_vector_t *src_y,
        dbl_vector_t *dst_x,
        dbl_vector_t *dst_y,
        double *result) {

    // minimal number of points avialable
    if (src_x->length < 3) return -1;

    // vector length is equal for sourc an destination
    if (src_x->length != src_y->length ||
            dst_x->length != dst_y->length ||
            src_x->length != dst_x->length ||
            src_y->length != dst_y->length)
        return -2; 

    // init matrices and vectors
    gsl_matrix *M = gsl_matrix_alloc(3, 3);
    gsl_vector *B1 = gsl_vector_alloc(3);
    gsl_vector *B2 = gsl_vector_alloc(3);
    gsl_vector *CX = gsl_vector_alloc(3);
    gsl_vector *CY = gsl_vector_alloc(3);
    
    // fill zeros
    gsl_matrix_set_zero(M);
    gsl_vector_set_zero(B1);
    gsl_vector_set_zero(B2);
    gsl_vector_set_zero(CX);
    gsl_vector_set_zero(CY);

    // build equations 
    for (int i = 0; i < src_x->length; i++) {

        // normal form of sources 1st part
        M->data[0 * M->tda + 0] += src_x->data[i] * src_x->data[i];
        M->data[0 * M->tda + 1] += src_x->data[i] * src_y->data[i];
        M->data[0 * M->tda + 2] += src_x->data[i];

        // normal form of sources 2nd part
        M->data[1 * M->tda + 0] += src_x->data[i] * src_y->data[i];
        M->data[1 * M->tda + 1] += src_y->data[i] * src_y->data[i];
        M->data[1 * M->tda + 2] += src_y->data[i];

        // mixed terms 1 source destination points
        B1->data[0 * B1->stride] += dst_x->data[i] * src_x->data[i];
        B1->data[1 * B1->stride] += dst_x->data[i] * src_y->data[i];
        B1->data[2 * B1->stride] += dst_x->data[i];

        // Mmxed terms 2 source destination points
        B2->data[0 * B2->stride] += dst_y->data[i] * src_x->data[i];
        B2->data[1 * B2->stride] += dst_y->data[i] * src_y->data[i];
        B2->data[2 * B2->stride] += dst_y->data[i];
    }

    // setup remaining terms int normal form 
    gsl_matrix_set(M, 2, 2, src_x->length);
    gsl_matrix_set(M, 2, 1, gsl_matrix_get(M, 1, 2));
    gsl_matrix_set(M, 2, 0, gsl_matrix_get(M, 0, 2));

    // solve the equations
    int s = 0;
    gsl_permutation * P = gsl_permutation_alloc(3);
    gsl_linalg_LU_decomp(M, P, &s);
    
    gsl_linalg_LU_solve(M, P, B1, CX);
    gsl_linalg_LU_solve(M, P, B2, CY);

    // free vactors an matrices
    gsl_matrix_free(M);
    gsl_vector_free(B1);
    gsl_vector_free(B2);
    gsl_permutation_free(P);

    // write resuling vectors
    for (int i = 0; i < 3; ++i) {
        result[i] = gsl_vector_get(CX, i);
        result[i + 3] = gsl_vector_get(CY, i);
    }
    // free intermediate vectors
    gsl_vector_free(CX);
    gsl_vector_free(CY);
}

// -------------------------------------------------------------------
/** 
 * strip extention
 * @param txt extetntion text
 * @return text without extetion
 * @todo use prce's
 */
char *strip_ext(char *txt) {
	size_t len = strlen(txt);
	if (len==0) return(txt);
	for (int l=len; l>0; l--) {
		if (txt[l]=='.') {
			txt[l] = '\0';
			return(txt);
		}
	}
	return(txt);
}
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
void trfm_pix_geo(double *trfm,
                  double col, double row,
                  double *x , double *y) {
  *x = trfm[0] + trfm[1] * col + trfm[2] * row;
  *y = trfm[3] + trfm[4] * col + trfm[5] * row;
}


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
int trfm_geo_pix(double *trfm,
                double x, double y,
                long *col , long *row) {

  double div = (trfm[2]*trfm[4]-trfm[1]*trfm[5]);
  if (div < DBL_EPSILON * 2) return 0;
  double dcol = -(trfm[2]*(trfm[3]-y)+trfm[5]*x-trfm[0]*trfm[5])/div;
  double drow =  (trfm[1]*(trfm[3]-y)+trfm[4]*x-trfm[0]*trfm[4])/div;
  *col = round(dcol); *row = round(drow);
  return 1;
}

// ---------------------------------------------------------------
/**
 * initialize a dynamic double vector
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
 * add element to a dynamic double vector
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
 * free a dynamic double vector
 * @param vec the vector
 */
void dbl_vector_free(dbl_vector_t *vec) {
    free(vec->data);
    vec->data = NULL;
    vec->length = vec->mem_size = 0;
}

// ---------------------------------------------------------------
/**
 * initialize a dynamic integer vector
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
 * add a emlement to a dynamic int vector
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
 * free a dynamic int vector
 * @param vec the vector struct
 */
void int_vector_free(int_vector_t *vec) {
    free(vec->data);
    vec->data = NULL;
    vec->length = vec->mem_size = 0;
}


// --- EOF ---------------------------------------------------------
