/*
  Copyright (c) 2010 Toru Tamaki

  Permission is hereby granted, free of charge, to any person
  obtaining a copy of this software and associated documentation
  files (the "Software"), to deal in the Software without
  restriction, including without limitation the rights to use,
  copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following
  conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
  OTHER DEALINGS IN THE SOFTWARE.
*/

#include <cstdio>
#include <cstdlib>

#include "emicp/3dregistration.h"


void eigenvectorOfN(double* N, float* q);

void findRTfromS(const float* h_Xc,
		 const float* h_Yc,
		 const float* h_S,
		 float* h_R, float* h_t)
{

	#define h_Sxx h_S[0]
	#define h_Sxy h_S[1]
	#define h_Sxz h_S[2]
	#define h_Syx h_S[3]
	#define h_Syy h_S[4]
	#define h_Syz h_S[5]
	#define h_Szx h_S[6]
	#define h_Szy h_S[7]
	#define h_Szz h_S[8]

	#define h_Xcx h_Xc[0]
	#define h_Xcy h_Xc[1]
	#define h_Xcz h_Xc[2]
	#define h_Ycx h_Yc[0]
	#define h_Ycy h_Yc[1]
	#define h_Ycz h_Yc[2]


	double N[4*4]; for(int n=0;n<16;n++) N[n] = 0.0;
	float q[4];    for(int a=0;a<4;a++)  q[a] = 0.0f;

	N[ 0] = h_Sxx + h_Syy + h_Szz;
	N[ 1] = h_Syz - h_Szy;
	N[ 2] = h_Szx - h_Sxz;
	N[ 3] = h_Sxy - h_Syx;
	N[ 4] = h_Syz - h_Szy;
	N[ 5] = h_Sxx - h_Syy - h_Szz;
	N[ 6] = h_Sxy + h_Syx;
	N[ 7] = h_Szx + h_Sxz;
	N[ 8] = h_Szx - h_Sxz;
	N[ 9] = h_Sxy + h_Syx;
	N[10] = h_Syy - h_Sxx - h_Szz;
	N[11] = h_Syz + h_Szy;
	N[12] = h_Sxy - h_Syx;
	N[13] = h_Szx + h_Sxz;
	N[14] = h_Syz + h_Szy;
	N[15] = h_Szz - h_Sxx - h_Syy;

	// computer the eigenvector corresponding the largest eivenvalue
	eigenvectorOfN(N, q);


	float q0 = q[0], qx = q[1], qy = q[2], qz = q[3];

	// quaternion to rotation matrix
	h_R[0] = q0*q0 + qx*qx - qy*qy - qz*qz;
	h_R[1] = 2 * (qx*qy - q0*qz);
	h_R[2] = 2 * (qx*qz + q0*qy);
	h_R[3] = 2 * (qy*qx + q0*qz);
	h_R[4] = q0*q0 - qx*qx + qy*qy - qz*qz;
	h_R[5] = 2 * (qy*qz - q0*qx);
	h_R[6] = 2 * (qz*qx - q0*qy);
	h_R[7] = 2 * (qz*qy + q0*qx);
	h_R[8] = q0*q0 - qx*qx - qy*qy + qz*qz;

	// translation vector
	h_t[0] = h_Xcx - (h_R[0]*h_Ycx + h_R[1]*h_Ycy + h_R[2]*h_Ycz);
	h_t[1] = h_Xcy - (h_R[3]*h_Ycx + h_R[4]*h_Ycy + h_R[5]*h_Ycz);
	h_t[2] = h_Xcz - (h_R[6]*h_Ycx + h_R[7]*h_Ycy + h_R[8]*h_Ycz);
}



extern "C" {
int dsyev_(char *jobz, char *uplo, 
	   int *n, double *a, int *lda, 
	   double *w, double *work, int *lwork, 
	   int *info);
}

void eigenvectorOfN(double *N, float* q)
{
  
	static float q_pre[4]; // previous result

	int dimN = 4;
	double w[4]; // eigenvalues
	double *work = new double; // workspace
	int info;
	int lwork = -1;

	dsyev_((char*)"V", (char*)"U",
		&dimN, N, &dimN,
		w, work, &lwork, &info);	
	if(info != 0)
	{
		fprintf(stderr, "info = %d\n", info);
		exit(1);
	}
	
	lwork = (int)work[0];
	delete work;

	work = new double [lwork];

	dsyev_((char*)"V", (char*)"U",
		&dimN, N, &dimN,
		w, work, &lwork, &info);

	delete [] work;

	if(info != 0)
	{
		fprintf(stderr, "computing eigenvector FAIL! info = %d\n", info);
		//exit(1);

		// if fail, put back the previous result
		for(int i=0; i<4; i++)
		{
			q[i] = q_pre[i];
		}
	}
	else
	{
		// last column of N is the eigenvector of the largest eigenvalue 
		// and N is stored column-major
		for(int i=0; i<4; i++)
		{
			q[i] = N[4*3 + i];
			q_pre[i] = q[i];
		}
	}
}

/*
*  =========
*
*  JOBZ    (input) CHARACTER*1
*          = 'N':  Compute eigenvalues only;
*          = 'V':  Compute eigenvalues and eigenvectors.
*
*  UPLO    (input) CHARACTER*1
*          = 'U':  Upper triangle of A is stored;
*          = 'L':  Lower triangle of A is stored.
*
*  N       (input) INTEGER
*          The order of the matrix A.  N >= 0.
*
*  A       (input/output) DOUBLE PRECISION array, dimension (LDA, N)
*          On entry, the symmetric matrix A.  If UPLO = 'U', the
*          leading N-by-N upper triangular part of A contains the
*          upper triangular part of the matrix A.  If UPLO = 'L',
*          the leading N-by-N lower triangular part of A contains
*          the lower triangular part of the matrix A.
*          On exit, if JOBZ = 'V', then if INFO = 0, A contains the
*          orthonormal eigenvectors of the matrix A.
*          If JOBZ = 'N', then on exit the lower triangle (if UPLO='L')
*          or the upper triangle (if UPLO='U') of A, including the
*          diagonal, is destroyed.
*
*  LDA     (input) INTEGER
*          The leading dimension of the array A.  LDA >= max(1,N).
*
*  W       (output) DOUBLE PRECISION array, dimension (N)
*          If INFO = 0, the eigenvalues in ascending order.
*
*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK))
*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK.
*
*  LWORK   (input) INTEGER
*          The length of the array WORK.  LWORK >= max(1,3*N-1).
*          For optimal efficiency, LWORK >= (NB+2)*N,
*          where NB is the blocksize for DSYTRD returned by ILAENV.
*
*          If LWORK = -1, then a workspace query is assumed; the routine
*          only calculates the optimal size of the WORK array, returns
*          this value as the first entry of the WORK array, and no error
*          message related to LWORK is issued by XERBLA.
*
*  INFO    (output) INTEGER
*          = 0:  successful exit
*          < 0:  if INFO = -i, the i-th argument had an illegal value
*          > 0:  if INFO = i, the algorithm failed to converge; i
*                off-diagonal elements of an intermediate tridiagonal
*                form did not converge to zero.
*
*  =====================================================================
*/
