#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static doublereal c_b6 = 1.;
static doublereal c_b7 = 0.;
static integer c__1 = 1;
static doublereal c_b12 = -1.;

/* Subroutine */ int dsgt01_(integer *itype, char *uplo, integer *n, integer *
	m, doublereal *a, integer *lda, doublereal *b, integer *ldb, 
	doublereal *z__, integer *ldz, doublereal *d__, doublereal *work, 
	doublereal *result)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, z_dim1, z_offset, i__1;

    /* Local variables */
    integer i__;
    doublereal ulp;
    extern /* Subroutine */ int dscal_(integer *, doublereal *, doublereal *, 
	    integer *);
    doublereal anorm;
    extern /* Subroutine */ int dsymm_(char *, char *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, integer *);
    extern doublereal dlamch_(char *), dlange_(char *, integer *, 
	    integer *, doublereal *, integer *, doublereal *), 
	    dlansy_(char *, char *, integer *, doublereal *, integer *, 
	    doublereal *);


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

/*  DDGT01 checks a decomposition of the form */

/*     A Z   =  B Z D or */
/*     A B Z =  Z D or */
/*     B A Z =  Z D */

/*  where A is a symmetric matrix, B is */
/*  symmetric positive definite, Z is orthogonal, and D is diagonal. */

/*  One of the following test ratios is computed: */

/*  ITYPE = 1:  RESULT(1) = | A Z - B Z D | / ( |A| |Z| n ulp ) */

/*  ITYPE = 2:  RESULT(1) = | A B Z - Z D | / ( |A| |Z| n ulp ) */

/*  ITYPE = 3:  RESULT(1) = | B A Z - Z D | / ( |A| |Z| n ulp ) */

/*  Arguments */
/*  ========= */

/*  ITYPE   (input) INTEGER */
/*          The form of the symmetric generalized eigenproblem. */
/*          = 1:  A*z = (lambda)*B*z */
/*          = 2:  A*B*z = (lambda)*z */
/*          = 3:  B*A*z = (lambda)*z */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the upper or lower triangular part of the */
/*          symmetric matrices A and B is stored. */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  M       (input) INTEGER */
/*          The number of eigenvalues found.  0 <= M <= N. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA, N) */
/*          The original symmetric matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  B       (input) DOUBLE PRECISION array, dimension (LDB, N) */
/*          The original symmetric positive definite matrix B. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  Z       (input) DOUBLE PRECISION array, dimension (LDZ, M) */
/*          The computed eigenvectors of the generalized eigenproblem. */

/*  LDZ     (input) INTEGER */
/*          The leading dimension of the array Z.  LDZ >= max(1,N). */

/*  D       (input) DOUBLE PRECISION array, dimension (M) */
/*          The computed eigenvalues of the generalized eigenproblem. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (N*N) */

/*  RESULT  (output) DOUBLE PRECISION array, dimension (1) */
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
    --result;

    /* Function Body */
    result[1] = 0.;
    if (*n <= 0) {
	return 0;
    }

    ulp = dlamch_("Epsilon");

/*     Compute product of 1-norms of A and Z. */

    anorm = dlansy_("1", uplo, n, &a[a_offset], lda, &work[1]) * dlange_("1", n, m, &z__[z_offset], ldz, &work[1]);
    if (anorm == 0.) {
	anorm = 1.;
    }

    if (*itype == 1) {

/*        Norm of AZ - BZD */

	dsymm_("Left", uplo, n, m, &c_b6, &a[a_offset], lda, &z__[z_offset], 
		ldz, &c_b7, &work[1], n);
	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    dscal_(n, &d__[i__], &z__[i__ * z_dim1 + 1], &c__1);
/* L10: */
	}
	dsymm_("Left", uplo, n, m, &c_b6, &b[b_offset], ldb, &z__[z_offset], 
		ldz, &c_b12, &work[1], n);

	result[1] = dlange_("1", n, m, &work[1], n, &work[1]) / 
		anorm / (*n * ulp);

    } else if (*itype == 2) {

/*        Norm of ABZ - ZD */

	dsymm_("Left", uplo, n, m, &c_b6, &b[b_offset], ldb, &z__[z_offset], 
		ldz, &c_b7, &work[1], n);
	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    dscal_(n, &d__[i__], &z__[i__ * z_dim1 + 1], &c__1);
/* L20: */
	}
	dsymm_("Left", uplo, n, m, &c_b6, &a[a_offset], lda, &work[1], n, &
		c_b12, &z__[z_offset], ldz);

	result[1] = dlange_("1", n, m, &z__[z_offset], ldz, &work[1]) / anorm / (*n * ulp);

    } else if (*itype == 3) {

/*        Norm of BAZ - ZD */

	dsymm_("Left", uplo, n, m, &c_b6, &a[a_offset], lda, &z__[z_offset], 
		ldz, &c_b7, &work[1], n);
	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    dscal_(n, &d__[i__], &z__[i__ * z_dim1 + 1], &c__1);
/* L30: */
	}
	dsymm_("Left", uplo, n, m, &c_b6, &b[b_offset], ldb, &work[1], n, &
		c_b12, &z__[z_offset], ldz);

	result[1] = dlange_("1", n, m, &z__[z_offset], ldz, &work[1]) / anorm / (*n * ulp);
    }

    return 0;

/*     End of DDGT01 */

} /* dsgt01_ */
