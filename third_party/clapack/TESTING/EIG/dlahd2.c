#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__1 = 1;
static integer c__2 = 2;

/* Subroutine */ int dlahd2_(integer *iounit, char *path)
{
    /* Format strings */
    static char fmt_9999[] = "(1x,a3,\002:  no header available\002)";
    static char fmt_9998[] = "(/1x,a3,\002 -- Real Non-symmetric eigenvalue "
	    "problem\002)";
    static char fmt_9988[] = "(\002 Matrix types (see xCHKHS for details):"
	    " \002)";
    static char fmt_9987[] = "(/\002 Special Matrices:\002,/\002  1=Zero mat"
	    "rix.             \002,\002           \002,\002  5=Diagonal: geom"
	    "etr. spaced entries.\002,/\002  2=Identity matrix.              "
	    "      \002,\002  6=Diagona\002,\002l: clustered entries.\002,"
	    "/\002  3=Transposed Jordan block.  \002,\002          \002,\002 "
	    " 7=Diagonal: large, evenly spaced.\002,/\002  \002,\0024=Diagona"
	    "l: evenly spaced entries.    \002,\002  8=Diagonal: s\002,\002ma"
	    "ll, evenly spaced.\002)";
    static char fmt_9986[] = "(\002 Dense, Non-Symmetric Matrices:\002,/\002"
	    "  9=Well-cond., ev\002,\002enly spaced eigenvals.\002,\002 14=Il"
	    "l-cond., geomet. spaced e\002,\002igenals.\002,/\002 10=Well-con"
	    "d., geom. spaced eigenvals. \002,\002 15=Ill-conditioned, cluste"
	    "red e.vals.\002,/\002 11=Well-cond\002,\002itioned, clustered e."
	    "vals. \002,\002 16=Ill-cond., random comp\002,\002lex \002,a6,"
	    "/\002 12=Well-cond., random complex \002,a6,\002   \002,\002 17="
	    "Ill-cond., large rand. complx \002,a4,/\002 13=Ill-condi\002,"
	    "\002tioned, evenly spaced.     \002,\002 18=Ill-cond., small ran"
	    "d.\002,\002 complx \002,a4)";
    static char fmt_9985[] = "(\002 19=Matrix with random O(1) entries.   "
	    " \002,\002 21=Matrix \002,\002with small random entries.\002,"
	    "/\002 20=Matrix with large ran\002,\002dom entries.   \002)";
    static char fmt_9984[] = "(/\002 Tests performed:   \002,\002(H is Hesse"
	    "nberg, T is Schur,\002,\002 U and Z are \002,a,\002,\002,/20x,a"
	    ",\002, W is a diagonal matr\002,\002ix of eigenvalues,\002,/20x"
	    ",\002L and R are the left and rig\002,\002ht eigenvector matrice"
	    "s)\002,/\002  1 = | A - U H U\002,a1,\002 |\002,\002 / ( |A| n u"
	    "lp )         \002,\002  2 = | I - U U\002,a1,\002 | / \002,\002("
	    " n ulp )\002,/\002  3 = | H - Z T Z\002,a1,\002 | / ( |H| n ulp"
	    " \002,\002)         \002,\002  4 = | I - Z Z\002,a1,\002 | / ( n"
	    " ulp )\002,/\002  5 = | A - UZ T (UZ)\002,a1,\002 | / ( |A| n ul"
	    "p )     \002,\002  6 = | I - UZ (UZ)\002,a1,\002 | / ( n ulp "
	    ")\002,/\002  7 = | T(\002,\002e.vects.) - T(no e.vects.) | / ( |"
	    "T| ulp )\002,/\002  8 = | W\002,\002(e.vects.) - W(no e.vects.) "
	    "| / ( |W| ulp )\002,/\002  9 = | \002,\002TR - RW | / ( |T| |R| "
	    "ulp )     \002,\002 10 = | LT - WL | / (\002,\002 |T| |L| ulp "
	    ")\002,/\002 11= |HX - XW| / (|H| |X| ulp)  (inv.\002,\002it)\002,"
	    "\002 12= |YH - WY| / (|H| |Y| ulp)  (inv.it)\002)";
    static char fmt_9997[] = "(/1x,a3,\002 -- Complex Non-symmetric eigenval"
	    "ue problem\002)";
    static char fmt_9996[] = "(/1x,a3,\002 -- Real Symmetric eigenvalue prob"
	    "lem\002)";
    static char fmt_9983[] = "(\002 Matrix types (see xDRVST for details):"
	    " \002)";
    static char fmt_9982[] = "(/\002 Special Matrices:\002,/\002  1=Zero mat"
	    "rix.             \002,\002           \002,\002  5=Diagonal: clus"
	    "tered entries.\002,/\002  2=\002,\002Identity matrix.           "
	    "         \002,\002  6=Diagonal: lar\002,\002ge, evenly spaced"
	    ".\002,/\002  3=Diagonal: evenly spaced entri\002,\002es.    \002,"
	    "\002  7=Diagonal: small, evenly spaced.\002,/\002  4=D\002,\002i"
	    "agonal: geometr. spaced entries.\002)";
    static char fmt_9981[] = "(\002 Dense \002,a,\002 Matrices:\002,/\002  8"
	    "=Evenly spaced eigen\002,\002vals.            \002,\002 12=Small"
	    ", evenly spaced eigenvals.\002,/\002  9=Geometrically spaced eig"
	    "envals.     \002,\002 13=Matrix \002,\002with random O(1) entrie"
	    "s.\002,/\002 10=Clustered eigenvalues.\002,\002              "
	    "\002,\002 14=Matrix with large random entries.\002,/\002 11=Larg"
	    "e, evenly spaced eigenvals.     \002,\002 15=Matrix \002,\002wit"
	    "h small random entries.\002)";
    static char fmt_9968[] = "(/\002 Tests performed:  See sdrvst.f\002)";
    static char fmt_9995[] = "(/1x,a3,\002 -- Complex Hermitian eigenvalue p"
	    "roblem\002)";
    static char fmt_9967[] = "(/\002 Tests performed:  See cdrvst.f\002)";
    static char fmt_9992[] = "(/1x,a3,\002 -- Real Symmetric Generalized eig"
	    "envalue \002,\002problem\002)";
    static char fmt_9980[] = "(\002 Matrix types (see xDRVSG for details):"
	    " \002)";
    static char fmt_9979[] = "(/\002 Special Matrices:\002,/\002  1=Zero mat"
	    "rix.             \002,\002           \002,\002  5=Diagonal: clus"
	    "tered entries.\002,/\002  2=\002,\002Identity matrix.           "
	    "         \002,\002  6=Diagonal: lar\002,\002ge, evenly spaced"
	    ".\002,/\002  3=Diagonal: evenly spaced entri\002,\002es.    \002,"
	    "\002  7=Diagonal: small, evenly spaced.\002,/\002  4=D\002,\002i"
	    "agonal: geometr. spaced entries.\002)";
    static char fmt_9978[] = "(\002 Dense or Banded \002,a,\002 Matrices:"
	    " \002,/\002  8=Evenly spaced eigenvals.         \002,\002 15=Mat"
	    "rix with small random entries.\002,/\002  9=Geometrically spaced"
	    " eigenvals.  \002,\002 16=Evenly spaced eigenvals, KA=1, KB=1"
	    ".\002,/\002 10=Clustered eigenvalues.           \002,\002 17=Eve"
	    "nly spaced eigenvals, KA=2, KB=1.\002,/\002 11=Large, evenly spa"
	    "ced eigenvals.  \002,\002 18=Evenly spaced eigenvals, KA=2, KB=2."
	    "\002,/\002 12=Small, evenly spaced eigenvals.  \002,\002 19=Even"
	    "ly spaced eigenvals, KA=3, KB=1.\002,/\002 13=Matrix with random"
	    " O(1) entries. \002,\002 20=Evenly spaced eigenvals, KA=3, KB=2"
	    ".\002,/\002 14=Matrix with large random entries.\002,\002 21=Eve"
	    "nly spaced eigenvals, KA=3, KB=3.\002)";
    static char fmt_9977[] = "(/\002 Tests performed:   \002,/\002( For each"
	    " pair (A,B), where A is of the given type \002,/\002 and B is a "
	    "random well-conditioned matrix. D is \002,/\002 diagonal, and Z "
	    "is orthogonal. )\002,/\002 1 = DSYGV, with ITYPE=1 and UPLO='U'"
	    ":\002,\002  | A Z - B Z D | / ( |A| |Z| n ulp )     \002,/\002 2"
	    " = DSPGV, with ITYPE=1 and UPLO='U':\002,\002  | A Z - B Z D | /"
	    " ( |A| |Z| n ulp )     \002,/\002 3 = DSBGV, with ITYPE=1 and UP"
	    "LO='U':\002,\002  | A Z - B Z D | / ( |A| |Z| n ulp )     \002,"
	    "/\002 4 = DSYGV, with ITYPE=1 and UPLO='L':\002,\002  | A Z - B "
	    "Z D | / ( |A| |Z| n ulp )     \002,/\002 5 = DSPGV, with ITYPE=1"
	    " and UPLO='L':\002,\002  | A Z - B Z D | / ( |A| |Z| n ulp )     "
	    "\002,/\002 6 = DSBGV, with ITYPE=1 and UPLO='L':\002,\002  | A Z"
	    " - B Z D | / ( |A| |Z| n ulp )     \002)";
    static char fmt_9976[] = "(\002 7 = DSYGV, with ITYPE=2 and UPLO='U':"
	    "\002,\002  | A B Z - Z D | / ( |A| |Z| n ulp )     \002,/\002 8 "
	    "= DSPGV, with ITYPE=2 and UPLO='U':\002,\002  | A B Z - Z D | / "
	    "( |A| |Z| n ulp )     \002,/\002 9 = DSPGV, with ITYPE=2 and UPL"
	    "O='L':\002,\002  | A B Z - Z D | / ( |A| |Z| n ulp )     \002,"
	    "/\00210 = DSPGV, with ITYPE=2 and UPLO='L':\002,\002  | A B Z - "
	    "Z D | / ( |A| |Z| n ulp )     \002,/\00211 = DSYGV, with ITYPE=3"
	    " and UPLO='U':\002,\002  | B A Z - Z D | / ( |A| |Z| n ulp )     "
	    "\002,/\00212 = DSPGV, with ITYPE=3 and UPLO='U':\002,\002  | B A"
	    " Z - Z D | / ( |A| |Z| n ulp )     \002,/\00213 = DSYGV, with IT"
	    "YPE=3 and UPLO='L':\002,\002  | B A Z - Z D | / ( |A| |Z| n ulp "
	    ")     \002,/\00214 = DSPGV, with ITYPE=3 and UPLO='L':\002,\002 "
	    " | B A Z - Z D | / ( |A| |Z| n ulp )     \002)";
    static char fmt_9991[] = "(/1x,a3,\002 -- Complex Hermitian Generalized "
	    "eigenvalue \002,\002problem\002)";
    static char fmt_9975[] = "(/\002 Tests performed:   \002,/\002( For each"
	    " pair (A,B), where A is of the given type \002,/\002 and B is a "
	    "random well-conditioned matrix. D is \002,/\002 diagonal, and Z "
	    "is unitary. )\002,/\002 1 = ZHEGV, with ITYPE=1 and UPLO='U':"
	    "\002,\002  | A Z - B Z D | / ( |A| |Z| n ulp )     \002,/\002 2 "
	    "= ZHPGV, with ITYPE=1 and UPLO='U':\002,\002  | A Z - B Z D | / "
	    "( |A| |Z| n ulp )     \002,/\002 3 = ZHBGV, with ITYPE=1 and UPL"
	    "O='U':\002,\002  | A Z - B Z D | / ( |A| |Z| n ulp )     \002,"
	    "/\002 4 = ZHEGV, with ITYPE=1 and UPLO='L':\002,\002  | A Z - B "
	    "Z D | / ( |A| |Z| n ulp )     \002,/\002 5 = ZHPGV, with ITYPE=1"
	    " and UPLO='L':\002,\002  | A Z - B Z D | / ( |A| |Z| n ulp )     "
	    "\002,/\002 6 = ZHBGV, with ITYPE=1 and UPLO='L':\002,\002  | A Z"
	    " - B Z D | / ( |A| |Z| n ulp )     \002)";
    static char fmt_9974[] = "(\002 7 = ZHEGV, with ITYPE=2 and UPLO='U':"
	    "\002,\002  | A B Z - Z D | / ( |A| |Z| n ulp )     \002,/\002 8 "
	    "= ZHPGV, with ITYPE=2 and UPLO='U':\002,\002  | A B Z - Z D | / "
	    "( |A| |Z| n ulp )     \002,/\002 9 = ZHPGV, with ITYPE=2 and UPL"
	    "O='L':\002,\002  | A B Z - Z D | / ( |A| |Z| n ulp )     \002,"
	    "/\00210 = ZHPGV, with ITYPE=2 and UPLO='L':\002,\002  | A B Z - "
	    "Z D | / ( |A| |Z| n ulp )     \002,/\00211 = ZHEGV, with ITYPE=3"
	    " and UPLO='U':\002,\002  | B A Z - Z D | / ( |A| |Z| n ulp )     "
	    "\002,/\00212 = ZHPGV, with ITYPE=3 and UPLO='U':\002,\002  | B A"
	    " Z - Z D | / ( |A| |Z| n ulp )     \002,/\00213 = ZHEGV, with IT"
	    "YPE=3 and UPLO='L':\002,\002  | B A Z - Z D | / ( |A| |Z| n ulp "
	    ")     \002,/\00214 = ZHPGV, with ITYPE=3 and UPLO='L':\002,\002 "
	    " | B A Z - Z D | / ( |A| |Z| n ulp )     \002)";
    static char fmt_9994[] = "(/1x,a3,\002 -- Real Singular Value Decomposit"
	    "ion\002)";
    static char fmt_9973[] = "(\002 Matrix types (see xCHKBD for details)"
	    ":\002,/\002 Diagonal matrices:\002,/\002   1: Zero\002,28x,\002 "
	    "5: Clustered entries\002,/\002   2: Identity\002,24x,\002 6: Lar"
	    "ge, evenly spaced entries\002,/\002   3: Evenly spaced entrie"
	    "s\002,11x,\002 7: Small, evenly spaced entries\002,/\002   4: Ge"
	    "ometrically spaced entries\002,/\002 General matrices:\002,/\002"
	    "   8: Evenly spaced sing. vals.\002,7x,\00212: Small, evenly spa"
	    "ced sing vals\002,/\002   9: Geometrically spaced sing vals  "
	    "\002,\00213: Random, O(1) entries\002,/\002  10: Clustered sing."
	    " vals.\002,11x,\00214: Random, scaled near overflow\002,/\002  1"
	    "1: Large, evenly spaced sing vals  \002,\00215: Random, scaled n"
	    "ear underflow\002)";
    static char fmt_9972[] = "(/\002 Test ratios:  \002,\002(B: bidiagonal, "
	    "S: diagonal, Q, P, U, and V: \002,a10,/16x,\002X: m x nrhs, Y = "
	    "Q' X, and Z = U' Y)\002,/\002   1: norm( A - Q B P' ) / ( norm(A"
	    ") max(m,n) ulp )\002,/\002   2: norm( I - Q' Q )   / ( m ulp "
	    ")\002,/\002   3: norm( I - P' P )   / ( n ulp )\002,/\002   4: n"
	    "orm( B - U S V' ) / ( norm(B) min(m,n) ulp )\002,/\002   5: norm"
	    "( Y - U Z )    / ( norm(Z) max(min(m,n),k) ulp )\002,/\002   6: "
	    "norm( I - U' U )   / ( min(m,n) ulp )\002,/\002   7: norm( I - V"
	    "' V )   / ( min(m,n) ulp )\002)";
    static char fmt_9971[] = "(\002   8: Test ordering of S  (0 if nondecrea"
	    "sing, 1/ulp \002,\002 otherwise)\002,/\002   9: norm( S - S2 )  "
	    "   / ( norm(S) ulp ),\002,\002 where S2 is computed\002,/44x,"
	    "\002without computing U and V'\002,/\002  10: Sturm sequence tes"
	    "t \002,\002(0 if sing. vals of B within THRESH of S)\002,/\002  "
	    "11: norm( A - (QU) S (V' P') ) / \002,\002( norm(A) max(m,n) ulp"
	    " )\002,/\002  12: norm( X - (QU) Z )         / ( |X| max(M,k) ul"
	    "p )\002,/\002  13: norm( I - (QU)'(QU) )      / ( M ulp )\002,"
	    "/\002  14: norm( I - (V' P') (P V) )  / ( N ulp )\002)";
    static char fmt_9993[] = "(/1x,a3,\002 -- Complex Singular Value Decompo"
	    "sition\002)";
    static char fmt_9990[] = "(/1x,a3,\002 -- Real Band reduc. to bidiagonal"
	    " form\002)";
    static char fmt_9970[] = "(\002 Matrix types (see xCHKBB for details)"
	    ":\002,/\002 Diagonal matrices:\002,/\002   1: Zero\002,28x,\002 "
	    "5: Clustered entries\002,/\002   2: Identity\002,24x,\002 6: Lar"
	    "ge, evenly spaced entries\002,/\002   3: Evenly spaced entrie"
	    "s\002,11x,\002 7: Small, evenly spaced entries\002,/\002   4: Ge"
	    "ometrically spaced entries\002,/\002 General matrices:\002,/\002"
	    "   8: Evenly spaced sing. vals.\002,7x,\00212: Small, evenly spa"
	    "ced sing vals\002,/\002   9: Geometrically spaced sing vals  "
	    "\002,\00213: Random, O(1) entries\002,/\002  10: Clustered sing."
	    " vals.\002,11x,\00214: Random, scaled near overflow\002,/\002  1"
	    "1: Large, evenly spaced sing vals  \002,\00215: Random, scaled n"
	    "ear underflow\002)";
    static char fmt_9969[] = "(/\002 Test ratios:  \002,\002(B: upper bidiag"
	    "onal, Q and P: \002,a10,/16x,\002C: m x nrhs, PT = P', Y = Q' C"
	    ")\002,/\002 1: norm( A - Q B PT ) / ( norm(A) max(m,n) ulp )\002"
	    ",/\002 2: norm( I - Q' Q )   / ( m ulp )\002,/\002 3: norm( I - "
	    "PT PT' )   / ( n ulp )\002,/\002 4: norm( Y - Q' C )   / ( norm("
	    "Y) max(m,nrhs) ulp )\002)";
    static char fmt_9989[] = "(/1x,a3,\002 -- Complex Band reduc. to bidiago"
	    "nal form\002)";

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    integer j;
    char c2[2];
    logical sord, corz;
    extern logical lsame_(char *, char *), lsamen_(integer *, 
	    char *, char *);

    /* Fortran I/O blocks */
    static cilist io___3 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___5 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___6 = { 0, 0, 0, fmt_9988, 0 };
    static cilist io___7 = { 0, 0, 0, fmt_9987, 0 };
    static cilist io___8 = { 0, 0, 0, fmt_9986, 0 };
    static cilist io___9 = { 0, 0, 0, fmt_9985, 0 };
    static cilist io___10 = { 0, 0, 0, fmt_9984, 0 };
    static cilist io___12 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___13 = { 0, 0, 0, fmt_9988, 0 };
    static cilist io___14 = { 0, 0, 0, fmt_9987, 0 };
    static cilist io___15 = { 0, 0, 0, fmt_9986, 0 };
    static cilist io___16 = { 0, 0, 0, fmt_9985, 0 };
    static cilist io___17 = { 0, 0, 0, fmt_9984, 0 };
    static cilist io___18 = { 0, 0, 0, fmt_9996, 0 };
    static cilist io___19 = { 0, 0, 0, fmt_9983, 0 };
    static cilist io___20 = { 0, 0, 0, fmt_9982, 0 };
    static cilist io___21 = { 0, 0, 0, fmt_9981, 0 };
    static cilist io___22 = { 0, 0, 0, fmt_9968, 0 };
    static cilist io___23 = { 0, 0, 0, fmt_9995, 0 };
    static cilist io___24 = { 0, 0, 0, fmt_9983, 0 };
    static cilist io___25 = { 0, 0, 0, fmt_9982, 0 };
    static cilist io___26 = { 0, 0, 0, fmt_9981, 0 };
    static cilist io___27 = { 0, 0, 0, fmt_9967, 0 };
    static cilist io___28 = { 0, 0, 0, fmt_9992, 0 };
    static cilist io___29 = { 0, 0, 0, fmt_9980, 0 };
    static cilist io___30 = { 0, 0, 0, fmt_9979, 0 };
    static cilist io___31 = { 0, 0, 0, fmt_9978, 0 };
    static cilist io___32 = { 0, 0, 0, fmt_9977, 0 };
    static cilist io___33 = { 0, 0, 0, fmt_9976, 0 };
    static cilist io___34 = { 0, 0, 0, fmt_9991, 0 };
    static cilist io___35 = { 0, 0, 0, fmt_9980, 0 };
    static cilist io___36 = { 0, 0, 0, fmt_9979, 0 };
    static cilist io___37 = { 0, 0, 0, fmt_9978, 0 };
    static cilist io___38 = { 0, 0, 0, fmt_9975, 0 };
    static cilist io___39 = { 0, 0, 0, fmt_9974, 0 };
    static cilist io___40 = { 0, 0, 0, fmt_9994, 0 };
    static cilist io___41 = { 0, 0, 0, fmt_9973, 0 };
    static cilist io___42 = { 0, 0, 0, fmt_9972, 0 };
    static cilist io___43 = { 0, 0, 0, fmt_9971, 0 };
    static cilist io___44 = { 0, 0, 0, fmt_9993, 0 };
    static cilist io___45 = { 0, 0, 0, fmt_9973, 0 };
    static cilist io___46 = { 0, 0, 0, fmt_9972, 0 };
    static cilist io___47 = { 0, 0, 0, fmt_9971, 0 };
    static cilist io___48 = { 0, 0, 0, fmt_9990, 0 };
    static cilist io___49 = { 0, 0, 0, fmt_9970, 0 };
    static cilist io___50 = { 0, 0, 0, fmt_9969, 0 };
    static cilist io___51 = { 0, 0, 0, fmt_9989, 0 };
    static cilist io___52 = { 0, 0, 0, fmt_9970, 0 };
    static cilist io___53 = { 0, 0, 0, fmt_9969, 0 };
    static cilist io___54 = { 0, 0, 0, fmt_9999, 0 };



/*  -- LAPACK auxiliary test routine (version 2.0) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAHD2 prints header information for the different test paths. */

/*  Arguments */
/*  ========= */

/*  IOUNIT  (input) INTEGER. */
/*          On entry, IOUNIT specifies the unit number to which the */
/*          header information should be printed. */

/*  PATH    (input) CHARACTER*3. */
/*          On entry, PATH contains the name of the path for which the */
/*          header information is to be printed.  Current paths are */

/*             DHS, ZHS:  Non-symmetric eigenproblem. */
/*             DST, ZST:  Symmetric eigenproblem. */
/*             DSG, ZSG:  Symmetric Generalized eigenproblem. */
/*             DBD, ZBD:  Singular Value Decomposition (SVD) */
/*             DBB, ZBB:  General Banded reduction to bidiagonal form */

/*          These paths also are supplied in double precision (replace */
/*          leading S by D and leading C by Z in path names). */

/*  ===================================================================== */

/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    if (*iounit <= 0) {
	return 0;
    }
    sord = lsame_(path, "S") || lsame_(path, "D");
    corz = lsame_(path, "C") || lsame_(path, "Z");
    if (! sord && ! corz) {
	io___3.ciunit = *iounit;
	s_wsfe(&io___3);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
    }
    s_copy(c2, path + 1, (ftnlen)2, (ftnlen)2);

    if (lsamen_(&c__2, c2, "HS")) {
	if (sord) {

/*           Real Non-symmetric Eigenvalue Problem: */

	    io___5.ciunit = *iounit;
	    s_wsfe(&io___5);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();

/*           Matrix types */

	    io___6.ciunit = *iounit;
	    s_wsfe(&io___6);
	    e_wsfe();
	    io___7.ciunit = *iounit;
	    s_wsfe(&io___7);
	    e_wsfe();
	    io___8.ciunit = *iounit;
	    s_wsfe(&io___8);
	    do_fio(&c__1, "pairs ", (ftnlen)6);
	    do_fio(&c__1, "pairs ", (ftnlen)6);
	    do_fio(&c__1, "prs.", (ftnlen)4);
	    do_fio(&c__1, "prs.", (ftnlen)4);
	    e_wsfe();
	    io___9.ciunit = *iounit;
	    s_wsfe(&io___9);
	    e_wsfe();

/*           Tests performed */

	    io___10.ciunit = *iounit;
	    s_wsfe(&io___10);
	    do_fio(&c__1, "orthogonal", (ftnlen)10);
	    do_fio(&c__1, "'=transpose", (ftnlen)11);
	    for (j = 1; j <= 6; ++j) {
		do_fio(&c__1, "'", (ftnlen)1);
	    }
	    e_wsfe();

	} else {

/*           Complex Non-symmetric Eigenvalue Problem: */

	    io___12.ciunit = *iounit;
	    s_wsfe(&io___12);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();

/*           Matrix types */

	    io___13.ciunit = *iounit;
	    s_wsfe(&io___13);
	    e_wsfe();
	    io___14.ciunit = *iounit;
	    s_wsfe(&io___14);
	    e_wsfe();
	    io___15.ciunit = *iounit;
	    s_wsfe(&io___15);
	    do_fio(&c__1, "e.vals", (ftnlen)6);
	    do_fio(&c__1, "e.vals", (ftnlen)6);
	    do_fio(&c__1, "e.vs", (ftnlen)4);
	    do_fio(&c__1, "e.vs", (ftnlen)4);
	    e_wsfe();
	    io___16.ciunit = *iounit;
	    s_wsfe(&io___16);
	    e_wsfe();

/*           Tests performed */

	    io___17.ciunit = *iounit;
	    s_wsfe(&io___17);
	    do_fio(&c__1, "unitary", (ftnlen)7);
	    do_fio(&c__1, "*=conj.transp.", (ftnlen)14);
	    for (j = 1; j <= 6; ++j) {
		do_fio(&c__1, "*", (ftnlen)1);
	    }
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, c2, "ST")) {

	if (sord) {

/*           Real Symmetric Eigenvalue Problem: */

	    io___18.ciunit = *iounit;
	    s_wsfe(&io___18);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();

/*           Matrix types */

	    io___19.ciunit = *iounit;
	    s_wsfe(&io___19);
	    e_wsfe();
	    io___20.ciunit = *iounit;
	    s_wsfe(&io___20);
	    e_wsfe();
	    io___21.ciunit = *iounit;
	    s_wsfe(&io___21);
	    do_fio(&c__1, "Symmetric", (ftnlen)9);
	    e_wsfe();

/*           Tests performed */

	    io___22.ciunit = *iounit;
	    s_wsfe(&io___22);
	    e_wsfe();

	} else {

/*           Complex Hermitian Eigenvalue Problem: */

	    io___23.ciunit = *iounit;
	    s_wsfe(&io___23);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();

/*           Matrix types */

	    io___24.ciunit = *iounit;
	    s_wsfe(&io___24);
	    e_wsfe();
	    io___25.ciunit = *iounit;
	    s_wsfe(&io___25);
	    e_wsfe();
	    io___26.ciunit = *iounit;
	    s_wsfe(&io___26);
	    do_fio(&c__1, "Hermitian", (ftnlen)9);
	    e_wsfe();

/*           Tests performed */

	    io___27.ciunit = *iounit;
	    s_wsfe(&io___27);
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, c2, "SG")) {

	if (sord) {

/*           Real Symmetric Generalized Eigenvalue Problem: */

	    io___28.ciunit = *iounit;
	    s_wsfe(&io___28);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();

/*           Matrix types */

	    io___29.ciunit = *iounit;
	    s_wsfe(&io___29);
	    e_wsfe();
	    io___30.ciunit = *iounit;
	    s_wsfe(&io___30);
	    e_wsfe();
	    io___31.ciunit = *iounit;
	    s_wsfe(&io___31);
	    do_fio(&c__1, "Symmetric", (ftnlen)9);
	    e_wsfe();

/*           Tests performed */

	    io___32.ciunit = *iounit;
	    s_wsfe(&io___32);
	    e_wsfe();
	    io___33.ciunit = *iounit;
	    s_wsfe(&io___33);
	    e_wsfe();

	} else {

/*           Complex Hermitian Generalized Eigenvalue Problem: */

	    io___34.ciunit = *iounit;
	    s_wsfe(&io___34);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();

/*           Matrix types */

	    io___35.ciunit = *iounit;
	    s_wsfe(&io___35);
	    e_wsfe();
	    io___36.ciunit = *iounit;
	    s_wsfe(&io___36);
	    e_wsfe();
	    io___37.ciunit = *iounit;
	    s_wsfe(&io___37);
	    do_fio(&c__1, "Hermitian", (ftnlen)9);
	    e_wsfe();

/*           Tests performed */

	    io___38.ciunit = *iounit;
	    s_wsfe(&io___38);
	    e_wsfe();
	    io___39.ciunit = *iounit;
	    s_wsfe(&io___39);
	    e_wsfe();

	}

    } else if (lsamen_(&c__2, c2, "BD")) {

	if (sord) {

/*           Real Singular Value Decomposition: */

	    io___40.ciunit = *iounit;
	    s_wsfe(&io___40);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();

/*           Matrix types */

	    io___41.ciunit = *iounit;
	    s_wsfe(&io___41);
	    e_wsfe();

/*           Tests performed */

	    io___42.ciunit = *iounit;
	    s_wsfe(&io___42);
	    do_fio(&c__1, "orthogonal", (ftnlen)10);
	    e_wsfe();
	    io___43.ciunit = *iounit;
	    s_wsfe(&io___43);
	    e_wsfe();
	} else {

/*           Complex Singular Value Decomposition: */

	    io___44.ciunit = *iounit;
	    s_wsfe(&io___44);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();

/*           Matrix types */

	    io___45.ciunit = *iounit;
	    s_wsfe(&io___45);
	    e_wsfe();

/*           Tests performed */

	    io___46.ciunit = *iounit;
	    s_wsfe(&io___46);
	    do_fio(&c__1, "unitary   ", (ftnlen)10);
	    e_wsfe();
	    io___47.ciunit = *iounit;
	    s_wsfe(&io___47);
	    e_wsfe();
	}

    } else if (lsamen_(&c__2, c2, "BB")) {

	if (sord) {

/*           Real General Band reduction to bidiagonal form: */

	    io___48.ciunit = *iounit;
	    s_wsfe(&io___48);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();

/*           Matrix types */

	    io___49.ciunit = *iounit;
	    s_wsfe(&io___49);
	    e_wsfe();

/*           Tests performed */

	    io___50.ciunit = *iounit;
	    s_wsfe(&io___50);
	    do_fio(&c__1, "orthogonal", (ftnlen)10);
	    e_wsfe();
	} else {

/*           Complex Band reduction to bidiagonal form: */

	    io___51.ciunit = *iounit;
	    s_wsfe(&io___51);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();

/*           Matrix types */

	    io___52.ciunit = *iounit;
	    s_wsfe(&io___52);
	    e_wsfe();

/*           Tests performed */

	    io___53.ciunit = *iounit;
	    s_wsfe(&io___53);
	    do_fio(&c__1, "unitary   ", (ftnlen)10);
	    e_wsfe();
	}

    } else {

	io___54.ciunit = *iounit;
	s_wsfe(&io___54);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
	return 0;
    }

    return 0;




/*     Symmetric/Hermitian eigenproblem */



/*     Symmetric/Hermitian Generalized eigenproblem */



/*     Singular Value Decomposition */



/*     Band reduction to bidiagonal form */



/*     End of DLAHD2 */

} /* dlahd2_ */
