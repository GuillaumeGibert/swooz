// $mchol.hpp 3.0 milbo$ mchol.h

#if !defined(mchol_hpp)
#define mchol_hpp

// namespace swStasm
// {
	int gsl_linalg_cholesky_decomp_aux (gsl_matrix * A, int return_on_EDOM);
	int gsl_linalg_cholesky_decomp1(gsl_matrix * A);
	int gsl_linalg_cholesky_decomp_return_on_EDOM(gsl_matrix * A);
// }

#endif // mchol_hpp
