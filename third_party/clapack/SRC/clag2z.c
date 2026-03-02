#include "f2c.h"
#include "blaswrap.h"

/* Subroutine */ int clag2z_(integer *m, integer *n, complex *sa, integer *
	ldsa, doublecomplex *a, integer *lda, integer *info)
{
    /* System generated locals */
    integer sa_dim1, sa_offset, a_dim1, a_offset, i__1, i__2, i__3, i__4;

    /* Local variables */
    integer i__, j;


/*  -- LAPACK PROTOTYPE auxilary routine (version 3.1.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     January 2007 */

/*     .. */
/*     .. WARNING: PROTOTYPE .. */
/*     This is an LAPACK PROTOTYPE routine which means that the */
/*     interface of this routine is likely to be changed in the future */
/*     based on community feedback. */

/*     .. */
/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CLAG2Z converts a COMPLEX SINGLE PRECISION matrix, SA, to a COMPLEX */
/*  DOUBLE PRECISION matrix, A. */

/*  Note that while it is possible to overflow while converting */
/*  from double to single, it is not possible to overflow when */
/*  converting from single to double. */

/*  This is a helper routine so there is no argument checking. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of lines of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  SA      (output) REAL array, dimension (LDSA,N) */
/*          On exit, the M-by-N coefficient matrix SA. */

/*  LDSA    (input) INTEGER */
/*          The leading dimension of the array SA.  LDSA >= max(1,M). */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the M-by-N coefficient matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*  ========= */

/*     .. Local Scalars .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    sa_dim1 = *ldsa;
    sa_offset = 1 + sa_dim1;
    sa -= sa_offset;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;

    /* Function Body */
    *info = 0;
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	i__2 = *m;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    i__3 = i__ + j * a_dim1;
	    i__4 = i__ + j * sa_dim1;
	    a[i__3].r = sa[i__4].r, a[i__3].i = sa[i__4].i;
/* L30: */
	}
/* L20: */
    }
    return 0;

/*     End of CLAG2Z */

} /* clag2z_ */
