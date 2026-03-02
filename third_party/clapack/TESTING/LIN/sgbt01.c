#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;
static real c_b12 = -1.f;

/* Subroutine */ int sgbt01_(integer *m, integer *n, integer *kl, integer *ku, 
	 real *a, integer *lda, real *afac, integer *ldafac, integer *ipiv, 
	real *work, real *resid)
{
    /* System generated locals */
    integer a_dim1, a_offset, afac_dim1, afac_offset, i__1, i__2, i__3, i__4;
    real r__1, r__2;

    /* Local variables */
    integer i__, j;
    real t;
    integer i1, i2, kd, il, jl, ip, ju, iw, jua;
    real eps;
    integer lenj;
    real anorm;
    extern doublereal sasum_(integer *, real *, integer *);
    extern /* Subroutine */ int scopy_(integer *, real *, integer *, real *, 
	    integer *), saxpy_(integer *, real *, real *, integer *, real *, 
	    integer *);
    extern doublereal slamch_(char *);


/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SGBT01 reconstructs a band matrix  A  from its L*U factorization and */
/*  computes the residual: */
/*     norm(L*U - A) / ( N * norm(A) * EPS ), */
/*  where EPS is the machine epsilon. */

/*  The expression L*U - A is computed one column at a time, so A and */
/*  AFAC are not modified. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  KL      (input) INTEGER */
/*          The number of subdiagonals within the band of A.  KL >= 0. */

/*  KU      (input) INTEGER */
/*          The number of superdiagonals within the band of A.  KU >= 0. */

/*  A       (input/output) REAL array, dimension (LDA,N) */
/*          The original matrix A in band storage, stored in rows 1 to */
/*          KL+KU+1. */

/*  LDA     (input) INTEGER. */
/*          The leading dimension of the array A.  LDA >= max(1,KL+KU+1). */

/*  AFAC    (input) REAL array, dimension (LDAFAC,N) */
/*          The factored form of the matrix A.  AFAC contains the banded */
/*          factors L and U from the L*U factorization, as computed by */
/*          SGBTRF.  U is stored as an upper triangular band matrix with */
/*          KL+KU superdiagonals in rows 1 to KL+KU+1, and the */
/*          multipliers used during the factorization are stored in rows */
/*          KL+KU+2 to 2*KL+KU+1.  See SGBTRF for further details. */

/*  LDAFAC  (input) INTEGER */
/*          The leading dimension of the array AFAC. */
/*          LDAFAC >= max(1,2*KL*KU+1). */

/*  IPIV    (input) INTEGER array, dimension (min(M,N)) */
/*          The pivot indices from SGBTRF. */

/*  WORK    (workspace) REAL array, dimension (2*KL+KU+1) */

/*  RESID   (output) REAL */
/*          norm(L*U - A) / ( N * norm(A) * EPS ) */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Quick exit if M = 0 or N = 0. */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    afac_dim1 = *ldafac;
    afac_offset = 1 + afac_dim1;
    afac -= afac_offset;
    --ipiv;
    --work;

    /* Function Body */
    *resid = 0.f;
    if (*m <= 0 || *n <= 0) {
	return 0;
    }

/*     Determine EPS and the norm of A. */

    eps = slamch_("Epsilon");
    kd = *ku + 1;
    anorm = 0.f;
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
/* Computing MAX */
	i__2 = kd + 1 - j;
	i1 = max(i__2,1);
/* Computing MIN */
	i__2 = kd + *m - j, i__3 = *kl + kd;
	i2 = min(i__2,i__3);
	if (i2 >= i1) {
/* Computing MAX */
	    i__2 = i2 - i1 + 1;
	    r__1 = anorm, r__2 = sasum_(&i__2, &a[i1 + j * a_dim1], &c__1);
	    anorm = dmax(r__1,r__2);
	}
/* L10: */
    }

/*     Compute one column at a time of L*U - A. */

    kd = *kl + *ku + 1;
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {

/*        Copy the J-th column of U to WORK. */

/* Computing MIN */
	i__2 = *kl + *ku, i__3 = j - 1;
	ju = min(i__2,i__3);
/* Computing MIN */
	i__2 = *kl, i__3 = *m - j;
	jl = min(i__2,i__3);
	lenj = min(*m,j) - j + ju + 1;
	if (lenj > 0) {
	    scopy_(&lenj, &afac[kd - ju + j * afac_dim1], &c__1, &work[1], &
		    c__1);
	    i__2 = ju + jl + 1;
	    for (i__ = lenj + 1; i__ <= i__2; ++i__) {
		work[i__] = 0.f;
/* L20: */
	    }

/*           Multiply by the unit lower triangular matrix L.  Note that L */
/*           is stored as a product of transformations and permutations. */

/* Computing MIN */
	    i__2 = *m - 1;
	    i__3 = j - ju;
	    for (i__ = min(i__2,j); i__ >= i__3; --i__) {
/* Computing MIN */
		i__2 = *kl, i__4 = *m - i__;
		il = min(i__2,i__4);
		if (il > 0) {
		    iw = i__ - j + ju + 1;
		    t = work[iw];
		    saxpy_(&il, &t, &afac[kd + 1 + i__ * afac_dim1], &c__1, &
			    work[iw + 1], &c__1);
		    ip = ipiv[i__];
		    if (i__ != ip) {
			ip = ip - j + ju + 1;
			work[iw] = work[ip];
			work[ip] = t;
		    }
		}
/* L30: */
	    }

/*           Subtract the corresponding column of A. */

	    jua = min(ju,*ku);
	    if (jua + jl + 1 > 0) {
		i__3 = jua + jl + 1;
		saxpy_(&i__3, &c_b12, &a[*ku + 1 - jua + j * a_dim1], &c__1, &
			work[ju + 1 - jua], &c__1);
	    }

/*           Compute the 1-norm of the column. */

/* Computing MAX */
	    i__3 = ju + jl + 1;
	    r__1 = *resid, r__2 = sasum_(&i__3, &work[1], &c__1);
	    *resid = dmax(r__1,r__2);
	}
/* L40: */
    }

/*     Compute norm( L*U - A ) / ( N * norm(A) * EPS ) */

    if (anorm <= 0.f) {
	if (*resid != 0.f) {
	    *resid = 1.f / eps;
	}
    } else {
	*resid = *resid / (real) (*n) / anorm / eps;
    }

    return 0;

/*     End of SGBT01 */

} /* sgbt01_ */
