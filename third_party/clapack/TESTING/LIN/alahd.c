#include "f2c.h"
#include "blaswrap.h"

/* Table of constant values */

static integer c__2 = 2;
static integer c__1 = 1;
static integer c__3 = 3;
static integer c__4 = 4;
static integer c__5 = 5;
static integer c__6 = 6;
static integer c__7 = 7;
static integer c__8 = 8;

/* Subroutine */ int alahd_(integer *iounit, char *path)
{
    /* Format strings */
    static char fmt_9999[] = "(/1x,a3,\002:  General dense matrices\002)";
    static char fmt_9979[] = "(4x,\0021. Diagonal\002,24x,\0027. Last n/2 co"
	    "lumns zero\002,/4x,\0022. Upper triangular\002,16x,\0028. Random"
	    ", CNDNUM = sqrt(0.1/EPS)\002,/4x,\0023. Lower triangular\002,16x,"
	    "\0029. Random, CNDNUM = 0.1/EPS\002,/4x,\0024. Random, CNDNUM = 2"
	    "\002,13x,\00210. Scaled near underflow\002,/4x,\0025. First colu"
	    "mn zero\002,14x,\00211. Scaled near overflow\002,/4x,\0026. Last"
	    " column zero\002)";
    static char fmt_9962[] = "(3x,i2,\002: norm( L * U - A )  / ( N * norm(A"
	    ") * EPS )\002)";
    static char fmt_9961[] = "(3x,i2,\002: norm( I - A*AINV ) / \002,\002( N"
	    " * norm(A) * norm(AINV) * EPS )\002)";
    static char fmt_9960[] = "(3x,i2,\002: norm( B - A * X )  / \002,\002( n"
	    "orm(A) * norm(X) * EPS )\002)";
    static char fmt_9959[] = "(3x,i2,\002: norm( X - XACT )   / \002,\002( n"
	    "orm(XACT) * CNDNUM * EPS )\002)";
    static char fmt_9958[] = "(3x,i2,\002: norm( X - XACT )   / \002,\002( n"
	    "orm(XACT) * CNDNUM * EPS ), refined\002)";
    static char fmt_9957[] = "(3x,i2,\002: norm( X - XACT )   / \002,\002( n"
	    "orm(XACT) * (error bound) )\002)";
    static char fmt_9956[] = "(3x,i2,\002: (backward error)   / EPS\002)";
    static char fmt_9955[] = "(3x,i2,\002: RCOND * CNDNUM - 1.0\002)";
    static char fmt_9998[] = "(/1x,a3,\002:  General band matrices\002)";
    static char fmt_9978[] = "(4x,\0021. Random, CNDNUM = 2\002,14x,\0025. R"
	    "andom, CNDNUM = sqrt(0.1/EPS)\002,/4x,\0022. First column zer"
	    "o\002,15x,\0026. Random, CNDNUM = .01/EPS\002,/4x,\0023. Last co"
	    "lumn zero\002,16x,\0027. Scaled near underflow\002,/4x,\0024. La"
	    "st n/2 columns zero\002,11x,\0028. Scaled near overflow\002)";
    static char fmt_9997[] = "(/1x,a3,\002:  General tridiagonal\002)";
    static char fmt_9977[] = "(\002 Matrix types (1-6 have specified conditi"
	    "on numbers):\002,/4x,\0021. Diagonal\002,24x,\0027. Random, unsp"
	    "ecified CNDNUM\002,/4x,\0022. Random, CNDNUM = 2\002,14x,\0028. "
	    "First column zero\002,/4x,\0023. Random, CNDNUM = sqrt(0.1/EPS"
	    ")\002,2x,\0029. Last column zero\002,/4x,\0024. Random, CNDNUM ="
	    " 0.1/EPS\002,7x,\00210. Last n/2 columns zero\002,/4x,\0025. Sca"
	    "led near underflow\002,10x,\00211. Scaled near underflow\002,/4x,"
	    "\0026. Scaled near overflow\002,11x,\00212. Scaled near overflo"
	    "w\002)";
    static char fmt_9996[] = "(/1x,a3,\002:  \002,a9,\002 positive definite "
	    "matrices\002)";
    static char fmt_9995[] = "(/1x,a3,\002:  \002,a9,\002 positive definite "
	    "packed matrices\002)";
    static char fmt_9975[] = "(4x,\0021. Diagonal\002,24x,\0026. Random, CND"
	    "NUM = sqrt(0.1/EPS)\002,/4x,\0022. Random, CNDNUM = 2\002,14x"
	    ",\0027. Random, CNDNUM = 0.1/EPS\002,/3x,\002*3. First row and c"
	    "olumn zero\002,7x,\0028. Scaled near underflow\002,/3x,\002*4. L"
	    "ast row and column zero\002,8x,\0029. Scaled near overflow\002,/"
	    "3x,\002*5. Middle row and column zero\002,/3x,\002(* - tests err"
	    "or exits from \002,a3,\002TRF, no test ratios are computed)\002)";
    static char fmt_9954[] = "(3x,i2,\002: norm( U' * U - A ) / ( N * norm(A"
	    ") * EPS )\002,\002, or\002,/7x,\002norm( L * L' - A ) / ( N * no"
	    "rm(A) * EPS )\002)";
    static char fmt_9994[] = "(/1x,a3,\002:  \002,a9,\002 positive definite "
	    "band matrices\002)";
    static char fmt_9973[] = "(4x,\0021. Random, CNDNUM = 2\002,14x,\0025. R"
	    "andom, CNDNUM = sqrt(0.1/EPS)\002,/3x,\002*2. First row and colu"
	    "mn zero\002,7x,\0026. Random, CNDNUM = 0.1/EPS\002,/3x,\002*3. L"
	    "ast row and column zero\002,8x,\0027. Scaled near underflow\002,"
	    "/3x,\002*4. Middle row and column zero\002,6x,\0028. Scaled near"
	    " overflow\002,/3x,\002(* - tests error exits from \002,a3,\002TR"
	    "F, no test ratios are computed)\002)";
    static char fmt_9993[] = "(/1x,a3,\002:  \002,a9,\002 positive definite "
	    "tridiagonal\002)";
    static char fmt_9976[] = "(\002 Matrix types (1-6 have specified conditi"
	    "on numbers):\002,/4x,\0021. Diagonal\002,24x,\0027. Random, unsp"
	    "ecified CNDNUM\002,/4x,\0022. Random, CNDNUM = 2\002,14x,\0028. "
	    "First row and column zero\002,/4x,\0023. Random, CNDNUM = sqrt(0"
	    ".1/EPS)\002,2x,\0029. Last row and column zero\002,/4x,\0024. Ra"
	    "ndom, CNDNUM = 0.1/EPS\002,7x,\00210. Middle row and column zer"
	    "o\002,/4x,\0025. Scaled near underflow\002,10x,\00211. Scaled ne"
	    "ar underflow\002,/4x,\0026. Scaled near overflow\002,11x,\00212."
	    " Scaled near overflow\002)";
    static char fmt_9952[] = "(3x,i2,\002: norm( U'*D*U - A ) / ( N * norm(A"
	    ") * EPS )\002,\002, or\002,/7x,\002norm( L*D*L' - A ) / ( N * no"
	    "rm(A) * EPS )\002)";
    static char fmt_9992[] = "(/1x,a3,\002:  \002,a9,\002 indefinite matri"
	    "ces\002)";
    static char fmt_9991[] = "(/1x,a3,\002:  \002,a9,\002 indefinite packed "
	    "matrices\002)";
    static char fmt_9972[] = "(4x,\0021. Diagonal\002,24x,\0026. Last n/2 ro"
	    "ws and columns zero\002,/4x,\0022. Random, CNDNUM = 2\002,14x"
	    ",\0027. Random, CNDNUM = sqrt(0.1/EPS)\002,/4x,\0023. First row "
	    "and column zero\002,7x,\0028. Random, CNDNUM = 0.1/EPS\002,/4x"
	    ",\0024. Last row and column zero\002,8x,\0029. Scaled near under"
	    "flow\002,/4x,\0025. Middle row and column zero\002,5x,\00210. Sc"
	    "aled near overflow\002)";
    static char fmt_9971[] = "(4x,\0021. Diagonal\002,24x,\0027. Random, CND"
	    "NUM = sqrt(0.1/EPS)\002,/4x,\0022. Random, CNDNUM = 2\002,14x"
	    ",\0028. Random, CNDNUM = 0.1/EPS\002,/4x,\0023. First row and co"
	    "lumn zero\002,7x,\0029. Scaled near underflow\002,/4x,\0024. Las"
	    "t row and column zero\002,7x,\00210. Scaled near overflow\002,/4"
	    "x,\0025. Middle row and column zero\002,5x,\00211. Block diagona"
	    "l matrix\002,/4x,\0026. Last n/2 rows and columns zero\002)";
    static char fmt_9953[] = "(3x,i2,\002: norm( U*D*U' - A ) / ( N * norm(A"
	    ") * EPS )\002,\002, or\002,/7x,\002norm( L*D*L' - A ) / ( N * no"
	    "rm(A) * EPS )\002)";
    static char fmt_9990[] = "(/1x,a3,\002:  Triangular matrices\002)";
    static char fmt_9989[] = "(/1x,a3,\002:  Triangular packed matrices\002)";
    static char fmt_9966[] = "(\002 Matrix types for \002,a3,\002 routines"
	    ":\002,/4x,\0021. Diagonal\002,24x,\0026. Scaled near overflow"
	    "\002,/4x,\0022. Random, CNDNUM = 2\002,14x,\0027. Identity\002,/"
	    "4x,\0023. Random, CNDNUM = sqrt(0.1/EPS)  \002,\0028. Unit trian"
	    "gular, CNDNUM = 2\002,/4x,\0024. Random, CNDNUM = 0.1/EPS\002,8x,"
	    "\0029. Unit, CNDNUM = sqrt(0.1/EPS)\002,/4x,\0025. Scaled near u"
	    "nderflow\002,10x,\00210. Unit, CNDNUM = 0.1/EPS\002)";
    static char fmt_9965[] = "(\002 Special types for testing \002,a6,\002"
	    ":\002,/3x,\00211. Matrix elements are O(1), large right hand side"
	    "\002,/3x,\00212. First diagonal causes overflow,\002,\002 offdia"
	    "gonal column norms < 1\002,/3x,\00213. First diagonal causes ove"
	    "rflow,\002,\002 offdiagonal column norms > 1\002,/3x,\00214. Gro"
	    "wth factor underflows, solution does not overflow\002,/3x,\00215"
	    ". Small diagonal causes gradual overflow\002,/3x,\00216. One zer"
	    "o diagonal element\002,/3x,\00217. Large offdiagonals cause over"
	    "flow when adding a column\002,/3x,\00218. Unit triangular with l"
	    "arge right hand side\002)";
    static char fmt_9951[] = "(\002 Test ratio for \002,a6,\002:\002,/3x,i2"
	    ",\002: norm( s*b - A*x )  / ( norm(A) * norm(x) * EPS )\002)";
    static char fmt_9988[] = "(/1x,a3,\002:  Triangular band matrices\002)";
    static char fmt_9964[] = "(\002 Matrix types for \002,a3,\002 routines"
	    ":\002,/4x,\0021. Random, CNDNUM = 2\002,14x,\0026. Identity\002,"
	    "/4x,\0022. Random, CNDNUM = sqrt(0.1/EPS)  \002,\0027. Unit tria"
	    "ngular, CNDNUM = 2\002,/4x,\0023. Random, CNDNUM = 0.1/EPS\002,8"
	    "x,\0028. Unit, CNDNUM = sqrt(0.1/EPS)\002,/4x,\0024. Scaled near"
	    " underflow\002,11x,\0029. Unit, CNDNUM = 0.1/EPS\002,/4x,\0025. "
	    "Scaled near overflow\002)";
    static char fmt_9963[] = "(\002 Special types for testing \002,a6,\002"
	    ":\002,/3x,\00210. Matrix elements are O(1), large right hand side"
	    "\002,/3x,\00211. First diagonal causes overflow,\002,\002 offdia"
	    "gonal column norms < 1\002,/3x,\00212. First diagonal causes ove"
	    "rflow,\002,\002 offdiagonal column norms > 1\002,/3x,\00213. Gro"
	    "wth factor underflows, solution does not overflow\002,/3x,\00214"
	    ". Small diagonal causes gradual overflow\002,/3x,\00215. One zer"
	    "o diagonal element\002,/3x,\00216. Large offdiagonals cause over"
	    "flow when adding a column\002,/3x,\00217. Unit triangular with l"
	    "arge right hand side\002)";
    static char fmt_9987[] = "(/1x,a3,\002:  \002,a2,\002 factorization of g"
	    "eneral matrices\002)";
    static char fmt_9970[] = "(4x,\0021. Diagonal\002,24x,\0025. Random, CND"
	    "NUM = sqrt(0.1/EPS)\002,/4x,\0022. Upper triangular\002,16x,\002"
	    "6. Random, CNDNUM = 0.1/EPS\002,/4x,\0023. Lower triangular\002,"
	    "16x,\0027. Scaled near underflow\002,/4x,\0024. Random, CNDNUM ="
	    " 2\002,14x,\0028. Scaled near overflow\002)";
    static char fmt_9950[] = "(3x,i2,\002: norm( R - Q' * A ) / ( M * norm(A"
	    ") * EPS )\002)";
    static char fmt_9946[] = "(3x,i2,\002: norm( I - Q'*Q )   / ( M * EPS "
	    ")\002)";
    static char fmt_9944[] = "(3x,i2,\002: norm( Q*C - Q*C )  / \002,\002("
	    " \002,a1,\002 * norm(C) * EPS )\002)";
    static char fmt_9943[] = "(3x,i2,\002: norm( C*Q - C*Q )  / \002,\002("
	    " \002,a1,\002 * norm(C) * EPS )\002)";
    static char fmt_9942[] = "(3x,i2,\002: norm( Q'*C - Q'*C )/ \002,\002("
	    " \002,a1,\002 * norm(C) * EPS )\002)";
    static char fmt_9941[] = "(3x,i2,\002: norm( C*Q' - C*Q' )/ \002,\002("
	    " \002,a1,\002 * norm(C) * EPS )\002)";
    static char fmt_9949[] = "(3x,i2,\002: norm( L - A * Q' ) / ( N * norm(A"
	    ") * EPS )\002)";
    static char fmt_9945[] = "(3x,i2,\002: norm( I - Q*Q' )   / ( N * EPS "
	    ")\002)";
    static char fmt_9948[] = "(3x,i2,\002: norm( L - Q' * A ) / ( M * norm(A"
	    ") * EPS )\002)";
    static char fmt_9947[] = "(3x,i2,\002: norm( R - A * Q' ) / ( N * norm(A"
	    ") * EPS )\002)";
    static char fmt_9986[] = "(/1x,a3,\002:  QR factorization with column pi"
	    "voting\002)";
    static char fmt_9969[] = "(\002 Matrix types (2-6 have condition 1/EPS)"
	    ":\002,/4x,\0021. Zero matrix\002,21x,\0024. First n/2 columns fi"
	    "xed\002,/4x,\0022. One small eigenvalue\002,12x,\0025. Last n/2 "
	    "columns fixed\002,/4x,\0023. Geometric distribution\002,10x,\002"
	    "6. Every second column fixed\002)";
    static char fmt_9940[] = "(3x,i2,\002: norm(svd(A) - svd(R)) / \002,\002"
	    "( M * norm(svd(R)) * EPS )\002)";
    static char fmt_9939[] = "(3x,i2,\002: norm( A*P - Q*R )     / ( M * nor"
	    "m(A) * EPS )\002)";
    static char fmt_9938[] = "(3x,i2,\002: norm( I - Q'*Q )      / ( M * EPS"
	    " )\002)";
    static char fmt_9985[] = "(/1x,a3,\002:  RQ factorization of trapezoidal"
	    " matrix\002)";
    static char fmt_9968[] = "(\002 Matrix types (2-3 have condition 1/EPS)"
	    ":\002,/4x,\0021. Zero matrix\002,/4x,\0022. One small eigenvalu"
	    "e\002,/4x,\0023. Geometric distribution\002)";
    static char fmt_9929[] = "(\002 Test ratios (1-3: \002,a1,\002TZRQF, 4-6"
	    ": \002,a1,\002TZRZF):\002)";
    static char fmt_9937[] = "(3x,i2,\002: norm( A - R*Q )       / ( M * nor"
	    "m(A) * EPS )\002)";
    static char fmt_9984[] = "(/1x,a3,\002:  Least squares driver routine"
	    "s\002)";
    static char fmt_9967[] = "(\002 Matrix types (1-3: full rank, 4-6: rank "
	    "deficient):\002,/4x,\0021 and 4. Normal scaling\002,/4x,\0022 an"
	    "d 5. Scaled near overflow\002,/4x,\0023 and 6. Scaled near under"
	    "flow\002)";
    static char fmt_9921[] = "(\002 Test ratios:\002,/\002    (1-2: \002,a1"
	    ",\002GELS, 3-6: \002,a1,\002GELSX, 7-10: \002,a1,\002GELSY, 11-1"
	    "4: \002,a1,\002GELSS, 15-18: \002,a1,\002GELSD)\002)";
    static char fmt_9935[] = "(3x,i2,\002: norm( B - A * X )   / \002,\002( "
	    "max(M,N) * norm(A) * norm(X) * EPS )\002)";
    static char fmt_9931[] = "(3x,i2,\002: norm( (A*X-B)' *A ) / \002,\002( "
	    "max(M,N,NRHS) * norm(A) * norm(B) * EPS )\002,/7x,\002if TRANS='"
	    "N' and M.GE.N or TRANS='T' and M.LT.N, \002,\002otherwise\002,/7"
	    "x,\002check if X is in the row space of A or A' \002,\002(overde"
	    "termined case)\002)";
    static char fmt_9933[] = "(3x,i2,\002: norm(svd(A)-svd(R)) / \002,\002( "
	    "min(M,N) * norm(svd(R)) * EPS )\002)";
    static char fmt_9934[] = "(3x,i2,\002: norm( (A*X-B)' *A ) / \002,\002( "
	    "max(M,N,NRHS) * norm(A) * norm(B) * EPS )\002)";
    static char fmt_9932[] = "(3x,i2,\002: Check if X is in the row space of"
	    " A or A'\002)";
    static char fmt_9920[] = "(3x,\002 7-10: same as 3-6\002,3x,\002 11-14: "
	    "same as 3-6\002,3x,\002 15-18: same as 3-6\002)";
    static char fmt_9983[] = "(/1x,a3,\002:  LU factorization variants\002)";
    static char fmt_9982[] = "(/1x,a3,\002:  Cholesky factorization variant"
	    "s\002)";
    static char fmt_9974[] = "(4x,\0021. Diagonal\002,24x,\0026. Random, CND"
	    "NUM = sqrt(0.1/EPS)\002,/4x,\0022. Random, CNDNUM = 2\002,14x"
	    ",\0027. Random, CNDNUM = 0.1/EPS\002,/3x,\002*3. First row and c"
	    "olumn zero\002,7x,\0028. Scaled near underflow\002,/3x,\002*4. L"
	    "ast row and column zero\002,8x,\0029. Scaled near overflow\002,/"
	    "3x,\002*5. Middle row and column zero\002,/3x,\002(* - tests err"
	    "or exits, no test ratios are computed)\002)";
    static char fmt_9981[] = "(/1x,a3,\002:  QR factorization variants\002)";
    static char fmt_9980[] = "(/1x,a3,\002:  No header available\002)";

    /* System generated locals */
    address a__1[2];
    integer i__1[2];
    cilist ci__1;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);
    /* Subroutine */ int s_cat(char *, char **, integer *, integer *, ftnlen);

    /* Local variables */
    char c1[1], c3[1], p2[2], sym[9];
    logical sord, corz;
    extern logical lsame_(char *, char *), lsamen_(integer *, 
	    char *, char *);
    char subnam[6];

    /* Fortran I/O blocks */
    static cilist io___6 = { 0, 0, 0, fmt_9999, 0 };
    static cilist io___7 = { 0, 0, 0, fmt_9979, 0 };
    static cilist io___8 = { 0, 0, 0, fmt_9962, 0 };
    static cilist io___9 = { 0, 0, 0, fmt_9961, 0 };
    static cilist io___10 = { 0, 0, 0, fmt_9960, 0 };
    static cilist io___11 = { 0, 0, 0, fmt_9959, 0 };
    static cilist io___12 = { 0, 0, 0, fmt_9958, 0 };
    static cilist io___13 = { 0, 0, 0, fmt_9957, 0 };
    static cilist io___14 = { 0, 0, 0, fmt_9956, 0 };
    static cilist io___15 = { 0, 0, 0, fmt_9955, 0 };
    static cilist io___16 = { 0, 0, 0, fmt_9998, 0 };
    static cilist io___17 = { 0, 0, 0, fmt_9978, 0 };
    static cilist io___18 = { 0, 0, 0, fmt_9962, 0 };
    static cilist io___19 = { 0, 0, 0, fmt_9960, 0 };
    static cilist io___20 = { 0, 0, 0, fmt_9959, 0 };
    static cilist io___21 = { 0, 0, 0, fmt_9958, 0 };
    static cilist io___22 = { 0, 0, 0, fmt_9957, 0 };
    static cilist io___23 = { 0, 0, 0, fmt_9956, 0 };
    static cilist io___24 = { 0, 0, 0, fmt_9955, 0 };
    static cilist io___25 = { 0, 0, 0, fmt_9997, 0 };
    static cilist io___26 = { 0, 0, 0, fmt_9977, 0 };
    static cilist io___27 = { 0, 0, 0, fmt_9962, 0 };
    static cilist io___28 = { 0, 0, 0, fmt_9960, 0 };
    static cilist io___29 = { 0, 0, 0, fmt_9959, 0 };
    static cilist io___30 = { 0, 0, 0, fmt_9958, 0 };
    static cilist io___31 = { 0, 0, 0, fmt_9957, 0 };
    static cilist io___32 = { 0, 0, 0, fmt_9956, 0 };
    static cilist io___33 = { 0, 0, 0, fmt_9955, 0 };
    static cilist io___35 = { 0, 0, 0, fmt_9996, 0 };
    static cilist io___36 = { 0, 0, 0, fmt_9995, 0 };
    static cilist io___37 = { 0, 0, 0, fmt_9975, 0 };
    static cilist io___38 = { 0, 0, 0, fmt_9954, 0 };
    static cilist io___39 = { 0, 0, 0, fmt_9961, 0 };
    static cilist io___40 = { 0, 0, 0, fmt_9960, 0 };
    static cilist io___41 = { 0, 0, 0, fmt_9959, 0 };
    static cilist io___42 = { 0, 0, 0, fmt_9958, 0 };
    static cilist io___43 = { 0, 0, 0, fmt_9957, 0 };
    static cilist io___44 = { 0, 0, 0, fmt_9956, 0 };
    static cilist io___45 = { 0, 0, 0, fmt_9955, 0 };
    static cilist io___46 = { 0, 0, 0, fmt_9994, 0 };
    static cilist io___47 = { 0, 0, 0, fmt_9994, 0 };
    static cilist io___48 = { 0, 0, 0, fmt_9973, 0 };
    static cilist io___49 = { 0, 0, 0, fmt_9954, 0 };
    static cilist io___50 = { 0, 0, 0, fmt_9960, 0 };
    static cilist io___51 = { 0, 0, 0, fmt_9959, 0 };
    static cilist io___52 = { 0, 0, 0, fmt_9958, 0 };
    static cilist io___53 = { 0, 0, 0, fmt_9957, 0 };
    static cilist io___54 = { 0, 0, 0, fmt_9956, 0 };
    static cilist io___55 = { 0, 0, 0, fmt_9955, 0 };
    static cilist io___56 = { 0, 0, 0, fmt_9993, 0 };
    static cilist io___57 = { 0, 0, 0, fmt_9993, 0 };
    static cilist io___58 = { 0, 0, 0, fmt_9976, 0 };
    static cilist io___59 = { 0, 0, 0, fmt_9952, 0 };
    static cilist io___60 = { 0, 0, 0, fmt_9960, 0 };
    static cilist io___61 = { 0, 0, 0, fmt_9959, 0 };
    static cilist io___62 = { 0, 0, 0, fmt_9958, 0 };
    static cilist io___63 = { 0, 0, 0, fmt_9957, 0 };
    static cilist io___64 = { 0, 0, 0, fmt_9956, 0 };
    static cilist io___65 = { 0, 0, 0, fmt_9955, 0 };
    static cilist io___66 = { 0, 0, 0, fmt_9992, 0 };
    static cilist io___67 = { 0, 0, 0, fmt_9991, 0 };
    static cilist io___68 = { 0, 0, 0, fmt_9972, 0 };
    static cilist io___69 = { 0, 0, 0, fmt_9971, 0 };
    static cilist io___70 = { 0, 0, 0, fmt_9953, 0 };
    static cilist io___71 = { 0, 0, 0, fmt_9961, 0 };
    static cilist io___72 = { 0, 0, 0, fmt_9960, 0 };
    static cilist io___73 = { 0, 0, 0, fmt_9959, 0 };
    static cilist io___74 = { 0, 0, 0, fmt_9958, 0 };
    static cilist io___75 = { 0, 0, 0, fmt_9956, 0 };
    static cilist io___76 = { 0, 0, 0, fmt_9957, 0 };
    static cilist io___77 = { 0, 0, 0, fmt_9955, 0 };
    static cilist io___78 = { 0, 0, 0, fmt_9992, 0 };
    static cilist io___79 = { 0, 0, 0, fmt_9991, 0 };
    static cilist io___80 = { 0, 0, 0, fmt_9972, 0 };
    static cilist io___81 = { 0, 0, 0, fmt_9953, 0 };
    static cilist io___82 = { 0, 0, 0, fmt_9961, 0 };
    static cilist io___83 = { 0, 0, 0, fmt_9960, 0 };
    static cilist io___84 = { 0, 0, 0, fmt_9959, 0 };
    static cilist io___85 = { 0, 0, 0, fmt_9958, 0 };
    static cilist io___86 = { 0, 0, 0, fmt_9956, 0 };
    static cilist io___87 = { 0, 0, 0, fmt_9957, 0 };
    static cilist io___88 = { 0, 0, 0, fmt_9955, 0 };
    static cilist io___89 = { 0, 0, 0, fmt_9990, 0 };
    static cilist io___91 = { 0, 0, 0, fmt_9989, 0 };
    static cilist io___92 = { 0, 0, 0, fmt_9966, 0 };
    static cilist io___93 = { 0, 0, 0, fmt_9965, 0 };
    static cilist io___94 = { 0, 0, 0, fmt_9961, 0 };
    static cilist io___95 = { 0, 0, 0, fmt_9960, 0 };
    static cilist io___96 = { 0, 0, 0, fmt_9959, 0 };
    static cilist io___97 = { 0, 0, 0, fmt_9958, 0 };
    static cilist io___98 = { 0, 0, 0, fmt_9957, 0 };
    static cilist io___99 = { 0, 0, 0, fmt_9956, 0 };
    static cilist io___100 = { 0, 0, 0, fmt_9955, 0 };
    static cilist io___101 = { 0, 0, 0, fmt_9951, 0 };
    static cilist io___102 = { 0, 0, 0, fmt_9988, 0 };
    static cilist io___103 = { 0, 0, 0, fmt_9964, 0 };
    static cilist io___104 = { 0, 0, 0, fmt_9963, 0 };
    static cilist io___105 = { 0, 0, 0, fmt_9960, 0 };
    static cilist io___106 = { 0, 0, 0, fmt_9959, 0 };
    static cilist io___107 = { 0, 0, 0, fmt_9958, 0 };
    static cilist io___108 = { 0, 0, 0, fmt_9957, 0 };
    static cilist io___109 = { 0, 0, 0, fmt_9956, 0 };
    static cilist io___110 = { 0, 0, 0, fmt_9955, 0 };
    static cilist io___111 = { 0, 0, 0, fmt_9951, 0 };
    static cilist io___112 = { 0, 0, 0, fmt_9987, 0 };
    static cilist io___113 = { 0, 0, 0, fmt_9970, 0 };
    static cilist io___114 = { 0, 0, 0, fmt_9950, 0 };
    static cilist io___115 = { 0, 0, 0, fmt_9946, 0 };
    static cilist io___116 = { 0, 0, 0, fmt_9944, 0 };
    static cilist io___117 = { 0, 0, 0, fmt_9943, 0 };
    static cilist io___118 = { 0, 0, 0, fmt_9942, 0 };
    static cilist io___119 = { 0, 0, 0, fmt_9941, 0 };
    static cilist io___120 = { 0, 0, 0, fmt_9960, 0 };
    static cilist io___121 = { 0, 0, 0, fmt_9987, 0 };
    static cilist io___122 = { 0, 0, 0, fmt_9970, 0 };
    static cilist io___123 = { 0, 0, 0, fmt_9949, 0 };
    static cilist io___124 = { 0, 0, 0, fmt_9945, 0 };
    static cilist io___125 = { 0, 0, 0, fmt_9944, 0 };
    static cilist io___126 = { 0, 0, 0, fmt_9943, 0 };
    static cilist io___127 = { 0, 0, 0, fmt_9942, 0 };
    static cilist io___128 = { 0, 0, 0, fmt_9941, 0 };
    static cilist io___129 = { 0, 0, 0, fmt_9960, 0 };
    static cilist io___130 = { 0, 0, 0, fmt_9987, 0 };
    static cilist io___131 = { 0, 0, 0, fmt_9970, 0 };
    static cilist io___132 = { 0, 0, 0, fmt_9948, 0 };
    static cilist io___133 = { 0, 0, 0, fmt_9946, 0 };
    static cilist io___134 = { 0, 0, 0, fmt_9944, 0 };
    static cilist io___135 = { 0, 0, 0, fmt_9943, 0 };
    static cilist io___136 = { 0, 0, 0, fmt_9942, 0 };
    static cilist io___137 = { 0, 0, 0, fmt_9941, 0 };
    static cilist io___138 = { 0, 0, 0, fmt_9960, 0 };
    static cilist io___139 = { 0, 0, 0, fmt_9987, 0 };
    static cilist io___140 = { 0, 0, 0, fmt_9970, 0 };
    static cilist io___141 = { 0, 0, 0, fmt_9947, 0 };
    static cilist io___142 = { 0, 0, 0, fmt_9945, 0 };
    static cilist io___143 = { 0, 0, 0, fmt_9944, 0 };
    static cilist io___144 = { 0, 0, 0, fmt_9943, 0 };
    static cilist io___145 = { 0, 0, 0, fmt_9942, 0 };
    static cilist io___146 = { 0, 0, 0, fmt_9941, 0 };
    static cilist io___147 = { 0, 0, 0, fmt_9960, 0 };
    static cilist io___148 = { 0, 0, 0, fmt_9986, 0 };
    static cilist io___149 = { 0, 0, 0, fmt_9969, 0 };
    static cilist io___150 = { 0, 0, 0, fmt_9940, 0 };
    static cilist io___151 = { 0, 0, 0, fmt_9939, 0 };
    static cilist io___152 = { 0, 0, 0, fmt_9938, 0 };
    static cilist io___153 = { 0, 0, 0, fmt_9985, 0 };
    static cilist io___154 = { 0, 0, 0, fmt_9968, 0 };
    static cilist io___155 = { 0, 0, 0, fmt_9929, 0 };
    static cilist io___156 = { 0, 0, 0, fmt_9940, 0 };
    static cilist io___157 = { 0, 0, 0, fmt_9937, 0 };
    static cilist io___158 = { 0, 0, 0, fmt_9938, 0 };
    static cilist io___159 = { 0, 0, 0, fmt_9940, 0 };
    static cilist io___160 = { 0, 0, 0, fmt_9937, 0 };
    static cilist io___161 = { 0, 0, 0, fmt_9938, 0 };
    static cilist io___162 = { 0, 0, 0, fmt_9984, 0 };
    static cilist io___163 = { 0, 0, 0, fmt_9967, 0 };
    static cilist io___164 = { 0, 0, 0, fmt_9921, 0 };
    static cilist io___165 = { 0, 0, 0, fmt_9935, 0 };
    static cilist io___166 = { 0, 0, 0, fmt_9931, 0 };
    static cilist io___167 = { 0, 0, 0, fmt_9933, 0 };
    static cilist io___168 = { 0, 0, 0, fmt_9935, 0 };
    static cilist io___169 = { 0, 0, 0, fmt_9934, 0 };
    static cilist io___170 = { 0, 0, 0, fmt_9932, 0 };
    static cilist io___171 = { 0, 0, 0, fmt_9920, 0 };
    static cilist io___172 = { 0, 0, 0, fmt_9983, 0 };
    static cilist io___173 = { 0, 0, 0, fmt_9979, 0 };
    static cilist io___174 = { 0, 0, 0, fmt_9962, 0 };
    static cilist io___175 = { 0, 0, 0, fmt_9982, 0 };
    static cilist io___176 = { 0, 0, 0, fmt_9974, 0 };
    static cilist io___177 = { 0, 0, 0, fmt_9954, 0 };
    static cilist io___178 = { 0, 0, 0, fmt_9981, 0 };
    static cilist io___179 = { 0, 0, 0, fmt_9970, 0 };
    static cilist io___180 = { 0, 0, 0, fmt_9980, 0 };



/*  -- LAPACK test routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ALAHD prints header information for the different test paths. */

/*  Arguments */
/*  ========= */

/*  IOUNIT  (input) INTEGER */
/*          The unit number to which the header information should be */
/*          printed. */

/*  PATH    (input) CHARACTER*3 */
/*          The name of the path for which the header information is to */
/*          be printed.  Current paths are */
/*             _GE:  General matrices */
/*             _GB:  General band */
/*             _GT:  General Tridiagonal */
/*             _PO:  Symmetric or Hermitian positive definite */
/*             _PP:  Symmetric or Hermitian positive definite packed */
/*             _PB:  Symmetric or Hermitian positive definite band */
/*             _PT:  Symmetric or Hermitian positive definite tridiagonal */
/*             _SY:  Symmetric indefinite */
/*             _SP:  Symmetric indefinite packed */
/*             _HE:  (complex) Hermitian indefinite */
/*             _HP:  (complex) Hermitian indefinite packed */
/*             _TR:  Triangular */
/*             _TP:  Triangular packed */
/*             _TB:  Triangular band */
/*             _QR:  QR (general matrices) */
/*             _LQ:  LQ (general matrices) */
/*             _QL:  QL (general matrices) */
/*             _RQ:  RQ (general matrices) */
/*             _QP:  QR with column pivoting */
/*             _TZ:  Trapezoidal */
/*             _LS:  Least Squares driver routines */
/*             _LU:  LU variants */
/*             _CH:  Cholesky variants */
/*             _QS:  QR variants */
/*          The first character must be one of S, D, C, or Z (C or Z only */
/*          if complex). */

/*  ===================================================================== */

/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    if (*iounit <= 0) {
	return 0;
    }
    *(unsigned char *)c1 = *(unsigned char *)path;
    *(unsigned char *)c3 = *(unsigned char *)&path[2];
    s_copy(p2, path + 1, (ftnlen)2, (ftnlen)2);
    sord = lsame_(c1, "S") || lsame_(c1, "D");
    corz = lsame_(c1, "C") || lsame_(c1, "Z");
    if (! (sord || corz)) {
	return 0;
    }

    if (lsamen_(&c__2, p2, "GE")) {

/*        GE: General dense */

	io___6.ciunit = *iounit;
	s_wsfe(&io___6);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Matrix types:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___7.ciunit = *iounit;
	s_wsfe(&io___7);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Test ratios:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___8.ciunit = *iounit;
	s_wsfe(&io___8);
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	io___9.ciunit = *iounit;
	s_wsfe(&io___9);
	do_fio(&c__1, (char *)&c__2, (ftnlen)sizeof(integer));
	e_wsfe();
	io___10.ciunit = *iounit;
	s_wsfe(&io___10);
	do_fio(&c__1, (char *)&c__3, (ftnlen)sizeof(integer));
	e_wsfe();
	io___11.ciunit = *iounit;
	s_wsfe(&io___11);
	do_fio(&c__1, (char *)&c__4, (ftnlen)sizeof(integer));
	e_wsfe();
	io___12.ciunit = *iounit;
	s_wsfe(&io___12);
	do_fio(&c__1, (char *)&c__5, (ftnlen)sizeof(integer));
	e_wsfe();
	io___13.ciunit = *iounit;
	s_wsfe(&io___13);
	do_fio(&c__1, (char *)&c__6, (ftnlen)sizeof(integer));
	e_wsfe();
	io___14.ciunit = *iounit;
	s_wsfe(&io___14);
	do_fio(&c__1, (char *)&c__7, (ftnlen)sizeof(integer));
	e_wsfe();
	io___15.ciunit = *iounit;
	s_wsfe(&io___15);
	do_fio(&c__1, (char *)&c__8, (ftnlen)sizeof(integer));
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Messages:' )";
	s_wsfe(&ci__1);
	e_wsfe();

    } else if (lsamen_(&c__2, p2, "GB")) {

/*        GB: General band */

	io___16.ciunit = *iounit;
	s_wsfe(&io___16);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Matrix types:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___17.ciunit = *iounit;
	s_wsfe(&io___17);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Test ratios:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___18.ciunit = *iounit;
	s_wsfe(&io___18);
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	io___19.ciunit = *iounit;
	s_wsfe(&io___19);
	do_fio(&c__1, (char *)&c__2, (ftnlen)sizeof(integer));
	e_wsfe();
	io___20.ciunit = *iounit;
	s_wsfe(&io___20);
	do_fio(&c__1, (char *)&c__3, (ftnlen)sizeof(integer));
	e_wsfe();
	io___21.ciunit = *iounit;
	s_wsfe(&io___21);
	do_fio(&c__1, (char *)&c__4, (ftnlen)sizeof(integer));
	e_wsfe();
	io___22.ciunit = *iounit;
	s_wsfe(&io___22);
	do_fio(&c__1, (char *)&c__5, (ftnlen)sizeof(integer));
	e_wsfe();
	io___23.ciunit = *iounit;
	s_wsfe(&io___23);
	do_fio(&c__1, (char *)&c__6, (ftnlen)sizeof(integer));
	e_wsfe();
	io___24.ciunit = *iounit;
	s_wsfe(&io___24);
	do_fio(&c__1, (char *)&c__7, (ftnlen)sizeof(integer));
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Messages:' )";
	s_wsfe(&ci__1);
	e_wsfe();

    } else if (lsamen_(&c__2, p2, "GT")) {

/*        GT: General tridiagonal */

	io___25.ciunit = *iounit;
	s_wsfe(&io___25);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
	io___26.ciunit = *iounit;
	s_wsfe(&io___26);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Test ratios:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___27.ciunit = *iounit;
	s_wsfe(&io___27);
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	io___28.ciunit = *iounit;
	s_wsfe(&io___28);
	do_fio(&c__1, (char *)&c__2, (ftnlen)sizeof(integer));
	e_wsfe();
	io___29.ciunit = *iounit;
	s_wsfe(&io___29);
	do_fio(&c__1, (char *)&c__3, (ftnlen)sizeof(integer));
	e_wsfe();
	io___30.ciunit = *iounit;
	s_wsfe(&io___30);
	do_fio(&c__1, (char *)&c__4, (ftnlen)sizeof(integer));
	e_wsfe();
	io___31.ciunit = *iounit;
	s_wsfe(&io___31);
	do_fio(&c__1, (char *)&c__5, (ftnlen)sizeof(integer));
	e_wsfe();
	io___32.ciunit = *iounit;
	s_wsfe(&io___32);
	do_fio(&c__1, (char *)&c__6, (ftnlen)sizeof(integer));
	e_wsfe();
	io___33.ciunit = *iounit;
	s_wsfe(&io___33);
	do_fio(&c__1, (char *)&c__7, (ftnlen)sizeof(integer));
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Messages:' )";
	s_wsfe(&ci__1);
	e_wsfe();

    } else if (lsamen_(&c__2, p2, "PO") || lsamen_(&
	    c__2, p2, "PP")) {

/*        PO: Positive definite full */
/*        PP: Positive definite packed */

	if (sord) {
	    s_copy(sym, "Symmetric", (ftnlen)9, (ftnlen)9);
	} else {
	    s_copy(sym, "Hermitian", (ftnlen)9, (ftnlen)9);
	}
	if (lsame_(c3, "O")) {
	    io___35.ciunit = *iounit;
	    s_wsfe(&io___35);
	    do_fio(&c__1, path, (ftnlen)3);
	    do_fio(&c__1, sym, (ftnlen)9);
	    e_wsfe();
	} else {
	    io___36.ciunit = *iounit;
	    s_wsfe(&io___36);
	    do_fio(&c__1, path, (ftnlen)3);
	    do_fio(&c__1, sym, (ftnlen)9);
	    e_wsfe();
	}
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Matrix types:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___37.ciunit = *iounit;
	s_wsfe(&io___37);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Test ratios:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___38.ciunit = *iounit;
	s_wsfe(&io___38);
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	io___39.ciunit = *iounit;
	s_wsfe(&io___39);
	do_fio(&c__1, (char *)&c__2, (ftnlen)sizeof(integer));
	e_wsfe();
	io___40.ciunit = *iounit;
	s_wsfe(&io___40);
	do_fio(&c__1, (char *)&c__3, (ftnlen)sizeof(integer));
	e_wsfe();
	io___41.ciunit = *iounit;
	s_wsfe(&io___41);
	do_fio(&c__1, (char *)&c__4, (ftnlen)sizeof(integer));
	e_wsfe();
	io___42.ciunit = *iounit;
	s_wsfe(&io___42);
	do_fio(&c__1, (char *)&c__5, (ftnlen)sizeof(integer));
	e_wsfe();
	io___43.ciunit = *iounit;
	s_wsfe(&io___43);
	do_fio(&c__1, (char *)&c__6, (ftnlen)sizeof(integer));
	e_wsfe();
	io___44.ciunit = *iounit;
	s_wsfe(&io___44);
	do_fio(&c__1, (char *)&c__7, (ftnlen)sizeof(integer));
	e_wsfe();
	io___45.ciunit = *iounit;
	s_wsfe(&io___45);
	do_fio(&c__1, (char *)&c__8, (ftnlen)sizeof(integer));
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Messages:' )";
	s_wsfe(&ci__1);
	e_wsfe();

    } else if (lsamen_(&c__2, p2, "PB")) {

/*        PB: Positive definite band */

	if (sord) {
	    io___46.ciunit = *iounit;
	    s_wsfe(&io___46);
	    do_fio(&c__1, path, (ftnlen)3);
	    do_fio(&c__1, "Symmetric", (ftnlen)9);
	    e_wsfe();
	} else {
	    io___47.ciunit = *iounit;
	    s_wsfe(&io___47);
	    do_fio(&c__1, path, (ftnlen)3);
	    do_fio(&c__1, "Hermitian", (ftnlen)9);
	    e_wsfe();
	}
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Matrix types:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___48.ciunit = *iounit;
	s_wsfe(&io___48);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Test ratios:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___49.ciunit = *iounit;
	s_wsfe(&io___49);
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	io___50.ciunit = *iounit;
	s_wsfe(&io___50);
	do_fio(&c__1, (char *)&c__2, (ftnlen)sizeof(integer));
	e_wsfe();
	io___51.ciunit = *iounit;
	s_wsfe(&io___51);
	do_fio(&c__1, (char *)&c__3, (ftnlen)sizeof(integer));
	e_wsfe();
	io___52.ciunit = *iounit;
	s_wsfe(&io___52);
	do_fio(&c__1, (char *)&c__4, (ftnlen)sizeof(integer));
	e_wsfe();
	io___53.ciunit = *iounit;
	s_wsfe(&io___53);
	do_fio(&c__1, (char *)&c__5, (ftnlen)sizeof(integer));
	e_wsfe();
	io___54.ciunit = *iounit;
	s_wsfe(&io___54);
	do_fio(&c__1, (char *)&c__6, (ftnlen)sizeof(integer));
	e_wsfe();
	io___55.ciunit = *iounit;
	s_wsfe(&io___55);
	do_fio(&c__1, (char *)&c__7, (ftnlen)sizeof(integer));
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Messages:' )";
	s_wsfe(&ci__1);
	e_wsfe();

    } else if (lsamen_(&c__2, p2, "PT")) {

/*        PT: Positive definite tridiagonal */

	if (sord) {
	    io___56.ciunit = *iounit;
	    s_wsfe(&io___56);
	    do_fio(&c__1, path, (ftnlen)3);
	    do_fio(&c__1, "Symmetric", (ftnlen)9);
	    e_wsfe();
	} else {
	    io___57.ciunit = *iounit;
	    s_wsfe(&io___57);
	    do_fio(&c__1, path, (ftnlen)3);
	    do_fio(&c__1, "Hermitian", (ftnlen)9);
	    e_wsfe();
	}
	io___58.ciunit = *iounit;
	s_wsfe(&io___58);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Test ratios:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___59.ciunit = *iounit;
	s_wsfe(&io___59);
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	io___60.ciunit = *iounit;
	s_wsfe(&io___60);
	do_fio(&c__1, (char *)&c__2, (ftnlen)sizeof(integer));
	e_wsfe();
	io___61.ciunit = *iounit;
	s_wsfe(&io___61);
	do_fio(&c__1, (char *)&c__3, (ftnlen)sizeof(integer));
	e_wsfe();
	io___62.ciunit = *iounit;
	s_wsfe(&io___62);
	do_fio(&c__1, (char *)&c__4, (ftnlen)sizeof(integer));
	e_wsfe();
	io___63.ciunit = *iounit;
	s_wsfe(&io___63);
	do_fio(&c__1, (char *)&c__5, (ftnlen)sizeof(integer));
	e_wsfe();
	io___64.ciunit = *iounit;
	s_wsfe(&io___64);
	do_fio(&c__1, (char *)&c__6, (ftnlen)sizeof(integer));
	e_wsfe();
	io___65.ciunit = *iounit;
	s_wsfe(&io___65);
	do_fio(&c__1, (char *)&c__7, (ftnlen)sizeof(integer));
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Messages:' )";
	s_wsfe(&ci__1);
	e_wsfe();

    } else if (lsamen_(&c__2, p2, "SY") || lsamen_(&
	    c__2, p2, "SP")) {

/*        SY: Symmetric indefinite full */
/*        SP: Symmetric indefinite packed */

	if (lsame_(c3, "Y")) {
	    io___66.ciunit = *iounit;
	    s_wsfe(&io___66);
	    do_fio(&c__1, path, (ftnlen)3);
	    do_fio(&c__1, "Symmetric", (ftnlen)9);
	    e_wsfe();
	} else {
	    io___67.ciunit = *iounit;
	    s_wsfe(&io___67);
	    do_fio(&c__1, path, (ftnlen)3);
	    do_fio(&c__1, "Symmetric", (ftnlen)9);
	    e_wsfe();
	}
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Matrix types:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	if (sord) {
	    io___68.ciunit = *iounit;
	    s_wsfe(&io___68);
	    e_wsfe();
	} else {
	    io___69.ciunit = *iounit;
	    s_wsfe(&io___69);
	    e_wsfe();
	}
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Test ratios:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___70.ciunit = *iounit;
	s_wsfe(&io___70);
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	io___71.ciunit = *iounit;
	s_wsfe(&io___71);
	do_fio(&c__1, (char *)&c__2, (ftnlen)sizeof(integer));
	e_wsfe();
	io___72.ciunit = *iounit;
	s_wsfe(&io___72);
	do_fio(&c__1, (char *)&c__3, (ftnlen)sizeof(integer));
	e_wsfe();
	io___73.ciunit = *iounit;
	s_wsfe(&io___73);
	do_fio(&c__1, (char *)&c__4, (ftnlen)sizeof(integer));
	e_wsfe();
	io___74.ciunit = *iounit;
	s_wsfe(&io___74);
	do_fio(&c__1, (char *)&c__5, (ftnlen)sizeof(integer));
	e_wsfe();
	io___75.ciunit = *iounit;
	s_wsfe(&io___75);
	do_fio(&c__1, (char *)&c__6, (ftnlen)sizeof(integer));
	e_wsfe();
	io___76.ciunit = *iounit;
	s_wsfe(&io___76);
	do_fio(&c__1, (char *)&c__7, (ftnlen)sizeof(integer));
	e_wsfe();
	io___77.ciunit = *iounit;
	s_wsfe(&io___77);
	do_fio(&c__1, (char *)&c__8, (ftnlen)sizeof(integer));
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Messages:' )";
	s_wsfe(&ci__1);
	e_wsfe();

    } else if (lsamen_(&c__2, p2, "HE") || lsamen_(&
	    c__2, p2, "HP")) {

/*        HE: Hermitian indefinite full */
/*        HP: Hermitian indefinite packed */

	if (lsame_(c3, "E")) {
	    io___78.ciunit = *iounit;
	    s_wsfe(&io___78);
	    do_fio(&c__1, path, (ftnlen)3);
	    do_fio(&c__1, "Hermitian", (ftnlen)9);
	    e_wsfe();
	} else {
	    io___79.ciunit = *iounit;
	    s_wsfe(&io___79);
	    do_fio(&c__1, path, (ftnlen)3);
	    do_fio(&c__1, "Hermitian", (ftnlen)9);
	    e_wsfe();
	}
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Matrix types:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___80.ciunit = *iounit;
	s_wsfe(&io___80);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Test ratios:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___81.ciunit = *iounit;
	s_wsfe(&io___81);
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	io___82.ciunit = *iounit;
	s_wsfe(&io___82);
	do_fio(&c__1, (char *)&c__2, (ftnlen)sizeof(integer));
	e_wsfe();
	io___83.ciunit = *iounit;
	s_wsfe(&io___83);
	do_fio(&c__1, (char *)&c__3, (ftnlen)sizeof(integer));
	e_wsfe();
	io___84.ciunit = *iounit;
	s_wsfe(&io___84);
	do_fio(&c__1, (char *)&c__4, (ftnlen)sizeof(integer));
	e_wsfe();
	io___85.ciunit = *iounit;
	s_wsfe(&io___85);
	do_fio(&c__1, (char *)&c__5, (ftnlen)sizeof(integer));
	e_wsfe();
	io___86.ciunit = *iounit;
	s_wsfe(&io___86);
	do_fio(&c__1, (char *)&c__6, (ftnlen)sizeof(integer));
	e_wsfe();
	io___87.ciunit = *iounit;
	s_wsfe(&io___87);
	do_fio(&c__1, (char *)&c__7, (ftnlen)sizeof(integer));
	e_wsfe();
	io___88.ciunit = *iounit;
	s_wsfe(&io___88);
	do_fio(&c__1, (char *)&c__8, (ftnlen)sizeof(integer));
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Messages:' )";
	s_wsfe(&ci__1);
	e_wsfe();

    } else if (lsamen_(&c__2, p2, "TR") || lsamen_(&
	    c__2, p2, "TP")) {

/*        TR: Triangular full */
/*        TP: Triangular packed */

	if (lsame_(c3, "R")) {
	    io___89.ciunit = *iounit;
	    s_wsfe(&io___89);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();
/* Writing concatenation */
	    i__1[0] = 1, a__1[0] = path;
	    i__1[1] = 5, a__1[1] = "LATRS";
	    s_cat(subnam, a__1, i__1, &c__2, (ftnlen)6);
	} else {
	    io___91.ciunit = *iounit;
	    s_wsfe(&io___91);
	    do_fio(&c__1, path, (ftnlen)3);
	    e_wsfe();
/* Writing concatenation */
	    i__1[0] = 1, a__1[0] = path;
	    i__1[1] = 5, a__1[1] = "LATPS";
	    s_cat(subnam, a__1, i__1, &c__2, (ftnlen)6);
	}
	io___92.ciunit = *iounit;
	s_wsfe(&io___92);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
	io___93.ciunit = *iounit;
	s_wsfe(&io___93);
	do_fio(&c__1, subnam, (ftnlen)6);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Test ratios:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___94.ciunit = *iounit;
	s_wsfe(&io___94);
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	io___95.ciunit = *iounit;
	s_wsfe(&io___95);
	do_fio(&c__1, (char *)&c__2, (ftnlen)sizeof(integer));
	e_wsfe();
	io___96.ciunit = *iounit;
	s_wsfe(&io___96);
	do_fio(&c__1, (char *)&c__3, (ftnlen)sizeof(integer));
	e_wsfe();
	io___97.ciunit = *iounit;
	s_wsfe(&io___97);
	do_fio(&c__1, (char *)&c__4, (ftnlen)sizeof(integer));
	e_wsfe();
	io___98.ciunit = *iounit;
	s_wsfe(&io___98);
	do_fio(&c__1, (char *)&c__5, (ftnlen)sizeof(integer));
	e_wsfe();
	io___99.ciunit = *iounit;
	s_wsfe(&io___99);
	do_fio(&c__1, (char *)&c__6, (ftnlen)sizeof(integer));
	e_wsfe();
	io___100.ciunit = *iounit;
	s_wsfe(&io___100);
	do_fio(&c__1, (char *)&c__7, (ftnlen)sizeof(integer));
	e_wsfe();
	io___101.ciunit = *iounit;
	s_wsfe(&io___101);
	do_fio(&c__1, subnam, (ftnlen)6);
	do_fio(&c__1, (char *)&c__8, (ftnlen)sizeof(integer));
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Messages:' )";
	s_wsfe(&ci__1);
	e_wsfe();

    } else if (lsamen_(&c__2, p2, "TB")) {

/*        TB: Triangular band */

	io___102.ciunit = *iounit;
	s_wsfe(&io___102);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
/* Writing concatenation */
	i__1[0] = 1, a__1[0] = path;
	i__1[1] = 5, a__1[1] = "LATBS";
	s_cat(subnam, a__1, i__1, &c__2, (ftnlen)6);
	io___103.ciunit = *iounit;
	s_wsfe(&io___103);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
	io___104.ciunit = *iounit;
	s_wsfe(&io___104);
	do_fio(&c__1, subnam, (ftnlen)6);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Test ratios:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___105.ciunit = *iounit;
	s_wsfe(&io___105);
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	io___106.ciunit = *iounit;
	s_wsfe(&io___106);
	do_fio(&c__1, (char *)&c__2, (ftnlen)sizeof(integer));
	e_wsfe();
	io___107.ciunit = *iounit;
	s_wsfe(&io___107);
	do_fio(&c__1, (char *)&c__3, (ftnlen)sizeof(integer));
	e_wsfe();
	io___108.ciunit = *iounit;
	s_wsfe(&io___108);
	do_fio(&c__1, (char *)&c__4, (ftnlen)sizeof(integer));
	e_wsfe();
	io___109.ciunit = *iounit;
	s_wsfe(&io___109);
	do_fio(&c__1, (char *)&c__5, (ftnlen)sizeof(integer));
	e_wsfe();
	io___110.ciunit = *iounit;
	s_wsfe(&io___110);
	do_fio(&c__1, (char *)&c__6, (ftnlen)sizeof(integer));
	e_wsfe();
	io___111.ciunit = *iounit;
	s_wsfe(&io___111);
	do_fio(&c__1, subnam, (ftnlen)6);
	do_fio(&c__1, (char *)&c__7, (ftnlen)sizeof(integer));
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Messages:' )";
	s_wsfe(&ci__1);
	e_wsfe();

    } else if (lsamen_(&c__2, p2, "QR")) {

/*        QR decomposition of rectangular matrices */

	io___112.ciunit = *iounit;
	s_wsfe(&io___112);
	do_fio(&c__1, path, (ftnlen)3);
	do_fio(&c__1, "QR", (ftnlen)2);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Matrix types:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___113.ciunit = *iounit;
	s_wsfe(&io___113);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Test ratios:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___114.ciunit = *iounit;
	s_wsfe(&io___114);
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	io___115.ciunit = *iounit;
	s_wsfe(&io___115);
	do_fio(&c__1, (char *)&c__2, (ftnlen)sizeof(integer));
	e_wsfe();
	io___116.ciunit = *iounit;
	s_wsfe(&io___116);
	do_fio(&c__1, (char *)&c__3, (ftnlen)sizeof(integer));
	do_fio(&c__1, "M", (ftnlen)1);
	e_wsfe();
	io___117.ciunit = *iounit;
	s_wsfe(&io___117);
	do_fio(&c__1, (char *)&c__4, (ftnlen)sizeof(integer));
	do_fio(&c__1, "M", (ftnlen)1);
	e_wsfe();
	io___118.ciunit = *iounit;
	s_wsfe(&io___118);
	do_fio(&c__1, (char *)&c__5, (ftnlen)sizeof(integer));
	do_fio(&c__1, "M", (ftnlen)1);
	e_wsfe();
	io___119.ciunit = *iounit;
	s_wsfe(&io___119);
	do_fio(&c__1, (char *)&c__6, (ftnlen)sizeof(integer));
	do_fio(&c__1, "M", (ftnlen)1);
	e_wsfe();
	io___120.ciunit = *iounit;
	s_wsfe(&io___120);
	do_fio(&c__1, (char *)&c__7, (ftnlen)sizeof(integer));
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Messages:' )";
	s_wsfe(&ci__1);
	e_wsfe();

    } else if (lsamen_(&c__2, p2, "LQ")) {

/*        LQ decomposition of rectangular matrices */

	io___121.ciunit = *iounit;
	s_wsfe(&io___121);
	do_fio(&c__1, path, (ftnlen)3);
	do_fio(&c__1, "LQ", (ftnlen)2);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Matrix types:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___122.ciunit = *iounit;
	s_wsfe(&io___122);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Test ratios:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___123.ciunit = *iounit;
	s_wsfe(&io___123);
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	io___124.ciunit = *iounit;
	s_wsfe(&io___124);
	do_fio(&c__1, (char *)&c__2, (ftnlen)sizeof(integer));
	e_wsfe();
	io___125.ciunit = *iounit;
	s_wsfe(&io___125);
	do_fio(&c__1, (char *)&c__3, (ftnlen)sizeof(integer));
	do_fio(&c__1, "N", (ftnlen)1);
	e_wsfe();
	io___126.ciunit = *iounit;
	s_wsfe(&io___126);
	do_fio(&c__1, (char *)&c__4, (ftnlen)sizeof(integer));
	do_fio(&c__1, "N", (ftnlen)1);
	e_wsfe();
	io___127.ciunit = *iounit;
	s_wsfe(&io___127);
	do_fio(&c__1, (char *)&c__5, (ftnlen)sizeof(integer));
	do_fio(&c__1, "N", (ftnlen)1);
	e_wsfe();
	io___128.ciunit = *iounit;
	s_wsfe(&io___128);
	do_fio(&c__1, (char *)&c__6, (ftnlen)sizeof(integer));
	do_fio(&c__1, "N", (ftnlen)1);
	e_wsfe();
	io___129.ciunit = *iounit;
	s_wsfe(&io___129);
	do_fio(&c__1, (char *)&c__7, (ftnlen)sizeof(integer));
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Messages:' )";
	s_wsfe(&ci__1);
	e_wsfe();

    } else if (lsamen_(&c__2, p2, "QL")) {

/*        QL decomposition of rectangular matrices */

	io___130.ciunit = *iounit;
	s_wsfe(&io___130);
	do_fio(&c__1, path, (ftnlen)3);
	do_fio(&c__1, "QL", (ftnlen)2);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Matrix types:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___131.ciunit = *iounit;
	s_wsfe(&io___131);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Test ratios:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___132.ciunit = *iounit;
	s_wsfe(&io___132);
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	io___133.ciunit = *iounit;
	s_wsfe(&io___133);
	do_fio(&c__1, (char *)&c__2, (ftnlen)sizeof(integer));
	e_wsfe();
	io___134.ciunit = *iounit;
	s_wsfe(&io___134);
	do_fio(&c__1, (char *)&c__3, (ftnlen)sizeof(integer));
	do_fio(&c__1, "M", (ftnlen)1);
	e_wsfe();
	io___135.ciunit = *iounit;
	s_wsfe(&io___135);
	do_fio(&c__1, (char *)&c__4, (ftnlen)sizeof(integer));
	do_fio(&c__1, "M", (ftnlen)1);
	e_wsfe();
	io___136.ciunit = *iounit;
	s_wsfe(&io___136);
	do_fio(&c__1, (char *)&c__5, (ftnlen)sizeof(integer));
	do_fio(&c__1, "M", (ftnlen)1);
	e_wsfe();
	io___137.ciunit = *iounit;
	s_wsfe(&io___137);
	do_fio(&c__1, (char *)&c__6, (ftnlen)sizeof(integer));
	do_fio(&c__1, "M", (ftnlen)1);
	e_wsfe();
	io___138.ciunit = *iounit;
	s_wsfe(&io___138);
	do_fio(&c__1, (char *)&c__7, (ftnlen)sizeof(integer));
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Messages:' )";
	s_wsfe(&ci__1);
	e_wsfe();

    } else if (lsamen_(&c__2, p2, "RQ")) {

/*        RQ decomposition of rectangular matrices */

	io___139.ciunit = *iounit;
	s_wsfe(&io___139);
	do_fio(&c__1, path, (ftnlen)3);
	do_fio(&c__1, "RQ", (ftnlen)2);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Matrix types:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___140.ciunit = *iounit;
	s_wsfe(&io___140);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Test ratios:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___141.ciunit = *iounit;
	s_wsfe(&io___141);
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	io___142.ciunit = *iounit;
	s_wsfe(&io___142);
	do_fio(&c__1, (char *)&c__2, (ftnlen)sizeof(integer));
	e_wsfe();
	io___143.ciunit = *iounit;
	s_wsfe(&io___143);
	do_fio(&c__1, (char *)&c__3, (ftnlen)sizeof(integer));
	do_fio(&c__1, "N", (ftnlen)1);
	e_wsfe();
	io___144.ciunit = *iounit;
	s_wsfe(&io___144);
	do_fio(&c__1, (char *)&c__4, (ftnlen)sizeof(integer));
	do_fio(&c__1, "N", (ftnlen)1);
	e_wsfe();
	io___145.ciunit = *iounit;
	s_wsfe(&io___145);
	do_fio(&c__1, (char *)&c__5, (ftnlen)sizeof(integer));
	do_fio(&c__1, "N", (ftnlen)1);
	e_wsfe();
	io___146.ciunit = *iounit;
	s_wsfe(&io___146);
	do_fio(&c__1, (char *)&c__6, (ftnlen)sizeof(integer));
	do_fio(&c__1, "N", (ftnlen)1);
	e_wsfe();
	io___147.ciunit = *iounit;
	s_wsfe(&io___147);
	do_fio(&c__1, (char *)&c__7, (ftnlen)sizeof(integer));
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Messages:' )";
	s_wsfe(&ci__1);
	e_wsfe();

    } else if (lsamen_(&c__2, p2, "QP")) {

/*        QR decomposition with column pivoting */

	io___148.ciunit = *iounit;
	s_wsfe(&io___148);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
	io___149.ciunit = *iounit;
	s_wsfe(&io___149);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Test ratios:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___150.ciunit = *iounit;
	s_wsfe(&io___150);
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	io___151.ciunit = *iounit;
	s_wsfe(&io___151);
	do_fio(&c__1, (char *)&c__2, (ftnlen)sizeof(integer));
	e_wsfe();
	io___152.ciunit = *iounit;
	s_wsfe(&io___152);
	do_fio(&c__1, (char *)&c__3, (ftnlen)sizeof(integer));
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Messages:' )";
	s_wsfe(&ci__1);
	e_wsfe();

    } else if (lsamen_(&c__2, p2, "TZ")) {

/*        TZ:  Trapezoidal */

	io___153.ciunit = *iounit;
	s_wsfe(&io___153);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
	io___154.ciunit = *iounit;
	s_wsfe(&io___154);
	e_wsfe();
	io___155.ciunit = *iounit;
	s_wsfe(&io___155);
	do_fio(&c__1, c1, (ftnlen)1);
	do_fio(&c__1, c1, (ftnlen)1);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Test ratios:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___156.ciunit = *iounit;
	s_wsfe(&io___156);
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	io___157.ciunit = *iounit;
	s_wsfe(&io___157);
	do_fio(&c__1, (char *)&c__2, (ftnlen)sizeof(integer));
	e_wsfe();
	io___158.ciunit = *iounit;
	s_wsfe(&io___158);
	do_fio(&c__1, (char *)&c__3, (ftnlen)sizeof(integer));
	e_wsfe();
	io___159.ciunit = *iounit;
	s_wsfe(&io___159);
	do_fio(&c__1, (char *)&c__4, (ftnlen)sizeof(integer));
	e_wsfe();
	io___160.ciunit = *iounit;
	s_wsfe(&io___160);
	do_fio(&c__1, (char *)&c__5, (ftnlen)sizeof(integer));
	e_wsfe();
	io___161.ciunit = *iounit;
	s_wsfe(&io___161);
	do_fio(&c__1, (char *)&c__6, (ftnlen)sizeof(integer));
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Messages:' )";
	s_wsfe(&ci__1);
	e_wsfe();

    } else if (lsamen_(&c__2, p2, "LS")) {

/*        LS:  Least Squares driver routines for */
/*             LS, LSD, LSS, LSX and LSY. */

	io___162.ciunit = *iounit;
	s_wsfe(&io___162);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
	io___163.ciunit = *iounit;
	s_wsfe(&io___163);
	e_wsfe();
	io___164.ciunit = *iounit;
	s_wsfe(&io___164);
	do_fio(&c__1, c1, (ftnlen)1);
	do_fio(&c__1, c1, (ftnlen)1);
	do_fio(&c__1, c1, (ftnlen)1);
	do_fio(&c__1, c1, (ftnlen)1);
	do_fio(&c__1, c1, (ftnlen)1);
	e_wsfe();
	io___165.ciunit = *iounit;
	s_wsfe(&io___165);
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	io___166.ciunit = *iounit;
	s_wsfe(&io___166);
	do_fio(&c__1, (char *)&c__2, (ftnlen)sizeof(integer));
	e_wsfe();
	io___167.ciunit = *iounit;
	s_wsfe(&io___167);
	do_fio(&c__1, (char *)&c__3, (ftnlen)sizeof(integer));
	e_wsfe();
	io___168.ciunit = *iounit;
	s_wsfe(&io___168);
	do_fio(&c__1, (char *)&c__4, (ftnlen)sizeof(integer));
	e_wsfe();
	io___169.ciunit = *iounit;
	s_wsfe(&io___169);
	do_fio(&c__1, (char *)&c__5, (ftnlen)sizeof(integer));
	e_wsfe();
	io___170.ciunit = *iounit;
	s_wsfe(&io___170);
	do_fio(&c__1, (char *)&c__6, (ftnlen)sizeof(integer));
	e_wsfe();
	io___171.ciunit = *iounit;
	s_wsfe(&io___171);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Messages:' )";
	s_wsfe(&ci__1);
	e_wsfe();

    } else if (lsamen_(&c__2, p2, "LU")) {

/*        LU factorization variants */

	io___172.ciunit = *iounit;
	s_wsfe(&io___172);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Matrix types:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___173.ciunit = *iounit;
	s_wsfe(&io___173);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Test ratio:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___174.ciunit = *iounit;
	s_wsfe(&io___174);
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Messages:' )";
	s_wsfe(&ci__1);
	e_wsfe();

    } else if (lsamen_(&c__2, p2, "CH")) {

/*        Cholesky factorization variants */

	io___175.ciunit = *iounit;
	s_wsfe(&io___175);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Matrix types:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___176.ciunit = *iounit;
	s_wsfe(&io___176);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Test ratio:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___177.ciunit = *iounit;
	s_wsfe(&io___177);
	do_fio(&c__1, (char *)&c__1, (ftnlen)sizeof(integer));
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Messages:' )";
	s_wsfe(&ci__1);
	e_wsfe();

    } else if (lsamen_(&c__2, p2, "QS")) {

/*        QR factorization variants */

	io___178.ciunit = *iounit;
	s_wsfe(&io___178);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Matrix types:' )";
	s_wsfe(&ci__1);
	e_wsfe();
	io___179.ciunit = *iounit;
	s_wsfe(&io___179);
	e_wsfe();
	ci__1.cierr = 0;
	ci__1.ciunit = *iounit;
	ci__1.cifmt = "( ' Test ratios:' )";
	s_wsfe(&ci__1);
	e_wsfe();

    } else {

/*        Print error message if no header is available. */

	io___180.ciunit = *iounit;
	s_wsfe(&io___180);
	do_fio(&c__1, path, (ftnlen)3);
	e_wsfe();
    }

/*     First line of header */


/*     GE matrix types */


/*     GB matrix types */


/*     GT matrix types */


/*     PT matrix types */


/*     PO, PP matrix types */


/*     CH matrix types */


/*     PB matrix types */


/*     SSY, SSP, CHE, CHP matrix types */


/*     CSY, CSP matrix types */


/*     QR matrix types */


/*     QP matrix types */


/*     TZ matrix types */


/*     LS matrix types */


/*     TR, TP matrix types */


/*     TB matrix types */


/*     Test ratios */

/* L9936: */
/* L9930: */

    return 0;

/*     End of ALAHD */

} /* alahd_ */
