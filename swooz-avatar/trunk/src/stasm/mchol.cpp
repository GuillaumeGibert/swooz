// $mchol.cpp 3.0 milbo$ lifted and modified from gsl1_6/cholesky.c
//
// gsl_linalg_cholesky_decomp1(gsl_matrix * A) replaces
// the GLS routine gsl_linalg_cholesky_decomp1(gsl_matrix * A)
// I added the return_on_EDOM parameter, which allows the func to
// be used as a test for pos definiteness
//
// Original credits:
//      Copyright (C) 2000  Thomas Walter
//      3 May 2000: Modified for GSL by Brian Gough
//      Standard GNU General Public License header
//
// milbo apr06 petaluma

#include "stasm.hpp"
// using namespace swStasm;

//-----------------------------------------------------------------------------
// if return_on_EDOM is set i.e. non-zero then return EDOM on error (instead of
// calling GSL_ERROR) so we can use this func to test for positive definiteness

int gsl_linalg_cholesky_decomp_aux (gsl_matrix * A, int return_on_EDOM)
{
const size_t M = A->size1;
const size_t N = A->size2;

if (M != N)
    {
    if (return_on_EDOM)
        return GSL_EDOM;
    else
        GSL_ERROR("cholesky decomposition requires square matrix", GSL_ENOTSQR);
    }
else
    {
    size_t i,j,k;
    int status = 0;

    // Do the first 2 rows explicitly.  It is simple, and faster.
    // And one can return if the matrix has only 1 or 2 rows.

    double A_00 = gsl_matrix_get (A, 0, 0);
    double L_00;

    if (A_00 <= 0)
        {
        if (return_on_EDOM)
            return GSL_EDOM;
        status = GSL_EDOM;
        }
    L_00 = sqrt(A_00);

    gsl_matrix_set (A, 0, 0, L_00);

    if (M > 1)
        {
        double A_10 = gsl_matrix_get (A, 1, 0);
        double A_11 = gsl_matrix_get (A, 1, 1);

        double L_10 = A_10 / L_00;
        double diag = A_11 - L_10 * L_10;
        double L_11;

        if (diag <= 0)
            {
            // matrix is non positive definite
            if (return_on_EDOM)
                return GSL_EDOM;
            status = GSL_EDOM;
            }
        L_11 = sqrt(diag);

        gsl_matrix_set (A, 1, 0, L_10);
        gsl_matrix_set (A, 1, 1, L_11);
        }
    for (k = 2; k < M; k++)
        {
        double A_kk = gsl_matrix_get (A, k, k);

        for (i = 0; i < k; i++)
            {
            double sum = 0;

            double A_ki = gsl_matrix_get (A, k, i);
            double A_ii = gsl_matrix_get (A, i, i);

            gsl_vector_view ci = gsl_matrix_row (A, i);
            gsl_vector_view ck = gsl_matrix_row (A, k);

            if (i > 0)
                {
                gsl_vector_view di = gsl_vector_subvector(&ci.vector, 0, i);
                gsl_vector_view dk = gsl_vector_subvector(&ck.vector, 0, i);

                gsl_blas_ddot (&di.vector, &dk.vector, &sum);
                }
            A_ki = (A_ki - sum) / A_ii;
            gsl_matrix_set (A, k, i, A_ki);
            }
        gsl_vector_view ck = gsl_matrix_row (A, k);
        gsl_vector_view dk = gsl_vector_subvector (&ck.vector, 0, k);

        double sum = gsl_blas_dnrm2 (&dk.vector);
        double diag = A_kk - sum * sum;
        double L_kk;
        if (diag <= 0)
            {
            // matrix is non positive definite
            if (return_on_EDOM)
                return GSL_EDOM;
            status = GSL_EDOM;
            }
        L_kk = sqrt(diag);
        gsl_matrix_set (A, k, k, L_kk);
        }
    // Now copy the transposed lower triangle to the upper
    // triangle, the diagonal is common.

    for (i = 1; i < M; i++)
        for (j = 0; j < i; j++)
            {
            double A_ij = gsl_matrix_get (A, i, j);
            gsl_matrix_set (A, j, i, A_ij);
            }
    if (status == GSL_EDOM)
        GSL_ERROR ("matrix must be positive definite", GSL_EDOM);

    return GSL_SUCCESS;
    }
}


//-----------------------------------------------------------------------------
int gsl_linalg_cholesky_decomp1(gsl_matrix * A)
{
return gsl_linalg_cholesky_decomp_aux(A, 0);
}


//-----------------------------------------------------------------------------
int gsl_linalg_cholesky_decomp_return_on_EDOM(gsl_matrix * A)
{
return gsl_linalg_cholesky_decomp_aux(A, 1);
}
