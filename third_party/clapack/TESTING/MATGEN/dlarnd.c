#include "f2c.h"
#include "blaswrap.h"

doublereal dlarnd_(integer *idist, integer *iseed)
{
    /* System generated locals */
    doublereal ret_val;

    /* Builtin functions */
    double log(doublereal), sqrt(doublereal), cos(doublereal);

    /* Local variables */
    doublereal t1, t2;
    extern doublereal dlaran_(integer *);


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLARND returns a random real number from a uniform or normal */
/*  distribution. */

/*  Arguments */
/*  ========= */

/*  IDIST   (input) INTEGER */
/*          Specifies the distribution of the random numbers: */
/*          = 1:  uniform (0,1) */
/*          = 2:  uniform (-1,1) */
/*          = 3:  normal (0,1) */

/*  ISEED   (input/output) INTEGER array, dimension (4) */
/*          On entry, the seed of the random number generator; the array */
/*          elements must be between 0 and 4095, and ISEED(4) must be */
/*          odd. */
/*          On exit, the seed is updated. */

/*  Further Details */
/*  =============== */

/*  This routine calls the auxiliary routine DLARAN to generate a random */
/*  real number from a uniform (0,1) distribution. The Box-Muller method */
/*  is used to transform numbers from a uniform to a normal distribution. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Generate a real random number from a uniform (0,1) distribution */

    /* Parameter adjustments */
    --iseed;

    /* Function Body */
    t1 = dlaran_(&iseed[1]);

    if (*idist == 1) {

/*        uniform (0,1) */

	ret_val = t1;
    } else if (*idist == 2) {

/*        uniform (-1,1) */

	ret_val = t1 * 2. - 1.;
    } else if (*idist == 3) {

/*        normal (0,1) */

	t2 = dlaran_(&iseed[1]);
	ret_val = sqrt(log(t1) * -2.) * cos(t2 * 
		6.2831853071795864769252867663);
    }
    return ret_val;

/*     End of DLARND */

} /* dlarnd_ */
