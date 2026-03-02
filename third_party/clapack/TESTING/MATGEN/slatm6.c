#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;
static integer c__4 = 4;
static integer c__12 = 12;
static integer c__8 = 8;
static integer c__40 = 40;
static integer c__2 = 2;
static integer c__3 = 3;
static integer c__60 = 60;

/* Subroutine */ int slatm6_(integer *type__, integer *n, real *a, integer *
	lda, real *b, real *x, integer *ldx, real *y, integer *ldy, real *
	alpha, real *beta, real *wx, real *wy, real *s, real *dif)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, x_dim1, x_offset, y_dim1, 
	    y_offset, i__1, i__2;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    integer i__, j;
    real z__[144]	/* was [12][12] */;
    integer info;
    real work[100];
    extern /* Subroutine */ int slakf2_(integer *, integer *, real *, integer 
	    *, real *, real *, real *, real *, integer *), sgesvd_(char *, 
	    char *, integer *, integer *, real *, integer *, real *, real *, 
	    integer *, real *, integer *, real *, integer *, integer *), slacpy_(char *, integer *, integer *, real *, 
	    integer *, real *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SLATM6 generates test matrices for the generalized eigenvalue */
/*  problem, their corresponding right and left eigenvector matrices, */
/*  and also reciprocal condition numbers for all eigenvalues and */
/*  the reciprocal condition numbers of eigenvectors corresponding to */
/*  the 1th and 5th eigenvalues. */

/*  Test Matrices */
/*  ============= */

/*  Two kinds of test matrix pairs */

/*        (A, B) = inverse(YH) * (Da, Db) * inverse(X) */

/*  are used in the tests: */

/*  Type 1: */
/*     Da = 1+a   0    0    0    0    Db = 1   0   0   0   0 */
/*           0   2+a   0    0    0         0   1   0   0   0 */
/*           0    0   3+a   0    0         0   0   1   0   0 */
/*           0    0    0   4+a   0         0   0   0   1   0 */
/*           0    0    0    0   5+a ,      0   0   0   0   1 , and */

/*  Type 2: */
/*     Da =  1   -1    0    0    0    Db = 1   0   0   0   0 */
/*           1    1    0    0    0         0   1   0   0   0 */
/*           0    0    1    0    0         0   0   1   0   0 */
/*           0    0    0   1+a  1+b        0   0   0   1   0 */
/*           0    0    0  -1-b  1+a ,      0   0   0   0   1 . */

/*  In both cases the same inverse(YH) and inverse(X) are used to compute */
/*  (A, B), giving the exact eigenvectors to (A,B) as (YH, X): */

/*  YH:  =  1    0   -y    y   -y    X =  1   0  -x  -x   x */
/*          0    1   -y    y   -y         0   1   x  -x  -x */
/*          0    0    1    0    0         0   0   1   0   0 */
/*          0    0    0    1    0         0   0   0   1   0 */
/*          0    0    0    0    1,        0   0   0   0   1 , */

/* where a, b, x and y will have all values independently of each other. */

/*  Arguments */
/*  ========= */

/*  TYPE    (input) INTEGER */
/*          Specifies the problem type (see futher details). */

/*  N       (input) INTEGER */
/*          Size of the matrices A and B. */

/*  A       (output) REAL array, dimension (LDA, N). */
/*          On exit A N-by-N is initialized according to TYPE. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A and of B. */

/*  B       (output) REAL array, dimension (LDA, N). */
/*          On exit B N-by-N is initialized according to TYPE. */

/*  X       (output) REAL array, dimension (LDX, N). */
/*          On exit X is the N-by-N matrix of right eigenvectors. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of X. */

/*  Y       (output) REAL array, dimension (LDY, N). */
/*          On exit Y is the N-by-N matrix of left eigenvectors. */

/*  LDY     (input) INTEGER */
/*          The leading dimension of Y. */

/*  ALPHA   (input) REAL */
/*  BETA    (input) REAL */
/*          Weighting constants for matrix A. */

/*  WX      (input) REAL */
/*          Constant for right eigenvector matrix. */

/*  WY      (input) REAL */
/*          Constant for left eigenvector matrix. */

/*  S       (output) REAL array, dimension (N) */
/*          S(i) is the reciprocal condition number for eigenvalue i. */

/*  DIF     (output) REAL array, dimension (N) */
/*          DIF(i) is the reciprocal condition number for eigenvector i. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Local Arrays .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Generate test problem ... */
/*     (Da, Db) ... */

    /* Parameter adjustments */
    b_dim1 = *lda;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    x_dim1 = *ldx;
    x_offset = 1 + x_dim1;
    x -= x_offset;
    y_dim1 = *ldy;
    y_offset = 1 + y_dim1;
    y -= y_offset;
    --s;
    --dif;

    /* Function Body */
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = *n;
	for (j = 1; j <= i__2; ++j) {

	    if (i__ == j) {
		a[i__ + i__ * a_dim1] = (real) i__ + *alpha;
		b[i__ + i__ * b_dim1] = 1.f;
	    } else {
		a[i__ + j * a_dim1] = 0.f;
		b[i__ + j * b_dim1] = 0.f;
	    }

/* L10: */
	}
/* L20: */
    }

/*     Form X and Y */

    slacpy_("F", n, n, &b[b_offset], lda, &y[y_offset], ldy);
    y[y_dim1 + 3] = -(*wy);
    y[y_dim1 + 4] = *wy;
    y[y_dim1 + 5] = -(*wy);
    y[(y_dim1 << 1) + 3] = -(*wy);
    y[(y_dim1 << 1) + 4] = *wy;
    y[(y_dim1 << 1) + 5] = -(*wy);

    slacpy_("F", n, n, &b[b_offset], lda, &x[x_offset], ldx);
    x[x_dim1 * 3 + 1] = -(*wx);
    x[(x_dim1 << 2) + 1] = -(*wx);
    x[x_dim1 * 5 + 1] = *wx;
    x[x_dim1 * 3 + 2] = *wx;
    x[(x_dim1 << 2) + 2] = -(*wx);
    x[x_dim1 * 5 + 2] = -(*wx);

/*     Form (A, B) */

    b[b_dim1 * 3 + 1] = *wx + *wy;
    b[b_dim1 * 3 + 2] = -(*wx) + *wy;
    b[(b_dim1 << 2) + 1] = *wx - *wy;
    b[(b_dim1 << 2) + 2] = *wx - *wy;
    b[b_dim1 * 5 + 1] = -(*wx) + *wy;
    b[b_dim1 * 5 + 2] = *wx + *wy;
    if (*type__ == 1) {
	a[a_dim1 * 3 + 1] = *wx * a[a_dim1 + 1] + *wy * a[a_dim1 * 3 + 3];
	a[a_dim1 * 3 + 2] = -(*wx) * a[(a_dim1 << 1) + 2] + *wy * a[a_dim1 * 
		3 + 3];
	a[(a_dim1 << 2) + 1] = *wx * a[a_dim1 + 1] - *wy * a[(a_dim1 << 2) + 
		4];
	a[(a_dim1 << 2) + 2] = *wx * a[(a_dim1 << 1) + 2] - *wy * a[(a_dim1 <<
		 2) + 4];
	a[a_dim1 * 5 + 1] = -(*wx) * a[a_dim1 + 1] + *wy * a[a_dim1 * 5 + 5];
	a[a_dim1 * 5 + 2] = *wx * a[(a_dim1 << 1) + 2] + *wy * a[a_dim1 * 5 + 
		5];
    } else if (*type__ == 2) {
	a[a_dim1 * 3 + 1] = *wx * 2.f + *wy;
	a[a_dim1 * 3 + 2] = *wy;
	a[(a_dim1 << 2) + 1] = -(*wy) * (*alpha + 2.f + *beta);
	a[(a_dim1 << 2) + 2] = *wx * 2.f - *wy * (*alpha + 2.f + *beta);
	a[a_dim1 * 5 + 1] = *wx * -2.f + *wy * (*alpha - *beta);
	a[a_dim1 * 5 + 2] = *wy * (*alpha - *beta);
	a[a_dim1 + 1] = 1.f;
	a[(a_dim1 << 1) + 1] = -1.f;
	a[a_dim1 + 2] = 1.f;
	a[(a_dim1 << 1) + 2] = a[a_dim1 + 1];
	a[a_dim1 * 3 + 3] = 1.f;
	a[(a_dim1 << 2) + 4] = *alpha + 1.f;
	a[a_dim1 * 5 + 4] = *beta + 1.f;
	a[(a_dim1 << 2) + 5] = -a[a_dim1 * 5 + 4];
	a[a_dim1 * 5 + 5] = a[(a_dim1 << 2) + 4];
    }

/*     Compute condition numbers */

    if (*type__ == 1) {

	s[1] = 1.f / sqrt((*wy * 3.f * *wy + 1.f) / (a[a_dim1 + 1] * a[a_dim1 
		+ 1] + 1.f));
	s[2] = 1.f / sqrt((*wy * 3.f * *wy + 1.f) / (a[(a_dim1 << 1) + 2] * a[
		(a_dim1 << 1) + 2] + 1.f));
	s[3] = 1.f / sqrt((*wx * 2.f * *wx + 1.f) / (a[a_dim1 * 3 + 3] * a[
		a_dim1 * 3 + 3] + 1.f));
	s[4] = 1.f / sqrt((*wx * 2.f * *wx + 1.f) / (a[(a_dim1 << 2) + 4] * a[
		(a_dim1 << 2) + 4] + 1.f));
	s[5] = 1.f / sqrt((*wx * 2.f * *wx + 1.f) / (a[a_dim1 * 5 + 5] * a[
		a_dim1 * 5 + 5] + 1.f));

	slakf2_(&c__1, &c__4, &a[a_offset], lda, &a[(a_dim1 << 1) + 2], &b[
		b_offset], &b[(b_dim1 << 1) + 2], z__, &c__12);
	sgesvd_("N", "N", &c__8, &c__8, z__, &c__12, work, &work[8], &c__1, &
		work[9], &c__1, &work[10], &c__40, &info);
	dif[1] = work[7];

	slakf2_(&c__4, &c__1, &a[a_offset], lda, &a[a_dim1 * 5 + 5], &b[
		b_offset], &b[b_dim1 * 5 + 5], z__, &c__12);
	sgesvd_("N", "N", &c__8, &c__8, z__, &c__12, work, &work[8], &c__1, &
		work[9], &c__1, &work[10], &c__40, &info);
	dif[5] = work[7];

    } else if (*type__ == 2) {

	s[1] = 1.f / sqrt(*wy * *wy + .33333333333333331f);
	s[2] = s[1];
	s[3] = 1.f / sqrt(*wx * *wx + .5f);
	s[4] = 1.f / sqrt((*wx * 2.f * *wx + 1.f) / ((*alpha + 1.f) * (*alpha 
		+ 1.f) + 1.f + (*beta + 1.f) * (*beta + 1.f)));
	s[5] = s[4];

	slakf2_(&c__2, &c__3, &a[a_offset], lda, &a[a_dim1 * 3 + 3], &b[
		b_offset], &b[b_dim1 * 3 + 3], z__, &c__12);
	sgesvd_("N", "N", &c__12, &c__12, z__, &c__12, work, &work[12], &c__1, 
		 &work[13], &c__1, &work[14], &c__60, &info);
	dif[1] = work[11];

	slakf2_(&c__3, &c__2, &a[a_offset], lda, &a[(a_dim1 << 2) + 4], &b[
		b_offset], &b[(b_dim1 << 2) + 4], z__, &c__12);
	sgesvd_("N", "N", &c__12, &c__12, z__, &c__12, work, &work[12], &c__1, 
		 &work[13], &c__1, &work[14], &c__60, &info);
	dif[5] = work[11];

    }

    return 0;

/*     End of SLATM6 */

} /* slatm6_ */
