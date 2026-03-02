#include "cblas.h"
#include "fblaswr.h"

#define CVT_TRANSPOSE(t) \
  (((t) == CblasNoTrans) ? 'N' : \
   ((t) == CblasTrans) ? 'T' : \
   ((t) == CblasConjTrans) ? 'C' : \
   '\0')

#define CVT_UPLO(u) \
  (((u) == CblasUpper) ? 'U' : \
   ((u) == CblasLower) ? 'L' : \
   '\0')

#define CVT_DIAG(d) \
  (((d) == CblasUnit) ? 'U' : \
   ((d) == CblasNonUnit) ? 'N' : \
   '\0')

#define CVT_SIDE(s) \
  (((s) == CblasLeft) ? 'L' : \
   ((s) == CblasRight) ? 'R' : \
   '\0')

/*
 * ===========================================================================
 * Prototypes for level 1 BLAS functions (complex are recast as routines)
 * ===========================================================================
 */
float  cblas_sdot(const int N, const float  *X, const int incX,
                  const float  *Y, const int incY) {
  return sdot_(&N, X, &incX, Y, &incY);
}

double cblas_ddot(const int N, const double *X, const int incX, const double *Y, const int incY) {
  return ddot_(&N, X, &incX, Y, &incY);
}

/*
 * Functions having prefixes Z and C only
 */
void cblas_cdotu_sub(
  const int N, const void *X, const int incX, const void *Y, const int incY, void *dotu) {
  cdotu_(dotu, &N, X, &incX, Y, &incY);
}

void cblas_cdotc_sub(
  const int N, const void *X, const int incX, const void *Y, const int incY, void *dotc) {
  cdotc_(dotc, &N, X, &incX, Y, &incY);
}

void cblas_zdotu_sub(
  const int N, const void *X, const int incX, const void *Y, const int incY, void *dotu) {
  zdotu_(dotu, &N, X, &incX, Y, &incY);
}

void cblas_zdotc_sub(
  const int N, const void *X, const int incX, const void *Y, const int incY, void *dotc) {
  zdotc_(dotc, &N, X, &incX, Y, &incY);
}

/*
 * Functions having prefixes S D SC DZ
 */
float cblas_snrm2(const int N, const float *X, const int incX) { return snrm2_(&N, X, &incX); }
float cblas_sasum(const int N, const float *X, const int incX) { return sasum_(&N, X, &incX); }
double cblas_dnrm2(const int N, const double *X, const int incX) { return dnrm2_(&N, X, &incX); }
double cblas_dasum(const int N, const double *X, const int incX) { return dasum_(&N, X, &incX); }
float cblas_scnrm2(const int N, const void *X, const int incX) { return scnrm2_(&N, X, &incX); }
float cblas_scasum(const int N, const void *X, const int incX) { return scasum_(&N, X, &incX); }
double cblas_dznrm2(const int N, const void *X, const int incX) { return dznrm2_(&N, X, &incX); }
double cblas_dzasum(const int N, const void *X, const int incX) { return dzasum_(&N, X, &incX); }

/*
 * Functions having standard 4 prefixes (S D C Z)
 */
size_t cblas_isamax(const int N, const float *X, const int incX) { return isamax_(&N, X, &incX); }
size_t cblas_idamax(const int N, const double *X, const int incX) { return idamax_(&N, X, &incX); }
size_t cblas_icamax(const int N, const void *X, const int incX) { return icamax_(&N, X, &incX); }
size_t cblas_izamax(const int N, const void *X, const int incX) { return izamax_(&N, X, &incX); }

/*
 * ===========================================================================
 * Prototypes for level 1 BLAS routines
 * ===========================================================================
 */

/*
 * Routines with standard 4 prefixes (s, d, c, z)
 */
void cblas_sswap(const int N, float *X, const int incX, float *Y, const int incY) {
  sswap_(&N, X, &incX, Y, &incY);
}

void cblas_scopy(const int N, const float *X, const int incX, float *Y, const int incY) {
  scopy_(&N, X, &incX, Y, &incY);
}

void cblas_saxpy(
  const int N, const float alpha, const float *X, const int incX, float *Y, const int incY) {
  saxpy_(&N, &alpha, X, &incX, Y, &incY);
}

void cblas_dswap(const int N, double *X, const int incX, double *Y, const int incY) {
  dswap_(&N, X, &incX, Y, &incY);
}

void cblas_dcopy(const int N, const double *X, const int incX, double *Y, const int incY) {
  dcopy_(&N, X, &incX, Y, &incY);
}

void cblas_daxpy(
  const int N, const double alpha, const double *X, const int incX, double *Y, const int incY) {
  daxpy_(&N, &alpha, X, &incX, Y, &incY);
}

void cblas_cswap(const int N, void *X, const int incX, void *Y, const int incY) {
  cswap_(&N, X, &incX, Y, &incY);
}

void cblas_ccopy(const int N, const void *X, const int incX, void *Y, const int incY) {
  ccopy_(&N, X, &incX, Y, &incY);
}

void cblas_caxpy(
  const int N, const void *alpha, const void *X, const int incX, void *Y, const int incY) {
  caxpy_(&N, alpha, X, &incX, Y, &incY);
}

void cblas_zswap(const int N, void *X, const int incX, void *Y, const int incY) {
  zswap_(&N, X, &incX, Y, &incY);
}

void cblas_zcopy(const int N, const void *X, const int incX, void *Y, const int incY) {
  zcopy_(&N, X, &incX, Y, &incY);
}

void cblas_zaxpy(
  const int N, const void *alpha, const void *X, const int incX, void *Y, const int incY) {
  zaxpy_(&N, alpha, X, &incX, Y, &incY);
}

/*
 * Routines with S and D prefix only
 */
void cblas_srotg(float *a, float *b, float *c, float *s) { srotg_(a, b, c, s); }

void cblas_srot(
  const int N, float *X, const int incX, float *Y, const int incY, const float c, const float s) {
  srot_(&N, X, &incX, Y, &incY, &c, &s);
}

void cblas_drotg(double *a, double *b, double *c, double *s) { drotg_(a, b, c, s); }

void cblas_drot(
  const int N,
  double *X,
  const int incX,
  double *Y,
  const int incY,
  const double c,
  const double s) {
  drot_(&N, X, &incX, Y, &incY, &c, &s);
}

/*
 * Routines with S D C Z CS and ZD prefixes
 */
void cblas_sscal(const int N, const float alpha, float *X, const int incX) {
  sscal_(&N, &alpha, X, &incX);
}

void cblas_dscal(const int N, const double alpha, double *X, const int incX) {
  dscal_(&N, &alpha, X, &incX);
}

void cblas_cscal(const int N, const void *alpha, void *X, const int incX) {
  cscal_(&N, alpha, X, &incX);
}

void cblas_zscal(const int N, const void *alpha, void *X, const int incX) {
  zscal_(&N, alpha, X, &incX);
}

void cblas_csscal(const int N, const float alpha, void *X, const int incX) {
  csscal_(&N, &alpha, X, &incX);
}

void cblas_zdscal(const int N, const double alpha, void *X, const int incX) {
  zdscal_(&N, &alpha, X, &incX);
}

/*
 * ===========================================================================
 * Prototypes for level 2 BLAS
 * ===========================================================================
 */

/*
 * Routines with standard 4 prefixes (S, D, C, Z)
 */
void cblas_sgemv(const enum CBLAS_ORDER Order,
                 const enum CBLAS_TRANSPOSE TransA, const int M, const int N,
                 const float alpha, const float *A, const int lda,
                 const float *X, const int incX, const float beta,
                 float *Y, const int incY)
{
  char transA = CVT_TRANSPOSE(TransA);
  sgemv_(&transA, &M, &N, &alpha, A, &lda, X, &incX, &beta, Y, &incY);
}

/*
int
cblas_sgbmv(char *trans, integer *M, integer *N, integer *KL, integer *KU,
          real *alpha,
          real *A, integer *lda,
          real *X, integer *incX,
          real *beta,
          real *Y, integer *incY)
{
    sgbmv_(trans, M, N, KL, KU,
           alpha, A, lda, X, incX, beta, Y, incY);
    return 0;
}
*/

void cblas_strmv(
  const enum CBLAS_ORDER Order,
  const enum CBLAS_UPLO Uplo,
  const enum CBLAS_TRANSPOSE TransA,
  const enum CBLAS_DIAG Diag,
  const int N,
  const float *A,
  const int lda,
  float *X,
  const int incX) {
  char uplo = CVT_UPLO(Uplo);
  char trans = CVT_TRANSPOSE(TransA);
  char diag = CVT_DIAG(Diag);
  strmv_(&uplo, &trans, &diag, &N, A, &lda, X, &incX);
}

/*
int
cblas_stbmv(char* uplo, char* trans, char* diag, integer* N, integer* K,
          real* A, integer* lda,
          real* X, integer* incX)
{
    stbmv_(uplo, trans, diag,
           N, K, A, lda, X, incX);
    return 0;
}

int
cblas_stpmv(char* uplo, char* trans, char* diag, integer* N,
          real* Ap,
          real* X, integer* incX)
{
    stpmv_(uplo, trans, diag,
           N, Ap, X, incX);
    return 0;
}

int
cblas_strsv(char* uplo, char* trans, char* diag, integer* N,
          real* A, integer* lda,
          real* X, integer* incX)
{
    strsv_(uplo, trans, diag,
           N, A, lda, X, incX);
    return 0;
}

int
cblas_stbsv(char* uplo, char* trans, char* diag, integer* N, integer* K,
          real* A, integer* lda,
          real* X, integer* incX)
{
    stbsv_(uplo, trans, diag,
           N, K, A, lda, X, incX);
    return 0;
}

int
cblas_stpsv(char* uplo, char* trans, char* diag, integer* N,
          real* Ap,
          real* X, integer* incX)
{
    stpsv_(uplo, trans, diag,
           N, Ap, X, incX);
    return 0;
}



int
cblas_dgemv(char* trans, integer* M, integer* N,
          doublereal* alpha,
          doublereal* A, integer* lda,
          doublereal* X, integer* incX,
          doublereal* beta,
          doublereal* Y, integer* incY)
{
    dgemv_(trans, M, N,
           alpha, A, lda, X, incX, beta, Y, incY);
    return 0;
}

int
cblas_dgbmv(char *trans, integer *M, integer *N, integer *KL, integer *KU,
          doublereal *alpha,
          doublereal *A, integer *lda,
          doublereal *X, integer *incX,
          doublereal *beta,
          doublereal *Y, integer *incY)
{
    dgbmv_(trans, M, N, KL, KU,
           alpha, A, lda, X, incX, beta, Y, incY);
    return 0;
}

int
cblas_dtrmv(char* uplo, char *trans, char* diag, integer *N,
          doublereal *A, integer *lda,
          doublereal *X, integer *incX)
{
    dtrmv_(uplo, trans, diag,
           N, A, lda, X, incX);
    return 0;
}

int
cblas_dtbmv(char* uplo, char* trans, char* diag, integer* N, integer* K,
          doublereal* A, integer* lda,
          doublereal* X, integer* incX)
{
    dtbmv_(uplo, trans, diag,
           N, K, A, lda, X, incX);
    return 0;
}

int
cblas_dtpmv(char* uplo, char* trans, char* diag, integer* N,
          doublereal* Ap,
          doublereal* X, integer* incX)
{
    dtpmv_(uplo, trans, diag,
           N, Ap, X, incX);
    return 0;
}

int
cblas_dtrsv(char* uplo, char* trans, char* diag, integer* N,
          doublereal* A, integer* lda,
          doublereal* X, integer* incX)
{
    dtrsv_(uplo, trans, diag,
           N, A, lda, X, incX);
    return 0;
}

int
cblas_dtbsv(char* uplo, char* trans, char* diag, integer* N, integer* K,
          doublereal* A, integer* lda,
          doublereal* X, integer* incX)
{
    dtbsv_(uplo, trans, diag,
           N, K, A, lda, X, incX);
    return 0;
}

int
cblas_dtpsv(char* uplo, char* trans, char* diag, integer* N,
          doublereal* Ap,
          doublereal* X, integer* incX)
{
    dtpsv_(uplo, trans, diag,
           N, Ap, X, incX);
    return 0;
}



int
cblas_cgemv(char* trans, integer* M, integer* N,
          complex* alpha,
          complex* A, integer* lda,
          complex* X, integer* incX,
          complex* beta,
          complex* Y, integer* incY)
{
    cgemv_(trans, M, N,
           alpha, A, lda, X, incX, beta, Y, incY);
    return 0;
}

int
cblas_cgbmv(char *trans, integer *M, integer *N, integer *KL, integer *KU,
          complex *alpha,
          complex *A, integer *lda,
          complex *X, integer *incX,
          complex *beta,
          complex *Y, integer *incY)
{
    cgbmv_(trans, M, N, KL, KU,
           alpha, A, lda, X, incX, beta, Y, incY);
    return 0;
}

int
cblas_ctrmv(char* uplo, char *trans, char* diag, integer *N,
          complex *A, integer *lda,
          complex *X, integer *incX)
{
    ctrmv_(uplo, trans, diag,
           N, A, lda, X, incX);
    return 0;
}

int
cblas_ctbmv(char* uplo, char* trans, char* diag, integer* N, integer* K,
          complex* A, integer* lda,
          complex* X, integer* incX)
{
    ctbmv_(uplo, trans, diag,
           N, K, A, lda, X, incX);
    return 0;
}

int
cblas_ctpmv(char* uplo, char* trans, char* diag, integer* N,
          complex* Ap,
          complex* X, integer* incX)
{
    ctpmv_(uplo, trans, diag,
           N, Ap, X, incX);
    return 0;
}

int
cblas_ctrsv(char* uplo, char* trans, char* diag, integer* N,
          complex* A, integer* lda,
          complex* X, integer* incX)
{
    ctrsv_(uplo, trans, diag,
           N, A, lda, X, incX);
    return 0;
}

int
cblas_ctbsv(char* uplo, char* trans, char* diag, integer* N, integer* K,
          complex* A, integer* lda,
          complex* X, integer* incX)
{
    ctbsv_(uplo, trans, diag,
           N, K, A, lda, X, incX);
    return 0;
}

int
cblas_ctpsv(char* uplo, char* trans, char* diag, integer* N,
          complex* Ap,
          complex* X, integer* incX)
{
    ctpsv_(uplo, trans, diag,
           N, Ap, X, incX);
    return 0;
}



int
cblas_zgemv(char* trans, integer* M, integer* N,
          doublecomplex* alpha,
          doublecomplex* A, integer* lda,
          doublecomplex* X, integer* incX,
          doublecomplex* beta,
          doublecomplex* Y, integer* incY)
{
    zgemv_(trans, M, N,
           alpha, A, lda, X, incX, beta, Y, incY);
    return 0;
}

int
cblas_zgbmv(char *trans, integer *M, integer *N, integer *KL, integer *KU,
          doublecomplex *alpha,
          doublecomplex *A, integer *lda,
          doublecomplex *X, integer *incX,
          doublecomplex *beta,
          doublecomplex *Y, integer *incY)
{
    zgbmv_(trans, M, N, KL, KU,
           alpha, A, lda, X, incX, beta, Y, incY);
    return 0;
}

int
cblas_ztrmv(char* uplo, char *trans, char* diag, integer *N,
          doublecomplex *A, integer *lda,
          doublecomplex *X, integer *incX)
{
    ztrmv_(uplo, trans, diag,
           N, A, lda, X, incX);
    return 0;
}

int
cblas_ztbmv(char* uplo, char* trans, char* diag, integer* N, integer* K,
          doublecomplex* A, integer* lda,
          doublecomplex* X, integer* incX)
{
    ztbmv_(uplo, trans, diag,
           N, K, A, lda, X, incX);
    return 0;
}

int
cblas_ztpmv(char* uplo, char* trans, char* diag, integer* N,
          doublecomplex* Ap,
          doublecomplex* X, integer* incX)
{
    ztpmv_(uplo, trans, diag,
           N, Ap, X, incX);
    return 0;
}

int
cblas_ztrsv(char* uplo, char* trans, char* diag, integer* N,
          doublecomplex* A, integer* lda,
          doublecomplex* X, integer* incX)
{
    ztrsv_(uplo, trans, diag,
           N, A, lda, X, incX);
    return 0;
}

int
cblas_ztbsv(char* uplo, char* trans, char* diag, integer* N, integer* K,
          doublecomplex* A, integer* lda,
          doublecomplex* X, integer* incX)
{
    ztbsv_(uplo, trans, diag,
           N, K, A, lda, X, incX);
    return 0;
}

int
cblas_ztpsv(char* uplo, char* trans, char* diag, integer* N,
          doublecomplex* Ap,
          doublecomplex* X, integer* incX)
{
    ztpsv_(uplo, trans, diag,
           N, Ap, X, incX);
    return 0;
}

*/
/*
 * Routines with S and D prefixes only
 */
/*
int
cblas_ssymv(char* uplo, integer* N,
          real* alpha,
          real* A, integer* lda,
          real* X, integer* incX,
          real* beta,
          real* Y, integer* incY)
{
    ssymv_(uplo, N, alpha, A, lda,
           X, incX, beta, Y, incY);
    return 0;
}

int
cblas_ssbmv(char* uplo, integer* N, integer* K,
          real* alpha,
          real* A, integer* lda,
          real* X, integer* incX,
          real* beta,
          real* Y, integer* incY)
{
    ssbmv_(uplo, N, K, alpha, A, lda,
           X, incX, beta, Y, incY);
    return 0;
}

int
cblas_sspmv(char* uplo, integer* N,
          real* alpha,
          real* Ap,
          real* X, integer* incX,
          real* beta,
          real* Y, integer* incY)
{
    sspmv_(uplo, N, alpha, Ap,
           X, incX, beta, Y, incY);
    return 0;
}
*/

void cblas_sger(
  const enum CBLAS_ORDER Order,
  const int M,
  const int N,
  const float alpha,
  const float *X,
  const int incX,
  const float *Y,
  const int incY,
  float *A,
  const int lda) {
  sger_(&M, &N, &alpha, X, &incX, Y, &incY, A, &lda);
}

/*
int
cblas_ssyr(char* uplo, integer* N,
         real* alpha,
         real* X, integer* incX,
         real* A, integer* lda)
{
    ssyr_(uplo, N, alpha, X, incX, A, lda);
    return 0;
}

int
cblas_sspr(char* uplo, integer* N,
         real* alpha,
         real* X, integer* incX,
         real* Ap)
{
    sspr_(uplo, N, alpha, X, incX, Ap);
    return 0;
}

int
cblas_ssyr2(char* uplo, integer* N,
          real* alpha,
          real* X, integer* incX,
          real* Y, integer* incY,
          real* A, integer* lda)
{
    ssyr2_(uplo, N, alpha,
           X, incX, Y, incY, A, lda);
    return 0;
}

int
cblas_sspr2(char* uplo, integer* N,
          real* alpha,
          real* X, integer* incX,
          real* Y, integer* incY,
          real* A)
{
    sspr2_(uplo, N, alpha,
           X, incX, Y, incY, A);
    return 0;
}



int
cblas_dsymv(char* uplo, integer* N,
          doublereal* alpha,
          doublereal* A, integer* lda,
          doublereal* X, integer* incX,
          doublereal* beta,
          doublereal* Y, integer* incY)
{
    dsymv_(uplo, N, alpha, A, lda,
           X, incX, beta, Y, incY);
    return 0;
}

int
cblas_dsbmv(char* uplo, integer* N, integer* K,
          doublereal* alpha,
          doublereal* A, integer* lda,
          doublereal* X, integer* incX,
          doublereal* beta,
          doublereal* Y, integer* incY)
{
    dsbmv_(uplo, N, K, alpha, A, lda,
           X, incX, beta, Y, incY);
    return 0;
}

int
cblas_dspmv(char* uplo, integer* N,
          doublereal* alpha,
          doublereal* Ap,
          doublereal* X, integer* incX,
          doublereal* beta,
          doublereal* Y, integer* incY)
{
    dspmv_(uplo, N, alpha, Ap,
           X, incX, beta, Y, incY);
    return 0;
}

int
cblas_dger(integer* M, integer* N,
         doublereal* alpha,
         doublereal* X, integer* incX,
         doublereal* Y, integer* incY,
         doublereal* A, integer* lda)
{
    dger_(M, N, alpha,
          X, incX, Y, incY, A, lda);
    return 0;
}

int
cblas_dsyr(char* uplo, integer* N,
         doublereal* alpha,
         doublereal* X, integer* incX,
         doublereal* A, integer* lda)
{
    dsyr_(uplo, N, alpha, X, incX, A, lda);
    return 0;
}

int
cblas_dspr(char* uplo, integer* N,
         doublereal* alpha,
         doublereal* X, integer* incX,
         doublereal* Ap)
{
    dspr_(uplo, N, alpha, X, incX, Ap);
    return 0;
}

int
cblas_dsyr2(char* uplo, integer* N,
          doublereal* alpha,
          doublereal* X, integer* incX,
          doublereal* Y, integer* incY,
          doublereal* A, integer* lda)
{
    dsyr2_(uplo, N, alpha,
           X, incX, Y, incY, A, lda);
    return 0;
}

int
cblas_dspr2(char* uplo, integer* N,
          doublereal* alpha,
          doublereal* X, integer* incX,
          doublereal* Y, integer* incY,
          doublereal* A)
{
    dspr2_(uplo, N, alpha,
           X, incX, Y, incY, A);
    return 0;
}
*/


/*
 * Routines with C and Z prefixes only
 */
/*
int
cblas_chemv(char* uplo, integer* N,
          complex* alpha,
          complex* A, integer* lda,
          complex* X, integer* incX,
          complex* beta,
          complex* Y, integer* incY)
{
    chemv_(uplo, N, alpha, A, lda,
           X, incX, beta, Y, incY);
    return 0;
}

int
cblas_chbmv(char* uplo, integer* N, integer* K,
          complex* alpha,
          complex* A, integer* lda,
          complex* X, integer* incX,
          complex* beta,
          complex* Y, integer* incY)
{
    chbmv_(uplo, N, K, alpha, A, lda,
           X, incX, beta, Y, incY);
    return 0;
}

int
cblas_chpmv(char* uplo, integer* N,
          complex* alpha,
          complex* Ap,
          complex* X, integer* incX,
          complex* beta,
          complex* Y, integer* incY)
{
    chpmv_(uplo, N, alpha, Ap,
           X, incX, beta, Y, incY);
    return 0;
}

int
cblas_cgeru(integer* M, integer* N,
          complex* alpha,
          complex* X, integer* incX,
          complex* Y, integer* incY,
          complex* A, integer* lda)
{
    cgeru_(M, N, alpha,
           X, incX, Y, incY, A, lda);
    return 0;
}

int
cblas_cgerc(integer* M, integer* N,
          complex* alpha,
          complex* X, integer* incX,
          complex* Y, integer* incY,
          complex* A, integer* lda)
{
    cgerc_(M, N, alpha,
           X, incX, Y, incY, A, lda);
    return 0;
}

int
cblas_cher(char* uplo, integer* N,
         real* alpha,
         complex* X, integer* incX,
         complex* A, integer* lda)
{
    cher_(uplo, N, alpha,
          X, incX, A, lda);
    return 0;
}

int
cblas_chpr(char* uplo, integer* N,
         real* alpha,
         complex* X, integer* incX,
         complex* Ap)
{
    chpr_(uplo, N, alpha,
          X, incX, Ap);
    return 0;
}

int
cblas_cher2(char* uplo, integer* N,
          complex* alpha,
          complex* X, integer* incX,
          complex* Y, integer* incY,
          complex* A, integer* lda)
{
    cher2_(uplo, N, alpha,
           X, incX, Y, incY, A, lda);
    return 0;
}

int
cblas_chpr2(char* uplo, integer* N,
          complex* alpha,
          complex* X, integer* incX,
          complex* Y, integer* incY,
          complex* Ap)
{
    chpr2_(uplo, N, alpha,
           X, incX, Y, incY, Ap);
    return 0;
}



int
cblas_zhemv(char* uplo, integer* N,
          doublecomplex* alpha,
          doublecomplex* A, integer* lda,
          doublecomplex* X, integer* incX,
          doublecomplex* beta,
          doublecomplex* Y, integer* incY)
{
    zhemv_(uplo, N, alpha, A, lda,
           X, incX, beta, Y, incY);
    return 0;
}

int
cblas_zhbmv(char* uplo, integer* N, integer* K,
          doublecomplex* alpha,
          doublecomplex* A, integer* lda,
          doublecomplex* X, integer* incX,
          doublecomplex* beta,
          doublecomplex* Y, integer* incY)
{
    zhbmv_(uplo, N, K, alpha, A, lda,
           X, incX, beta, Y, incY);
    return 0;
}

int
cblas_zhpmv(char* uplo, integer* N,
          doublecomplex* alpha,
          doublecomplex* Ap,
          doublecomplex* X, integer* incX,
          doublecomplex* beta,
          doublecomplex* Y, integer* incY)
{
    zhpmv_(uplo, N, alpha, Ap,
           X, incX, beta, Y, incY);
    return 0;
}

int
cblas_zgeru(integer* M, integer* N,
          doublecomplex* alpha,
          doublecomplex* X, integer* incX,
          doublecomplex* Y, integer* incY,
          doublecomplex* A, integer* lda)
{
    zgeru_(M, N, alpha,
           X, incX, Y, incY, A, lda);
    return 0;
}

int
cblas_zgerc(integer* M, integer* N,
          doublecomplex* alpha,
          doublecomplex* X, integer* incX,
          doublecomplex* Y, integer* incY,
          doublecomplex* A, integer* lda)
{
    zgerc_(M, N, alpha,
           X, incX, Y, incY, A, lda);
    return 0;
}

int
cblas_zher(char* uplo, integer* N,
         doublereal* alpha,
         doublecomplex* X, integer* incX,
         doublecomplex* A, integer* lda)
{
    zher_(uplo, N, alpha,
          X, incX, A, lda);
    return 0;
}

int
cblas_zhpr(char* uplo, integer* N,
         doublereal* alpha,
         doublecomplex* X, integer* incX,
         doublecomplex* Ap)
{
    zhpr_(uplo, N, alpha,
          X, incX, Ap);
    return 0;
}

int
cblas_zher2(char* uplo, integer* N,
          doublecomplex* alpha,
          doublecomplex* X, integer* incX,
          doublecomplex* Y, integer* incY,
          doublecomplex* A, integer* lda)
{
    zher2_(uplo, N, alpha,
           X, incX, Y, incY, A, lda);
    return 0;
}

int
cblas_zhpr2(char* uplo, integer* N,
          doublecomplex* alpha,
          doublecomplex* X, integer* incX,
          doublecomplex* Y, integer* incY,
          doublecomplex* Ap)
{
    zhpr2_(uplo, N, alpha,
           X, incX, Y, incY, Ap);
    return 0;
}
*/


/*
 * ===========================================================================
 * Prototypes for level 3 BLAS
 * ===========================================================================
 */

/*
 * Routines with standard 4 prefixes (S, D, C, Z)
 */
void cblas_sgemm(
  const enum CBLAS_ORDER Order,
  const enum CBLAS_TRANSPOSE TransA,
  const enum CBLAS_TRANSPOSE TransB,
  const int M,
  const int N,
  const int K,
  const float alpha,
  const float *A,
  const int lda,
  const float *B,
  const int ldb,
  const float beta,
  float *C,
  const int ldc) {
  char transA = CVT_TRANSPOSE(TransA);
  char transB = CVT_TRANSPOSE(TransB);
  sgemm_(&transA, &transB, &M, &N, &K, &alpha, A, &lda, B, &ldb, &beta, C, &ldc);
}
/*
int
cblas_ssymm(char* side, char* uplo, integer* M, integer* N,
          real* alpha,
          real* A, integer* lda,
          real* B, integer* ldb,
          real* beta,
          real* C, integer* ldc)
{
    ssymm_(side, uplo, M, N,
           alpha, A, lda, B, ldb, beta, C, ldc);
    return 0;
}

int
cblas_ssyrk(char* uplo, char* trans, integer* N, integer* K,
          real* alpha,
          real* A, integer* lda,
          real* beta,
          real* C, integer* ldc)
{
    ssyrk_(uplo, trans, N, K,
           alpha, A, lda, beta, C, ldc);
    return 0;
}

int
cblas_ssyr2k(char* uplo, char* trans, integer* N, integer* K,
           real* alpha,
           real* A, integer* lda,
           real* B, integer* ldb,
           real* beta,
           real* C, integer* ldc)
{
    ssyr2k_(uplo, trans, N, K,
            alpha, A, lda, B, ldb, beta, C, ldc);
    return 0;
}
*/

void cblas_strmm(
  const enum CBLAS_ORDER Order,
  const enum CBLAS_SIDE Side,
  const enum CBLAS_UPLO Uplo,
  const enum CBLAS_TRANSPOSE TransA,
  const enum CBLAS_DIAG Diag,
  const int M,
  const int N,
  const float alpha,
  const float *A,
  const int lda,
  float *B,
  const int ldb) {
  char side = CVT_SIDE(Side);
  char uplo = CVT_UPLO(Uplo);
  char trans = CVT_TRANSPOSE(TransA);
  char diag = CVT_DIAG(Diag);
  strmm_(&side, &uplo, &trans, &diag, &M, &N, &alpha, A, &lda, B, &ldb);
}

void cblas_strsm(
  const enum CBLAS_ORDER Order,
  const enum CBLAS_SIDE Side,
  const enum CBLAS_UPLO Uplo,
  const enum CBLAS_TRANSPOSE TransA,
  const enum CBLAS_DIAG Diag,
  const int M,
  const int N,
  const float alpha,
  const float *A,
  const int lda,
  float *B,
  const int ldb) {
  char side = CVT_SIDE(Side);
  char uplo = CVT_UPLO(Uplo);
  char trans = CVT_TRANSPOSE(TransA);
  char diag = CVT_DIAG(Diag);
  strsm_(&side, &uplo, &trans, &diag, &M, &N, &alpha, A, &lda, B, &ldb);
}

/*
int
cblas_dgemm(char* transA, char* transB, integer* M, integer* N, integer* K,
          doublereal* alpha,
          doublereal* A, integer* lda,
          doublereal* B, integer* ldb,
          doublereal* beta,
          doublereal* C, integer* ldc)
{
    dgemm_(transA, transB, M, N, K,
           alpha, A, lda, B, ldb, beta, C, ldc);
    return 0;
}

int
cblas_dsymm(char* side, char* uplo, integer* M, integer* N,
          doublereal* alpha,
          doublereal* A, integer* lda,
          doublereal* B, integer* ldb,
          doublereal* beta,
          doublereal* C, integer* ldc)
{
    dsymm_(side, uplo, M, N,
           alpha, A, lda, B, ldb, beta, C, ldc);
    return 0;
}

int
cblas_dsyrk(char* uplo, char* trans, integer* N, integer* K,
          doublereal* alpha,
          doublereal* A, integer* lda,
          doublereal* beta,
          doublereal* C, integer* ldc)
{
    dsyrk_(uplo, trans, N, K,
           alpha, A, lda, beta, C, ldc);
    return 0;
}

int
cblas_dsyr2k(char* uplo, char* trans, integer* N, integer* K,
           doublereal* alpha,
           doublereal* A, integer* lda,
           doublereal* B, integer* ldb,
           doublereal* beta,
           doublereal* C, integer* ldc)
{
    dsyr2k_(uplo, trans, N, K,
            alpha, A, lda, B, ldb, beta, C, ldc);
    return 0;
}

int
cblas_dtrmm(char* side, char* uplo, char* trans, char* diag,
          integer* M, integer* N,
          doublereal* alpha,
          doublereal* A, integer* lda,
          doublereal* B, integer* ldb)
{
    dtrmm_(side, uplo, trans, diag,
           M, N, alpha, A, lda, B, ldb);
    return 0;
}

int
cblas_dtrsm(char* side, char* uplo, char* trans, char* diag,
          integer* M, integer* N,
          doublereal* alpha,
          doublereal* A, integer* lda,
          doublereal* B, integer* ldb)
{
    dtrsm_(side, uplo, trans, diag,
           M, N, alpha, A, lda, B, ldb);
    return 0;
}



int
cblas_cgemm(char* transA, char* transB, integer* M, integer* N, integer* K,
          complex* alpha,
          complex* A, integer* lda,
          complex* B, integer* ldb,
          complex* beta,
          complex* C, integer* ldc)
{
    cgemm_(transA, transB, M, N, K,
           alpha, A, lda, B, ldb, beta, C, ldc);
    return 0;
}

int
cblas_csymm(char* side, char* uplo, integer* M, integer* N,
          complex* alpha,
          complex* A, integer* lda,
          complex* B, integer* ldb,
          complex* beta,
          complex* C, integer* ldc)
{
    csymm_(side, uplo, M, N,
           alpha, A, lda, B, ldb, beta, C, ldc);
    return 0;
}

int
cblas_csyrk(char* uplo, char* trans, integer* N, integer* K,
          complex* alpha,
          complex* A, integer* lda,
          complex* beta,
          complex* C, integer* ldc)
{
    csyrk_(uplo, trans, N, K,
           alpha, A, lda, beta, C, ldc);
    return 0;
}

int
cblas_csyr2k(char* uplo, char* trans, integer* N, integer* K,
           complex* alpha,
           complex* A, integer* lda,
           complex* B, integer* ldb,
           complex* beta,
           complex* C, integer* ldc)
{
    csyr2k_(uplo, trans, N, K,
            alpha, A, lda, B, ldb, beta, C, ldc);
    return 0;
}

int
cblas_ctrmm(char* side, char* uplo, char* trans, char* diag,
          integer* M, integer* N,
          complex* alpha,
          complex* A, integer* lda,
          complex* B, integer* ldb)
{
    ctrmm_(side, uplo, trans, diag,
           M, N, alpha, A, lda, B, ldb);
    return 0;
}

int
cblas_ctrsm(char* side, char* uplo, char* trans, char* diag,
          integer* M, integer* N,
          complex* alpha,
          complex* A, integer* lda,
          complex* B, integer* ldb)
{
    ctrsm_(side, uplo, trans, diag,
           M, N, alpha, A, lda, B, ldb);
    return 0;
}



int
cblas_zgemm(char* transA, char* transB, integer* M, integer* N, integer* K,
          doublecomplex* alpha,
          doublecomplex* A, integer* lda,
          doublecomplex* B, integer* ldb,
          doublecomplex* beta,
          doublecomplex* C, integer* ldc)
{
    zgemm_(transA, transB, M, N, K,
           alpha, A, lda, B, ldb, beta, C, ldc);
    return 0;
}

int
cblas_zsymm(char* side, char* uplo, integer* M, integer* N,
          doublecomplex* alpha,
          doublecomplex* A, integer* lda,
          doublecomplex* B, integer* ldb,
          doublecomplex* beta,
          doublecomplex* C, integer* ldc)
{
    zsymm_(side, uplo, M, N,
           alpha, A, lda, B, ldb, beta, C, ldc);
    return 0;
}

int
cblas_zsyrk(char* uplo, char* trans, integer* N, integer* K,
          doublecomplex* alpha,
          doublecomplex* A, integer* lda,
          doublecomplex* beta,
          doublecomplex* C, integer* ldc)
{
    zsyrk_(uplo, trans, N, K,
           alpha, A, lda, beta, C, ldc);
    return 0;
}

int
cblas_zsyr2k(char* uplo, char* trans, integer* N, integer* K,
           doublecomplex* alpha,
           doublecomplex* A, integer* lda,
           doublecomplex* B, integer* ldb,
           doublecomplex* beta,
           doublecomplex* C, integer* ldc)
{
    zsyr2k_(uplo, trans, N, K,
            alpha, A, lda, B, ldb, beta, C, ldc);
    return 0;
}

int
cblas_ztrmm(char* side, char* uplo, char* trans, char* diag,
          integer* M, integer* N,
          doublecomplex* alpha,
          doublecomplex* A, integer* lda,
          doublecomplex* B, integer* ldb)
{
    ztrmm_(side, uplo, trans, diag,
           M, N, alpha, A, lda, B, ldb);
    return 0;
}

int
cblas_ztrsm(char* side, char* uplo, char* trans, char* diag,
          integer* M, integer* N,
          doublecomplex* alpha,
          doublecomplex* A, integer* lda,
          doublecomplex* B, integer* ldb)
{
    ztrsm_(side, uplo, trans, diag,
           M, N, alpha, A, lda, B, ldb);
    return 0;
}
*/


/*
 * Routines with prefixes C and Z only
 */
/*
int
cblas_chemm(char* side, char* uplo, integer* M, integer* N,
          complex* alpha,
          complex* A, integer* lda,
          complex* B, integer* ldb,
          complex* beta,
          complex* C, integer* ldc)
{
    chemm_(side, uplo, M, N,
           alpha, A, lda, B, ldb, beta, C, ldc);
    return 0;
}

int
cblas_cherk(char* uplo, char* trans, integer* N, integer* K,
          real* alpha,
          complex* A, integer* lda,
          real* beta,
          complex* C, integer* ldc)
{
    cherk_(uplo, trans, N, K,
           alpha, A, lda, beta, C, ldc);
    return 0;
}

int
cblas_cher2k(char* uplo, char* trans, integer* N, integer* K,
           complex* alpha,
           complex* A, integer* lda,
           complex* B, integer* ldb,
           real* beta,
           complex* C, integer* ldc)
{
    cher2k_(uplo, trans, N, K,
            alpha, A, lda, B, ldb, beta, C, ldc);
    return 0;
}



int
cblas_zhemm(char* side, char* uplo, integer* M, integer* N,
          doublecomplex* alpha,
          doublecomplex* A, integer* lda,
          doublecomplex* B, integer* ldb,
          doublecomplex* beta,
          doublecomplex* C, integer* ldc)
{
    zhemm_(side, uplo, M, N,
           alpha, A, lda, B, ldb, beta, C, ldc);
    return 0;
}

int
cblas_zherk(char* uplo, char* trans, integer* N, integer* K,
          doublereal* alpha,
          doublecomplex* A, integer* lda,
          doublereal* beta,
          doublecomplex* C, integer* ldc)
{
    zherk_(uplo, trans, N, K,
           alpha, A, lda, beta, C, ldc);
    return 0;
}

int
cblas_zher2k(char* uplo, char* trans, integer* N, integer* K,
           doublecomplex* alpha,
           doublecomplex* A, integer* lda,
           doublecomplex* B, integer* ldb,
           doublereal* beta,
           doublecomplex* C, integer* ldc)
{
    zher2k_(uplo, trans, N, K,
            alpha, A, lda, B, ldb, beta, C, ldc);
    return 0;
}
*/
