#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static complex c_b1 = {0.f,0.f};
static complex c_b2 = {1.f,0.f};
static integer c__1 = 1;

/* Subroutine */ int csgt01_(integer *itype, char *uplo, integer *n, integer *
	m, complex *a, integer *lda, complex *b, integer *ldb, complex *z__, 
	integer *ldz, real *d__, complex *work, real *rwork, real *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, z_dim1, z_offset, i__1;
    complex q__1;

    /* Local variables */
    integer i__;
    real ulp;
    extern /* Subroutine */ int chemm_(char *, char *, integer *, integer *, 
	    complex *, complex *, integer *, complex *, integer *, complex *, 
	    complex *, integer *);
    real anorm;
    extern doublereal clange_(char *, integer *, integer *, complex *, 
	    integer *, real *), clanhe_(char *, char *, integer *, 
	    complex *, integer *, real *), slamch_(char *);
    extern /* Subroutine */ int csscal_(integer *, real *, complex *, integer 
	    *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     modified August 1997, a new parameter M is added to the calling */
/*     sequence. */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CSGT01 checks a decomposition of the form */

/*     A Z   =  B Z D or */
/*     A B Z =  Z D or */
/*     B A Z =  Z D */

/*  where A is a Hermitian matrix, B is Hermitian positive definite, */
/*  Z is unitary, and D is diagonal. */

/*  One of the following test ratios is computed: */

/*  ITYPE = 1:  RESULT(1) = | A Z - B Z D | / ( |A| |Z| n ulp ) */

/*  ITYPE = 2:  RESULT(1) = | A B Z - Z D | / ( |A| |Z| n ulp ) */

/*  ITYPE = 3:  RESULT(1) = | B A Z - Z D | / ( |A| |Z| n ulp ) */

/*  Arguments */
/*  ========= */

/*  ITYPE   (input) INTEGER */
/*          The form of the Hermitian generalized eigenproblem. */
/*          = 1:  A*z = (lambda)*B*z */
/*          = 2:  A*B*z = (lambda)*z */
/*          = 3:  B*A*z = (lambda)*z */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the upper or lower triangular part of the */
/*          Hermitian matrices A and B is stored. */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  M       (input) INTEGER */
/*          The number of eigenvalues found.  M >= 0. */

/*  A       (input) COMPLEX array, dimension (LDA, N) */
/*          The original Hermitian matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  B       (input) COMPLEX array, dimension (LDB, N) */
/*          The original Hermitian positive definite matrix B. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  Z       (input) COMPLEX array, dimension (LDZ, M) */
/*          The computed eigenvectors of the generalized eigenproblem. */

/*  LDZ     (input) INTEGER */
/*          The leading dimension of the array Z.  LDZ >= max(1,N). */

/*  D       (input) REAL array, dimension (M) */
/*          The computed eigenvalues of the generalized eigenproblem. */

/*  WORK    (workspace) COMPLEX array, dimension (N*N) */

/*  RWORK   (workspace) REAL array, dimension (N) */

/*  RESULT  (output) REAL array, dimension (1) */
/*          The test ratio as described above. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --d__;
    --work;
    --rwork;
    --result;

    /* Function Body */
    result[1] = 0.f;
    if (*n <= 0) {
	return 0;
    }

    ulp = slamch_("Epsilon");

/*     Compute product of 1-norms of A and Z. */

    anorm = clanhe_("1", uplo, n, &a[a_offset], lda, &rwork[1]) * clange_("1", n, m, &z__[z_offset], ldz, &rwork[1]);
    if (anorm == 0.f) {
	anorm = 1.f;
    }

    if (*itype == 1) {

/*        Norm of AZ - BZD */

	chemm_("Left", uplo, n, m, &c_b2, &a[a_offset], lda, &z__[z_offset], 
		ldz, &c_b1, &work[1], n);
	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    csscal_(n, &d__[i__], &z__[i__ * z_dim1 + 1], &c__1);
/* L10: */
	}
	q__1.r = -1.f, q__1.i = -0.f;
	chemm_("Left", uplo, n, m, &c_b2, &b[b_offset], ldb, &z__[z_offset], 
		ldz, &q__1, &work[1], n);

	result[1] = clange_("1", n, m, &work[1], n, &rwork[1]) / 
		anorm / (*n * ulp);

    } else if (*itype == 2) {

/*        Norm of ABZ - ZD */

	chemm_("Left", uplo, n, m, &c_b2, &b[b_offset], ldb, &z__[z_offset], 
		ldz, &c_b1, &work[1], n);
	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    csscal_(n, &d__[i__], &z__[i__ * z_dim1 + 1], &c__1);
/* L20: */
	}
	q__1.r = -1.f, q__1.i = -0.f;
	chemm_("Left", uplo, n, m, &c_b2, &a[a_offset], lda, &work[1], n, &
		q__1, &z__[z_offset], ldz);

	result[1] = clange_("1", n, m, &z__[z_offset], ldz, &rwork[1]) / anorm / (*n * ulp);

    } else if (*itype == 3) {

/*        Norm of BAZ - ZD */

	chemm_("Left", uplo, n, m, &c_b2, &a[a_offset], lda, &z__[z_offset], 
		ldz, &c_b1, &work[1], n);
	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    csscal_(n, &d__[i__], &z__[i__ * z_dim1 + 1], &c__1);
/* L30: */
	}
	q__1.r = -1.f, q__1.i = -0.f;
	chemm_("Left", uplo, n, m, &c_b2, &b[b_offset], ldb, &work[1], n, &
		q__1, &z__[z_offset], ldz);

	result[1] = clange_("1", n, m, &z__[z_offset], ldz, &rwork[1]) / anorm / (*n * ulp);
    }

    return 0;

/*     End of CSGT01 */

} /* csgt01_ */
