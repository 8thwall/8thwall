#include "f2c.h"
#include "blaswrap.h"

/* Common Block Declarations */

struct {
    integer selopt, seldim;
    logical selval[20];
    doublereal selwr[20], selwi[20];
} sslct_;

#define sslct_1 sslct_

logical dslect_(doublereal *zr, doublereal *zi)
{
    /* System generated locals */
    integer i__1;
    doublereal d__1, d__2;
    logical ret_val;

    /* Local variables */
    integer i__;
    doublereal x, rmin;
    extern doublereal dlapy2_(doublereal *, doublereal *);


/*  -- LAPACK test routine (version 3.1.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     February 2007 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSLECT returns .TRUE. if the eigenvalue ZR+sqrt(-1)*ZI is to be */
/*  selected, and otherwise it returns .FALSE. */
/*  It is used by DCHK41 to test if DGEES succesfully sorts eigenvalues, */
/*  and by DCHK43 to test if DGEESX succesfully sorts eigenvalues. */

/*  The common block /SSLCT/ controls how eigenvalues are selected. */
/*  If SELOPT = 0, then DSLECT return .TRUE. when ZR is less than zero, */
/*  and .FALSE. otherwise. */
/*  If SELOPT is at least 1, DSLECT returns SELVAL(SELOPT) and adds 1 */
/*  to SELOPT, cycling back to 1 at SELMAX. */

/*  Arguments */
/*  ========= */

/*  ZR      (input) DOUBLE PRECISION */
/*          The real part of a complex eigenvalue ZR + i*ZI. */

/*  ZI      (input) DOUBLE PRECISION */
/*          The imaginary part of a complex eigenvalue ZR + i*ZI. */

/*  ===================================================================== */

/*     .. Arrays in Common .. */
/*     .. */
/*     .. Scalars in Common .. */
/*     .. */
/*     .. Common blocks .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Parameters .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    if (sslct_1.selopt == 0) {
	ret_val = *zr < 0.;
    } else {
	d__1 = *zr - sslct_1.selwr[0];
	d__2 = *zi - sslct_1.selwi[0];
	rmin = dlapy2_(&d__1, &d__2);
	ret_val = sslct_1.selval[0];
	i__1 = sslct_1.seldim;
	for (i__ = 2; i__ <= i__1; ++i__) {
	    d__1 = *zr - sslct_1.selwr[i__ - 1];
	    d__2 = *zi - sslct_1.selwi[i__ - 1];
	    x = dlapy2_(&d__1, &d__2);
	    if (x <= rmin) {
		rmin = x;
		ret_val = sslct_1.selval[i__ - 1];
	    }
/* L10: */
	}
    }
    return ret_val;

/*     End of DSLECT */

} /* dslect_ */
