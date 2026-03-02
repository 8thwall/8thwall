#include "blaswrap.h"
#include "f2c.h"

/* Subroutine */ int zrotg_(doublecomplex *ca, doublecomplex *cb, doublereal *
	c__, doublecomplex *s)
{
    /* System generated locals */
    doublereal d__1, d__2;
    doublecomplex z__1, z__2, z__3, z__4;
    /* Builtin functions */
    double z_abs(doublecomplex *);
    void z_div(doublecomplex *, doublecomplex *, doublecomplex *);
    double sqrt(doublereal);
    void d_cnjg(doublecomplex *, doublecomplex *);
    /* Local variables */
    doublereal norm;
    doublecomplex alpha;
    doublereal scale;
    if (z_abs(ca) != 0.) {
	goto L10;
    }
    *c__ = 0.;
    s->r = 1., s->i = 0.;
    ca->r = cb->r, ca->i = cb->i;
    goto L20;
L10:
    scale = z_abs(ca) + z_abs(cb);
    z__2.r = scale, z__2.i = 0.;
    z_div(&z__1, ca, &z__2);
/* Computing 2nd power */
    d__1 = z_abs(&z__1);
    z__4.r = scale, z__4.i = 0.;
    z_div(&z__3, cb, &z__4);
/* Computing 2nd power */
    d__2 = z_abs(&z__3);
    norm = scale * sqrt(d__1 * d__1 + d__2 * d__2);
    d__1 = z_abs(ca);
    z__1.r = ca->r / d__1, z__1.i = ca->i / d__1;
    alpha.r = z__1.r, alpha.i = z__1.i;
    *c__ = z_abs(ca) / norm;
    d_cnjg(&z__3, cb);
    z__2.r = alpha.r * z__3.r - alpha.i * z__3.i, z__2.i = alpha.r * z__3.i + 
	    alpha.i * z__3.r;
    z__1.r = z__2.r / norm, z__1.i = z__2.i / norm;
    s->r = z__1.r, s->i = z__1.i;
    z__1.r = norm * alpha.r, z__1.i = norm * alpha.i;
    ca->r = z__1.r, ca->i = z__1.i;
L20:
    return 0;
} /* zrotg_ */

