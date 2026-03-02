#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__3 = 3;
static integer c__1 = 1;
static integer c__6 = 6;
static integer c__10 = 10;
static complex c_b43 = {1.f,0.f};

/* Subroutine */ int cget35_(real *rmax, integer *lmax, integer *ninfo, 
	integer *knt, integer *nin)
{
    /* System generated locals */
    integer i__1, i__2, i__3, i__4, i__5;
    real r__1, r__2;
    complex q__1;

    /* Builtin functions */
    double sqrt(doublereal);
    integer s_rsle(cilist *), do_lio(integer *, integer *, char *, ftnlen), 
	    e_rsle(void);
    double c_abs(complex *);
    void c_div(complex *, complex *, complex *);

    /* Local variables */
    complex a[100]	/* was [10][10] */, b[100]	/* was [10][10] */, 
	    c__[100]	/* was [10][10] */;
    integer i__, j, m, n;
    real vm1[3], vm2[3], dum[1], eps, res, res1;
    integer imla, imlb, imlc, info;
    complex csav[100]	/* was [10][10] */;
    integer isgn;
    complex atmp[100]	/* was [10][10] */, btmp[100]	/* was [10][10] */, 
	    ctmp[100]	/* was [10][10] */;
    real tnrm;
    complex rmul;
    real xnrm;
    integer imlad;
    real scale;
    extern /* Subroutine */ int cgemm_(char *, char *, integer *, integer *, 
	    integer *, complex *, complex *, integer *, complex *, integer *, 
	    complex *, complex *, integer *);
    char trana[1], tranb[1];
    extern /* Subroutine */ int slabad_(real *, real *);
    extern doublereal clange_(char *, integer *, integer *, complex *, 
	    integer *, real *), slamch_(char *);
    integer itrana, itranb;
    real bignum, smlnum;
    extern /* Subroutine */ int ctrsyl_(char *, char *, integer *, integer *, 
	    integer *, complex *, integer *, complex *, integer *, complex *, 
	    integer *, real *, integer *);

    /* Fortran I/O blocks */
    static cilist io___6 = { 0, 0, 0, 0, 0 };
    static cilist io___10 = { 0, 0, 0, 0, 0 };
    static cilist io___13 = { 0, 0, 0, 0, 0 };
    static cilist io___15 = { 0, 0, 0, 0, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CGET35 tests CTRSYL, a routine for solving the Sylvester matrix */
/*  equation */

/*     op(A)*X + ISGN*X*op(B) = scale*C, */

/*  A and B are assumed to be in Schur canonical form, op() represents an */
/*  optional transpose, and ISGN can be -1 or +1.  Scale is an output */
/*  less than or equal to 1, chosen to avoid overflow in X. */

/*  The test code verifies that the following residual is order 1: */

/*     norm(op(A)*X + ISGN*X*op(B) - scale*C) / */
/*         (EPS*max(norm(A),norm(B))*norm(X)) */

/*  Arguments */
/*  ========== */

/*  RMAX    (output) REAL */
/*          Value of the largest test ratio. */

/*  LMAX    (output) INTEGER */
/*          Example number where largest test ratio achieved. */

/*  NINFO   (output) INTEGER */
/*          Number of examples where INFO is nonzero. */

/*  KNT     (output) INTEGER */
/*          Total number of examples tested. */

/*  NIN     (input) INTEGER */
/*          Input logical unit number. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Local Arrays .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Get machine parameters */

    eps = slamch_("P");
    smlnum = slamch_("S") / eps;
    bignum = 1.f / smlnum;
    slabad_(&smlnum, &bignum);

/*     Set up test case parameters */

    vm1[0] = sqrt(smlnum);
    vm1[1] = 1.f;
    vm1[2] = 1e6f;
    vm2[0] = 1.f;
    vm2[1] = eps * 2.f + 1.f;
    vm2[2] = 2.f;

    *knt = 0;
    *ninfo = 0;
    *lmax = 0;
    *rmax = 0.f;

/*     Begin test loop */

L10:
    io___6.ciunit = *nin;
    s_rsle(&io___6);
    do_lio(&c__3, &c__1, (char *)&m, (ftnlen)sizeof(integer));
    do_lio(&c__3, &c__1, (char *)&n, (ftnlen)sizeof(integer));
    e_rsle();
    if (n == 0) {
	return 0;
    }
    i__1 = m;
    for (i__ = 1; i__ <= i__1; ++i__) {
	io___10.ciunit = *nin;
	s_rsle(&io___10);
	i__2 = m;
	for (j = 1; j <= i__2; ++j) {
	    do_lio(&c__6, &c__1, (char *)&atmp[i__ + j * 10 - 11], (ftnlen)
		    sizeof(complex));
	}
	e_rsle();
/* L20: */
    }
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	io___13.ciunit = *nin;
	s_rsle(&io___13);
	i__2 = n;
	for (j = 1; j <= i__2; ++j) {
	    do_lio(&c__6, &c__1, (char *)&btmp[i__ + j * 10 - 11], (ftnlen)
		    sizeof(complex));
	}
	e_rsle();
/* L30: */
    }
    i__1 = m;
    for (i__ = 1; i__ <= i__1; ++i__) {
	io___15.ciunit = *nin;
	s_rsle(&io___15);
	i__2 = n;
	for (j = 1; j <= i__2; ++j) {
	    do_lio(&c__6, &c__1, (char *)&ctmp[i__ + j * 10 - 11], (ftnlen)
		    sizeof(complex));
	}
	e_rsle();
/* L40: */
    }
    for (imla = 1; imla <= 3; ++imla) {
	for (imlad = 1; imlad <= 3; ++imlad) {
	    for (imlb = 1; imlb <= 3; ++imlb) {
		for (imlc = 1; imlc <= 3; ++imlc) {
		    for (itrana = 1; itrana <= 2; ++itrana) {
			for (itranb = 1; itranb <= 2; ++itranb) {
			    for (isgn = -1; isgn <= 1; isgn += 2) {
				if (itrana == 1) {
				    *(unsigned char *)trana = 'N';
				}
				if (itrana == 2) {
				    *(unsigned char *)trana = 'C';
				}
				if (itranb == 1) {
				    *(unsigned char *)tranb = 'N';
				}
				if (itranb == 2) {
				    *(unsigned char *)tranb = 'C';
				}
				tnrm = 0.f;
				i__1 = m;
				for (i__ = 1; i__ <= i__1; ++i__) {
				    i__2 = m;
				    for (j = 1; j <= i__2; ++j) {
					i__3 = i__ + j * 10 - 11;
					i__4 = i__ + j * 10 - 11;
					i__5 = imla - 1;
					q__1.r = vm1[i__5] * atmp[i__4].r, 
						q__1.i = vm1[i__5] * atmp[
						i__4].i;
					a[i__3].r = q__1.r, a[i__3].i = 
						q__1.i;
/* Computing MAX */
					r__1 = tnrm, r__2 = c_abs(&a[i__ + j *
						 10 - 11]);
					tnrm = dmax(r__1,r__2);
/* L50: */
				    }
				    i__2 = i__ + i__ * 10 - 11;
				    i__3 = i__ + i__ * 10 - 11;
				    i__4 = imlad - 1;
				    q__1.r = vm2[i__4] * a[i__3].r, q__1.i = 
					    vm2[i__4] * a[i__3].i;
				    a[i__2].r = q__1.r, a[i__2].i = q__1.i;
/* Computing MAX */
				    r__1 = tnrm, r__2 = c_abs(&a[i__ + i__ * 
					    10 - 11]);
				    tnrm = dmax(r__1,r__2);
/* L60: */
				}
				i__1 = n;
				for (i__ = 1; i__ <= i__1; ++i__) {
				    i__2 = n;
				    for (j = 1; j <= i__2; ++j) {
					i__3 = i__ + j * 10 - 11;
					i__4 = i__ + j * 10 - 11;
					i__5 = imlb - 1;
					q__1.r = vm1[i__5] * btmp[i__4].r, 
						q__1.i = vm1[i__5] * btmp[
						i__4].i;
					b[i__3].r = q__1.r, b[i__3].i = 
						q__1.i;
/* Computing MAX */
					r__1 = tnrm, r__2 = c_abs(&b[i__ + j *
						 10 - 11]);
					tnrm = dmax(r__1,r__2);
/* L70: */
				    }
/* L80: */
				}
				if (tnrm == 0.f) {
				    tnrm = 1.f;
				}
				i__1 = m;
				for (i__ = 1; i__ <= i__1; ++i__) {
				    i__2 = n;
				    for (j = 1; j <= i__2; ++j) {
					i__3 = i__ + j * 10 - 11;
					i__4 = i__ + j * 10 - 11;
					i__5 = imlc - 1;
					q__1.r = vm1[i__5] * ctmp[i__4].r, 
						q__1.i = vm1[i__5] * ctmp[
						i__4].i;
					c__[i__3].r = q__1.r, c__[i__3].i = 
						q__1.i;
					i__3 = i__ + j * 10 - 11;
					i__4 = i__ + j * 10 - 11;
					csav[i__3].r = c__[i__4].r, csav[i__3]
						.i = c__[i__4].i;
/* L90: */
				    }
/* L100: */
				}
				++(*knt);
				ctrsyl_(trana, tranb, &isgn, &m, &n, a, &
					c__10, b, &c__10, c__, &c__10, &scale, 
					 &info);
				if (info != 0) {
				    ++(*ninfo);
				}
				xnrm = clange_("M", &m, &n, c__, &c__10, dum);
				rmul.r = 1.f, rmul.i = 0.f;
				if (xnrm > 1.f && tnrm > 1.f) {
				    if (xnrm > bignum / tnrm) {
					r__1 = dmax(xnrm,tnrm);
					rmul.r = r__1, rmul.i = 0.f;
					c_div(&q__1, &c_b43, &rmul);
					rmul.r = q__1.r, rmul.i = q__1.i;
				    }
				}
				r__1 = -scale;
				q__1.r = r__1 * rmul.r, q__1.i = r__1 * 
					rmul.i;
				cgemm_(trana, "N", &m, &n, &m, &rmul, a, &
					c__10, c__, &c__10, &q__1, csav, &
					c__10);
				r__1 = (real) isgn;
				q__1.r = r__1 * rmul.r, q__1.i = r__1 * 
					rmul.i;
				cgemm_("N", tranb, &m, &n, &n, &q__1, c__, &
					c__10, b, &c__10, &c_b43, csav, &
					c__10);
				res1 = clange_("M", &m, &n, csav, &c__10, dum);
/* Computing MAX */
				r__1 = smlnum, r__2 = smlnum * xnrm, r__1 = 
					max(r__1,r__2), r__2 = c_abs(&rmul) * 
					tnrm * eps * xnrm;
				res = res1 / dmax(r__1,r__2);
				if (res > *rmax) {
				    *lmax = *knt;
				    *rmax = res;
				}
/* L110: */
			    }
/* L120: */
			}
/* L130: */
		    }
/* L140: */
		}
/* L150: */
	    }
/* L160: */
	}
/* L170: */
    }
    goto L10;

/*     End of CGET35 */

} /* cget35_ */
