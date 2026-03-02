#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__8 = 8;
static integer c__1 = 1;
static integer c__0 = 0;

/* Subroutine */ int cckgqr_(integer *nm, integer *mval, integer *np, integer 
	*pval, integer *nn, integer *nval, integer *nmats, integer *iseed, 
	real *thresh, integer *nmax, complex *a, complex *af, complex *aq, 
	complex *ar, complex *taua, complex *b, complex *bf, complex *bz, 
	complex *bt, complex *bwk, complex *taub, complex *work, real *rwork, 
	integer *nin, integer *nout, integer *info)
{
    /* Format strings */
    static char fmt_9999[] = "(\002 CLATMS in CCKGQR:    INFO = \002,i5)";
    static char fmt_9998[] = "(\002 M=\002,i4,\002 P=\002,i4,\002, N=\002,"
	    "i4,\002, type \002,i2,\002, test \002,i2,\002, ratio=\002,g13.6)";
    static char fmt_9997[] = "(\002 N=\002,i4,\002 M=\002,i4,\002, P=\002,"
	    "i4,\002, type \002,i2,\002, test \002,i2,\002, ratio=\002,g13.6)";

    /* System generated locals */
    integer i__1, i__2, i__3, i__4;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    integer i__, m, n, p, im, in, ip, nt, lda, ldb, kla, klb, kua, kub;
    char path[3];
    integer imat;
    char type__[1];
    integer nrun, modea, modeb, nfail;
    char dista[1], distb[1];
    integer iinfo;
    real anorm, bnorm;
    integer lwork;
    extern /* Subroutine */ int slatb9_(char *, integer *, integer *, integer 
	    *, integer *, char *, integer *, integer *, integer *, integer *, 
	    real *, real *, integer *, integer *, real *, real *, char *, 
	    char *), alahdg_(integer *, char *
);
    real cndnma, cndnmb;
    extern /* Subroutine */ int alareq_(char *, integer *, logical *, integer 
	    *, integer *, integer *), alasum_(char *, integer *, 
	    integer *, integer *, integer *), clatms_(integer *, 
	    integer *, char *, integer *, char *, real *, integer *, real *, 
	    real *, integer *, integer *, char *, complex *, integer *, 
	    complex *, integer *), cgqrts_(integer *, 
	    integer *, integer *, complex *, complex *, complex *, complex *, 
	    integer *, complex *, complex *, complex *, complex *, complex *, 
	    complex *, integer *, complex *, complex *, integer *, real *, 
	    real *);
    logical dotype[8];
    extern /* Subroutine */ int cgrqts_(integer *, integer *, integer *, 
	    complex *, complex *, complex *, complex *, integer *, complex *, 
	    complex *, complex *, complex *, complex *, complex *, integer *, 
	    complex *, complex *, integer *, real *, real *);
    logical firstt;
    real result[7];

    /* Fortran I/O blocks */
    static cilist io___30 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___31 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___35 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___36 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___37 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___38 = { 0, 0, 0, fmt_9997, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  CCKGQR tests */
/*  CGGQRF: GQR factorization for N-by-M matrix A and N-by-P matrix B, */
/*  CGGRQF: GRQ factorization for M-by-N matrix A and P-by-N matrix B. */

/*  Arguments */
/*  ========= */

/*  NM      (input) INTEGER */
/*          The number of values of M contained in the vector MVAL. */

/*  MVAL    (input) INTEGER array, dimension (NM) */
/*          The values of the matrix row(column) dimension M. */

/*  NP      (input) INTEGER */
/*          The number of values of P contained in the vector PVAL. */

/*  PVAL    (input) INTEGER array, dimension (NP) */
/*          The values of the matrix row(column) dimension P. */

/*  NN      (input) INTEGER */
/*          The number of values of N contained in the vector NVAL. */

/*  NVAL    (input) INTEGER array, dimension (NN) */
/*          The values of the matrix column(row) dimension N. */

/*  NMATS   (input) INTEGER */
/*          The number of matrix types to be tested for each combination */
/*          of matrix dimensions.  If NMATS >= NTYPES (the maximum */
/*          number of matrix types), then all the different types are */
/*          generated for testing.  If NMATS < NTYPES, another input line */
/*          is read to get the numbers of the matrix types to be used. */

/*  ISEED   (input/output) INTEGER array, dimension (4) */
/*          On entry, the seed of the random number generator.  The array */
/*          elements should be between 0 and 4095, otherwise they will be */
/*          reduced mod 4096, and ISEED(4) must be odd. */
/*          On exit, the next seed in the random number sequence after */
/*          all the test matrices have been generated. */

/*  THRESH  (input) REAL */
/*          The threshold value for the test ratios.  A result is */
/*          included in the output file if RESULT >= THRESH.  To have */
/*          every test ratio printed, use THRESH = 0. */

/*  NMAX    (input) INTEGER */
/*          The maximum value permitted for M or N, used in dimensioning */
/*          the work arrays. */

/*  A       (workspace) COMPLEX array, dimension (NMAX*NMAX) */

/*  AF      (workspace) COMPLEX array, dimension (NMAX*NMAX) */

/*  AQ      (workspace) COMPLEX array, dimension (NMAX*NMAX) */

/*  AR      (workspace) COMPLEX array, dimension (NMAX*NMAX) */

/*  TAUA    (workspace) COMPLEX array, dimension (NMAX) */

/*  B       (workspace) COMPLEX array, dimension (NMAX*NMAX) */

/*  BF      (workspace) COMPLEX array, dimension (NMAX*NMAX) */

/*  BZ      (workspace) COMPLEX array, dimension (NMAX*NMAX) */

/*  BT      (workspace) COMPLEX array, dimension (NMAX*NMAX) */

/*  BWK     (workspace) COMPLEX array, dimension (NMAX*NMAX) */

/*  TAUB    (workspace) COMPLEX array, dimension (NMAX) */

/*  WORK    (workspace) COMPLEX array, dimension (NMAX*NMAX) */

/*  RWORK   (workspace) REAL array, dimension (NMAX) */

/*  NIN     (input) INTEGER */
/*          The unit number for input. */

/*  NOUT    (input) INTEGER */
/*          The unit number for output. */

/*  INFO    (output) INTEGER */
/*          = 0 :  successful exit */
/*          > 0 :  If CLATMS returns an error code, the absolute value */
/*                 of it is returned. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Local Arrays .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Initialize constants. */

    /* Parameter adjustments */
    --rwork;
    --work;
    --taub;
    --bwk;
    --bt;
    --bz;
    --bf;
    --b;
    --taua;
    --ar;
    --aq;
    --af;
    --a;
    --iseed;
    --nval;
    --pval;
    --mval;

    /* Function Body */
    s_copy(path, "GQR", (ftnlen)3, (ftnlen)3);
    *info = 0;
    nrun = 0;
    nfail = 0;
    firstt = TRUE_;
    alareq_(path, nmats, dotype, &c__8, nin, nout);
    lda = *nmax;
    ldb = *nmax;
    lwork = *nmax * *nmax;

/*     Do for each value of M in MVAL. */

    i__1 = *nm;
    for (im = 1; im <= i__1; ++im) {
	m = mval[im];

/*        Do for each value of P in PVAL. */

	i__2 = *np;
	for (ip = 1; ip <= i__2; ++ip) {
	    p = pval[ip];

/*           Do for each value of N in NVAL. */

	    i__3 = *nn;
	    for (in = 1; in <= i__3; ++in) {
		n = nval[in];

		for (imat = 1; imat <= 8; ++imat) {

/*                 Do the tests only if DOTYPE( IMAT ) is true. */

		    if (! dotype[imat - 1]) {
			goto L30;
		    }

/*                 Test CGGRQF */

/*                 Set up parameters with SLATB9 and generate test */
/*                 matrices A and B with CLATMS. */

		    slatb9_("GRQ", &imat, &m, &p, &n, type__, &kla, &kua, &
			    klb, &kub, &anorm, &bnorm, &modea, &modeb, &
			    cndnma, &cndnmb, dista, distb);

		    clatms_(&m, &n, dista, &iseed[1], type__, &rwork[1], &
			    modea, &cndnma, &anorm, &kla, &kua, "No packing", 
			    &a[1], &lda, &work[1], &iinfo);
		    if (iinfo != 0) {
			io___30.ciunit = *nout;
			s_wsfe(&io___30);
			do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer))
				;
			e_wsfe();
			*info = abs(iinfo);
			goto L30;
		    }

		    clatms_(&p, &n, distb, &iseed[1], type__, &rwork[1], &
			    modeb, &cndnmb, &bnorm, &klb, &kub, "No packing", 
			    &b[1], &ldb, &work[1], &iinfo);
		    if (iinfo != 0) {
			io___31.ciunit = *nout;
			s_wsfe(&io___31);
			do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer))
				;
			e_wsfe();
			*info = abs(iinfo);
			goto L30;
		    }

		    nt = 4;

		    cgrqts_(&m, &p, &n, &a[1], &af[1], &aq[1], &ar[1], &lda, &
			    taua[1], &b[1], &bf[1], &bz[1], &bt[1], &bwk[1], &
			    ldb, &taub[1], &work[1], &lwork, &rwork[1], 
			    result);

/*                 Print information about the tests that did not */
/*                 pass the threshold. */

		    i__4 = nt;
		    for (i__ = 1; i__ <= i__4; ++i__) {
			if (result[i__ - 1] >= *thresh) {
			    if (nfail == 0 && firstt) {
				firstt = FALSE_;
				alahdg_(nout, "GRQ");
			    }
			    io___35.ciunit = *nout;
			    s_wsfe(&io___35);
			    do_fio(&c__1, (char *)&m, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&p, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&imat, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, (char *)&result[i__ - 1], (ftnlen)
				    sizeof(real));
			    e_wsfe();
			    ++nfail;
			}
/* L10: */
		    }
		    nrun += nt;

/*                 Test CGGQRF */

/*                 Set up parameters with SLATB9 and generate test */
/*                 matrices A and B with CLATMS. */

		    slatb9_("GQR", &imat, &m, &p, &n, type__, &kla, &kua, &
			    klb, &kub, &anorm, &bnorm, &modea, &modeb, &
			    cndnma, &cndnmb, dista, distb);

		    clatms_(&n, &m, dista, &iseed[1], type__, &rwork[1], &
			    modea, &cndnma, &anorm, &kla, &kua, "No packing", 
			    &a[1], &lda, &work[1], &iinfo);
		    if (iinfo != 0) {
			io___36.ciunit = *nout;
			s_wsfe(&io___36);
			do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer))
				;
			e_wsfe();
			*info = abs(iinfo);
			goto L30;
		    }

		    clatms_(&n, &p, distb, &iseed[1], type__, &rwork[1], &
			    modea, &cndnma, &bnorm, &klb, &kub, "No packing", 
			    &b[1], &ldb, &work[1], &iinfo);
		    if (iinfo != 0) {
			io___37.ciunit = *nout;
			s_wsfe(&io___37);
			do_fio(&c__1, (char *)&iinfo, (ftnlen)sizeof(integer))
				;
			e_wsfe();
			*info = abs(iinfo);
			goto L30;
		    }

		    nt = 4;

		    cgqrts_(&n, &m, &p, &a[1], &af[1], &aq[1], &ar[1], &lda, &
			    taua[1], &b[1], &bf[1], &bz[1], &bt[1], &bwk[1], &
			    ldb, &taub[1], &work[1], &lwork, &rwork[1], 
			    result);

/*                 Print information about the tests that did not */
/*                 pass the threshold. */

		    i__4 = nt;
		    for (i__ = 1; i__ <= i__4; ++i__) {
			if (result[i__ - 1] >= *thresh) {
			    if (nfail == 0 && firstt) {
				firstt = FALSE_;
				alahdg_(nout, path);
			    }
			    io___38.ciunit = *nout;
			    s_wsfe(&io___38);
			    do_fio(&c__1, (char *)&n, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&m, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&p, (ftnlen)sizeof(integer))
				    ;
			    do_fio(&c__1, (char *)&imat, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, (char *)&result[i__ - 1], (ftnlen)
				    sizeof(real));
			    e_wsfe();
			    ++nfail;
			}
/* L20: */
		    }
		    nrun += nt;

L30:
		    ;
		}
/* L40: */
	    }
/* L50: */
	}
/* L60: */
    }

/*     Print a summary of the results. */

    alasum_(path, nout, &nfail, &nrun, &c__0);

    return 0;

/*     End of CCKGQR */

} /* cckgqr_ */
