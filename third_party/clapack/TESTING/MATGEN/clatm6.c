#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;
static integer c__4 = 4;
static integer c__8 = 8;
static integer c__24 = 24;

/* Subroutine */ int clatm6_(integer *type__, integer *n, complex *a, integer 
	*lda, complex *b, complex *x, integer *ldx, complex *y, integer *ldy, 
	complex *alpha, complex *beta, complex *wx, complex *wy, real *s, 
	real *dif)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, x_dim1, x_offset, y_dim1, 
	    y_offset, i__1, i__2, i__3;
    real r__1, r__2;
    complex q__1, q__2, q__3, q__4;

    /* Builtin functions */
    void r_cnjg(complex *, complex *);
    double c_abs(complex *), sqrt(doublereal);

    /* Local variables */
    integer i__, j;
    complex z__[64]	/* was [8][8] */;
    integer info;
    complex work[26];
    extern /* Subroutine */ int clakf2_(integer *, integer *, complex *, 
	    integer *, complex *, complex *, complex *, complex *, integer *);
    real rwork[50];
    extern /* Subroutine */ int cgesvd_(char *, char *, integer *, integer *, 
	    complex *, integer *, real *, complex *, integer *, complex *, 
	    integer *, complex *, integer *, real *, integer *), clacpy_(char *, integer *, integer *, complex *, integer 
	    *, complex *, integer *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CLATM6 generates test matrices for the generalized eigenvalue */
/*  problem, their corresponding right and left eigenvector matrices, */
/*  and also reciprocal condition numbers for all eigenvalues and */
/*  the reciprocal condition numbers of eigenvectors corresponding to */
/*  the 1th and 5th eigenvalues. */

/*  Test Matrices */
/*  ============= */

/*  Two kinds of test matrix pairs */
/*           (A, B) = inverse(YH) * (Da, Db) * inverse(X) */
/*  are used in the tests: */

/*  Type 1: */
/*     Da = 1+a   0    0    0    0    Db = 1   0   0   0   0 */
/*           0   2+a   0    0    0         0   1   0   0   0 */
/*           0    0   3+a   0    0         0   0   1   0   0 */
/*           0    0    0   4+a   0         0   0   0   1   0 */
/*           0    0    0    0   5+a ,      0   0   0   0   1 */
/*  and Type 2: */
/*     Da = 1+i   0    0       0       0    Db = 1   0   0   0   0 */
/*           0   1-i   0       0       0         0   1   0   0   0 */
/*           0    0    1       0       0         0   0   1   0   0 */
/*           0    0    0 (1+a)+(1+b)i  0         0   0   0   1   0 */
/*           0    0    0       0 (1+a)-(1+b)i,   0   0   0   0   1 . */

/*  In both cases the same inverse(YH) and inverse(X) are used to compute */
/*  (A, B), giving the exact eigenvectors to (A,B) as (YH, X): */

/*  YH:  =  1    0   -y    y   -y    X =  1   0  -x  -x   x */
/*          0    1   -y    y   -y         0   1   x  -x  -x */
/*          0    0    1    0    0         0   0   1   0   0 */
/*          0    0    0    1    0         0   0   0   1   0 */
/*          0    0    0    0    1,        0   0   0   0   1 , where */

/*  a, b, x and y will have all values independently of each other. */

/*  Arguments */
/*  ========= */

/*  TYPE    (input) INTEGER */
/*          Specifies the problem type (see futher details). */

/*  N       (input) INTEGER */
/*          Size of the matrices A and B. */

/*  A       (output) COMPLEX array, dimension (LDA, N). */
/*          On exit A N-by-N is initialized according to TYPE. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A and of B. */

/*  B       (output) COMPLEX array, dimension (LDA, N). */
/*          On exit B N-by-N is initialized according to TYPE. */

/*  X       (output) COMPLEX array, dimension (LDX, N). */
/*          On exit X is the N-by-N matrix of right eigenvectors. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of X. */

/*  Y       (output) COMPLEX array, dimension (LDY, N). */
/*          On exit Y is the N-by-N matrix of left eigenvectors. */

/*  LDY     (input) INTEGER */
/*          The leading dimension of Y. */

/*  ALPHA   (input) COMPLEX */
/*  BETA    (input) COMPLEX */
/*          Weighting constants for matrix A. */

/*  WX      (input) COMPLEX */
/*          Constant for right eigenvector matrix. */

/*  WY      (input) COMPLEX */
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
		i__3 = i__ + i__ * a_dim1;
		q__2.r = (real) i__, q__2.i = 0.f;
		q__1.r = q__2.r + alpha->r, q__1.i = q__2.i + alpha->i;
		a[i__3].r = q__1.r, a[i__3].i = q__1.i;
		i__3 = i__ + i__ * b_dim1;
		b[i__3].r = 1.f, b[i__3].i = 0.f;
	    } else {
		i__3 = i__ + j * a_dim1;
		a[i__3].r = 0.f, a[i__3].i = 0.f;
		i__3 = i__ + j * b_dim1;
		b[i__3].r = 0.f, b[i__3].i = 0.f;
	    }

/* L10: */
	}
/* L20: */
    }
    if (*type__ == 2) {
	i__1 = a_dim1 + 1;
	a[i__1].r = 1.f, a[i__1].i = 1.f;
	i__1 = (a_dim1 << 1) + 2;
	r_cnjg(&q__1, &a[a_dim1 + 1]);
	a[i__1].r = q__1.r, a[i__1].i = q__1.i;
	i__1 = a_dim1 * 3 + 3;
	a[i__1].r = 1.f, a[i__1].i = 0.f;
	i__1 = (a_dim1 << 2) + 4;
	q__2.r = alpha->r + 1.f, q__2.i = alpha->i + 0.f;
	r__1 = q__2.r;
	q__3.r = beta->r + 1.f, q__3.i = beta->i + 0.f;
	r__2 = q__3.r;
	q__1.r = r__1, q__1.i = r__2;
	a[i__1].r = q__1.r, a[i__1].i = q__1.i;
	i__1 = a_dim1 * 5 + 5;
	r_cnjg(&q__1, &a[(a_dim1 << 2) + 4]);
	a[i__1].r = q__1.r, a[i__1].i = q__1.i;
    }

/*     Form X and Y */

    clacpy_("F", n, n, &b[b_offset], lda, &y[y_offset], ldy);
    i__1 = y_dim1 + 3;
    r_cnjg(&q__2, wy);
    q__1.r = -q__2.r, q__1.i = -q__2.i;
    y[i__1].r = q__1.r, y[i__1].i = q__1.i;
    i__1 = y_dim1 + 4;
    r_cnjg(&q__1, wy);
    y[i__1].r = q__1.r, y[i__1].i = q__1.i;
    i__1 = y_dim1 + 5;
    r_cnjg(&q__2, wy);
    q__1.r = -q__2.r, q__1.i = -q__2.i;
    y[i__1].r = q__1.r, y[i__1].i = q__1.i;
    i__1 = (y_dim1 << 1) + 3;
    r_cnjg(&q__2, wy);
    q__1.r = -q__2.r, q__1.i = -q__2.i;
    y[i__1].r = q__1.r, y[i__1].i = q__1.i;
    i__1 = (y_dim1 << 1) + 4;
    r_cnjg(&q__1, wy);
    y[i__1].r = q__1.r, y[i__1].i = q__1.i;
    i__1 = (y_dim1 << 1) + 5;
    r_cnjg(&q__2, wy);
    q__1.r = -q__2.r, q__1.i = -q__2.i;
    y[i__1].r = q__1.r, y[i__1].i = q__1.i;

    clacpy_("F", n, n, &b[b_offset], lda, &x[x_offset], ldx);
    i__1 = x_dim1 * 3 + 1;
    q__1.r = -wx->r, q__1.i = -wx->i;
    x[i__1].r = q__1.r, x[i__1].i = q__1.i;
    i__1 = (x_dim1 << 2) + 1;
    q__1.r = -wx->r, q__1.i = -wx->i;
    x[i__1].r = q__1.r, x[i__1].i = q__1.i;
    i__1 = x_dim1 * 5 + 1;
    x[i__1].r = wx->r, x[i__1].i = wx->i;
    i__1 = x_dim1 * 3 + 2;
    x[i__1].r = wx->r, x[i__1].i = wx->i;
    i__1 = (x_dim1 << 2) + 2;
    q__1.r = -wx->r, q__1.i = -wx->i;
    x[i__1].r = q__1.r, x[i__1].i = q__1.i;
    i__1 = x_dim1 * 5 + 2;
    q__1.r = -wx->r, q__1.i = -wx->i;
    x[i__1].r = q__1.r, x[i__1].i = q__1.i;

/*     Form (A, B) */

    i__1 = b_dim1 * 3 + 1;
    q__1.r = wx->r + wy->r, q__1.i = wx->i + wy->i;
    b[i__1].r = q__1.r, b[i__1].i = q__1.i;
    i__1 = b_dim1 * 3 + 2;
    q__2.r = -wx->r, q__2.i = -wx->i;
    q__1.r = q__2.r + wy->r, q__1.i = q__2.i + wy->i;
    b[i__1].r = q__1.r, b[i__1].i = q__1.i;
    i__1 = (b_dim1 << 2) + 1;
    q__1.r = wx->r - wy->r, q__1.i = wx->i - wy->i;
    b[i__1].r = q__1.r, b[i__1].i = q__1.i;
    i__1 = (b_dim1 << 2) + 2;
    q__1.r = wx->r - wy->r, q__1.i = wx->i - wy->i;
    b[i__1].r = q__1.r, b[i__1].i = q__1.i;
    i__1 = b_dim1 * 5 + 1;
    q__2.r = -wx->r, q__2.i = -wx->i;
    q__1.r = q__2.r + wy->r, q__1.i = q__2.i + wy->i;
    b[i__1].r = q__1.r, b[i__1].i = q__1.i;
    i__1 = b_dim1 * 5 + 2;
    q__1.r = wx->r + wy->r, q__1.i = wx->i + wy->i;
    b[i__1].r = q__1.r, b[i__1].i = q__1.i;
    i__1 = a_dim1 * 3 + 1;
    i__2 = a_dim1 + 1;
    q__2.r = wx->r * a[i__2].r - wx->i * a[i__2].i, q__2.i = wx->r * a[i__2]
	    .i + wx->i * a[i__2].r;
    i__3 = a_dim1 * 3 + 3;
    q__3.r = wy->r * a[i__3].r - wy->i * a[i__3].i, q__3.i = wy->r * a[i__3]
	    .i + wy->i * a[i__3].r;
    q__1.r = q__2.r + q__3.r, q__1.i = q__2.i + q__3.i;
    a[i__1].r = q__1.r, a[i__1].i = q__1.i;
    i__1 = a_dim1 * 3 + 2;
    q__3.r = -wx->r, q__3.i = -wx->i;
    i__2 = (a_dim1 << 1) + 2;
    q__2.r = q__3.r * a[i__2].r - q__3.i * a[i__2].i, q__2.i = q__3.r * a[
	    i__2].i + q__3.i * a[i__2].r;
    i__3 = a_dim1 * 3 + 3;
    q__4.r = wy->r * a[i__3].r - wy->i * a[i__3].i, q__4.i = wy->r * a[i__3]
	    .i + wy->i * a[i__3].r;
    q__1.r = q__2.r + q__4.r, q__1.i = q__2.i + q__4.i;
    a[i__1].r = q__1.r, a[i__1].i = q__1.i;
    i__1 = (a_dim1 << 2) + 1;
    i__2 = a_dim1 + 1;
    q__2.r = wx->r * a[i__2].r - wx->i * a[i__2].i, q__2.i = wx->r * a[i__2]
	    .i + wx->i * a[i__2].r;
    i__3 = (a_dim1 << 2) + 4;
    q__3.r = wy->r * a[i__3].r - wy->i * a[i__3].i, q__3.i = wy->r * a[i__3]
	    .i + wy->i * a[i__3].r;
    q__1.r = q__2.r - q__3.r, q__1.i = q__2.i - q__3.i;
    a[i__1].r = q__1.r, a[i__1].i = q__1.i;
    i__1 = (a_dim1 << 2) + 2;
    i__2 = (a_dim1 << 1) + 2;
    q__2.r = wx->r * a[i__2].r - wx->i * a[i__2].i, q__2.i = wx->r * a[i__2]
	    .i + wx->i * a[i__2].r;
    i__3 = (a_dim1 << 2) + 4;
    q__3.r = wy->r * a[i__3].r - wy->i * a[i__3].i, q__3.i = wy->r * a[i__3]
	    .i + wy->i * a[i__3].r;
    q__1.r = q__2.r - q__3.r, q__1.i = q__2.i - q__3.i;
    a[i__1].r = q__1.r, a[i__1].i = q__1.i;
    i__1 = a_dim1 * 5 + 1;
    q__3.r = -wx->r, q__3.i = -wx->i;
    i__2 = a_dim1 + 1;
    q__2.r = q__3.r * a[i__2].r - q__3.i * a[i__2].i, q__2.i = q__3.r * a[
	    i__2].i + q__3.i * a[i__2].r;
    i__3 = a_dim1 * 5 + 5;
    q__4.r = wy->r * a[i__3].r - wy->i * a[i__3].i, q__4.i = wy->r * a[i__3]
	    .i + wy->i * a[i__3].r;
    q__1.r = q__2.r + q__4.r, q__1.i = q__2.i + q__4.i;
    a[i__1].r = q__1.r, a[i__1].i = q__1.i;
    i__1 = a_dim1 * 5 + 2;
    i__2 = (a_dim1 << 1) + 2;
    q__2.r = wx->r * a[i__2].r - wx->i * a[i__2].i, q__2.i = wx->r * a[i__2]
	    .i + wx->i * a[i__2].r;
    i__3 = a_dim1 * 5 + 5;
    q__3.r = wy->r * a[i__3].r - wy->i * a[i__3].i, q__3.i = wy->r * a[i__3]
	    .i + wy->i * a[i__3].r;
    q__1.r = q__2.r + q__3.r, q__1.i = q__2.i + q__3.i;
    a[i__1].r = q__1.r, a[i__1].i = q__1.i;

/*     Compute condition numbers */

    s[1] = 1.f / sqrt((c_abs(wy) * 3.f * c_abs(wy) + 1.f) / (c_abs(&a[a_dim1 
	    + 1]) * c_abs(&a[a_dim1 + 1]) + 1.f));
    s[2] = 1.f / sqrt((c_abs(wy) * 3.f * c_abs(wy) + 1.f) / (c_abs(&a[(a_dim1 
	    << 1) + 2]) * c_abs(&a[(a_dim1 << 1) + 2]) + 1.f));
    s[3] = 1.f / sqrt((c_abs(wx) * 2.f * c_abs(wx) + 1.f) / (c_abs(&a[a_dim1 *
	     3 + 3]) * c_abs(&a[a_dim1 * 3 + 3]) + 1.f));
    s[4] = 1.f / sqrt((c_abs(wx) * 2.f * c_abs(wx) + 1.f) / (c_abs(&a[(a_dim1 
	    << 2) + 4]) * c_abs(&a[(a_dim1 << 2) + 4]) + 1.f));
    s[5] = 1.f / sqrt((c_abs(wx) * 2.f * c_abs(wx) + 1.f) / (c_abs(&a[a_dim1 *
	     5 + 5]) * c_abs(&a[a_dim1 * 5 + 5]) + 1.f));

    clakf2_(&c__1, &c__4, &a[a_offset], lda, &a[(a_dim1 << 1) + 2], &b[
	    b_offset], &b[(b_dim1 << 1) + 2], z__, &c__8);
    cgesvd_("N", "N", &c__8, &c__8, z__, &c__8, rwork, work, &c__1, &work[1], 
	    &c__1, &work[2], &c__24, &rwork[8], &info);
    dif[1] = rwork[7];

    clakf2_(&c__4, &c__1, &a[a_offset], lda, &a[a_dim1 * 5 + 5], &b[b_offset], 
	     &b[b_dim1 * 5 + 5], z__, &c__8);
    cgesvd_("N", "N", &c__8, &c__8, z__, &c__8, rwork, work, &c__1, &work[1], 
	    &c__1, &work[2], &c__24, &rwork[8], &info);
    dif[5] = rwork[7];

    return 0;

/*     End of CLATM6 */

} /* clatm6_ */
