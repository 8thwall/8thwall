#include "f2c.h"
#include "blaswrap.h"

/* Common Block Declarations */

struct {
    integer iparms[100];
} claenv_;

#define claenv_1 claenv_

/* Table of constant values */

static integer c__0 = 0;
static real c_b3 = 0.f;
static real c_b4 = 1.f;
static integer c__1 = 1;

integer ilaenv_(integer *ispec, char *name__, char *opts, integer *n1, 
	integer *n2, integer *n3, integer *n4)
{
    /* System generated locals */
    integer ret_val;

    /* Local variables */
    extern integer ieeeck_(integer *, real *, real *);


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ILAENV returns problem-dependent parameters for the local */
/*  environment.  See ISPEC for a description of the parameters. */

/*  In this version, the problem-dependent parameters are contained in */
/*  the integer array IPARMS in the common block CLAENV and the value */
/*  with index ISPEC is copied to ILAENV.  This version of ILAENV is */
/*  to be used in conjunction with XLAENV in TESTING and TIMING. */

/*  Arguments */
/*  ========= */

/*  ISPEC   (input) INTEGER */
/*          Specifies the parameter to be returned as the value of */
/*          ILAENV. */
/*          = 1: the optimal blocksize; if this value is 1, an unblocked */
/*               algorithm will give the best performance. */
/*          = 2: the minimum block size for which the block routine */
/*               should be used; if the usable block size is less than */
/*               this value, an unblocked routine should be used. */
/*          = 3: the crossover point (in a block routine, for N less */
/*               than this value, an unblocked routine should be used) */
/*          = 4: the number of shifts, used in the nonsymmetric */
/*               eigenvalue routines */
/*          = 5: the minimum column dimension for blocking to be used; */
/*               rectangular blocks must have dimension at least k by m, */
/*               where k is given by ILAENV(2,...) and m by ILAENV(5,...) */
/*          = 6: the crossover point for the SVD (when reducing an m by n */
/*               matrix to bidiagonal form, if max(m,n)/min(m,n) exceeds */
/*               this value, a QR factorization is used first to reduce */
/*               the matrix to a triangular form.) */
/*          = 7: the number of processors */
/*          = 8: the crossover point for the multishift QR and QZ methods */
/*               for nonsymmetric eigenvalue problems. */
/*          = 9: maximum size of the subproblems at the bottom of the */
/*               computation tree in the divide-and-conquer algorithm */
/*          =10: ieee NaN arithmetic can be trusted not to trap */
/*          =11: infinity arithmetic can be trusted not to trap */
/*          12 <= ISPEC <= 16: */
/*               xHSEQR or one of its subroutines, */
/*               see IPARMQ for detailed explanation */

/*          Other specifications (up to 100) can be added later. */

/*  NAME    (input) CHARACTER*(*) */
/*          The name of the calling subroutine. */

/*  OPTS    (input) CHARACTER*(*) */
/*          The character options to the subroutine NAME, concatenated */
/*          into a single character string.  For example, UPLO = 'U', */
/*          TRANS = 'T', and DIAG = 'N' for a triangular routine would */
/*          be specified as OPTS = 'UTN'. */

/*  N1      (input) INTEGER */
/*  N2      (input) INTEGER */
/*  N3      (input) INTEGER */
/*  N4      (input) INTEGER */
/*          Problem dimensions for the subroutine NAME; these may not all */
/*          be required. */

/* (ILAENV) (output) INTEGER */
/*          >= 0: the value of the parameter specified by ISPEC */
/*          < 0:  if ILAENV = -k, the k-th argument had an illegal value. */

/*  Further Details */
/*  =============== */

/*  The following conventions have been used when calling ILAENV from the */
/*  LAPACK routines: */
/*  1)  OPTS is a concatenation of all of the character options to */
/*      subroutine NAME, in the same order that they appear in the */
/*      argument list for NAME, even if they are not used in determining */
/*      the value of the parameter specified by ISPEC. */
/*  2)  The problem dimensions N1, N2, N3, N4 are specified in the order */
/*      that they appear in the argument list for NAME.  N1 is used */
/*      first, N2 second, and so on, and unused problem dimensions are */
/*      passed a value of -1. */
/*  3)  The parameter value returned by ILAENV is checked for validity in */
/*      the calling subroutine.  For example, ILAENV is used to retrieve */
/*      the optimal blocksize for STRTRI as follows: */

/*      NB = ILAENV( 1, 'STRTRI', UPLO // DIAG, N, -1, -1, -1 ) */
/*      IF( NB.LE.1 ) NB = MAX( 1, N ) */

/*  ===================================================================== */

/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Arrays in Common .. */
/*     .. */
/*     .. Common blocks .. */
/*     .. */
/*     .. Save statement .. */
/*     .. */
/*     .. Executable Statements .. */

    if (*ispec >= 1 && *ispec <= 5) {

/*        Return a value from the common block. */

	ret_val = claenv_1.iparms[*ispec - 1];

    } else if (*ispec == 6) {

/*        Compute SVD crossover point. */

	ret_val = (integer) ((real) min(*n1,*n2) * 1.6f);

    } else if (*ispec >= 7 && *ispec <= 9) {

/*        Return a value from the common block. */

	ret_val = claenv_1.iparms[*ispec - 1];

    } else if (*ispec == 10) {

/*        IEEE NaN arithmetic can be trusted not to trap */

/*        ILAENV = 0 */
	ret_val = 1;
	if (ret_val == 1) {
	    ret_val = ieeeck_(&c__0, &c_b3, &c_b4);
	}

    } else if (*ispec == 11) {

/*        Infinity arithmetic can be trusted not to trap */

/*        ILAENV = 0 */
	ret_val = 1;
	if (ret_val == 1) {
	    ret_val = ieeeck_(&c__1, &c_b3, &c_b4);
	}

    } else if (*ispec >= 12 && *ispec <= 16) {

/*     12 <= ISPEC <= 16: xHSEQR or one of its subroutines. */

	ret_val = claenv_1.iparms[*ispec - 1];
/*         WRITE(*,*) 'ISPEC = ',ISPEC,' ILAENV =',ILAENV */
/*         ILAENV = IPARMQ( ISPEC, NAME, OPTS, N1, N2, N3, N4 ) */

    } else {

/*        Invalid value for ISPEC */

	ret_val = -1;
    }

    return ret_val;

/*     End of ILAENV */

} /* ilaenv_ */

integer iparmq_(integer *ispec, char *name__, char *opts, integer *n, integer 
	*ilo, integer *ihi, integer *lwork)
{
    /* System generated locals */
    integer ret_val, i__1, i__2;
    real r__1;

    /* Builtin functions */
    double log(doublereal);
    integer i_nint(real *);

    /* Local variables */
    integer nh, ns;


/*     .. */
/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */
    if (*ispec == 15 || *ispec == 13 || *ispec == 16) {

/*        ==== Set the number simultaneous shifts ==== */

	nh = *ihi - *ilo + 1;
	ns = 2;
	if (nh >= 30) {
	    ns = 4;
	}
	if (nh >= 60) {
	    ns = 10;
	}
	if (nh >= 150) {
/* Computing MAX */
	    r__1 = log((real) nh) / log(2.f);
	    i__1 = 10, i__2 = nh / i_nint(&r__1);
	    ns = max(i__1,i__2);
	}
	if (nh >= 590) {
	    ns = 64;
	}
	if (nh >= 3000) {
	    ns = 128;
	}
	if (nh >= 6000) {
	    ns = 256;
	}
/* Computing MAX */
	i__1 = 2, i__2 = ns - ns % 2;
	ns = max(i__1,i__2);
    }

    if (*ispec == 12) {


/*        ===== Matrices of order smaller than NMIN get sent */
/*        .     to LAHQR, the classic double shift algorithm. */
/*        .     This must be at least 11. ==== */

	ret_val = 11;

    } else if (*ispec == 14) {

/*        ==== INIBL: skip a multi-shift qr iteration and */
/*        .    whenever aggressive early deflation finds */
/*        .    at least (NIBBLE*(window size)/100) deflations. ==== */

	ret_val = 14;

    } else if (*ispec == 15) {

/*        ==== NSHFTS: The number of simultaneous shifts ===== */

	ret_val = ns;

    } else if (*ispec == 13) {

/*        ==== NW: deflation window size.  ==== */

	if (nh <= 500) {
	    ret_val = ns;
	} else {
	    ret_val = ns * 3 / 2;
	}

    } else if (*ispec == 16) {

/*        ==== IACC22: Whether to accumulate reflections */
/*        .     before updating the far-from-diagonal elements */
/*        .     and whether to use 2-by-2 block structure while */
/*        .     doing it.  A small amount of work could be saved */
/*        .     by making this choice dependent also upon the */
/*        .     NH=IHI-ILO+1. */

	ret_val = 0;
	if (ns >= 14) {
	    ret_val = 1;
	}
	if (ns >= 14) {
	    ret_val = 2;
	}

    } else {
/*        ===== invalid value of ispec ===== */
	ret_val = -1;

    }

/*     ==== End of IPARMQ ==== */

    return ret_val;
} /* iparmq_ */
