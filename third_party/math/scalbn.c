///////////////////////////////////////
// Implements:
//   double scalbn(double x, int n);
///////////////////////////////////////
// Notes:
//   This is taken from newlib.
//   Only included because pow() needs it.
// @nolint

/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

#include <stdint.h>
#include <pblibc_private.h>

/*
 * scalbn (double x, int n)
 * scalbn(x,n) returns x* 2**n  computed by  exponent
 * manipulation rather than by actually performing an
 * exponentiation or a multiplication.
 */

static const double
two54   =  1.80143985094819840000e+16, /* 0x43500000, 0x00000000 */
twom54  =  5.55111512312578270212e-17, /* 0x3C900000, 0x00000000 */
huge   = 1.0e+300,
tiny   = 1.0e-300;

typedef union
{
  double value;
  struct
  {
    uint32_t lsw;
    uint32_t msw;
  } parts;
} ieee_double_shape_type;

#define EXTRACT_WORDS(ix0,ix1,d)        \
do {                \
  ieee_double_shape_type ew_u;          \
  ew_u.value = (d);           \
  (ix0) = ew_u.parts.msw;         \
  (ix1) = ew_u.parts.lsw;         \
} while (0)

/* Get the more significant 32 bit int from a double.  */

#define GET_HIGH_WORD(i,d)          \
do {                \
  ieee_double_shape_type gh_u;          \
  gh_u.value = (d);           \
  (i) = gh_u.parts.msw;           \
} while (0)

/* Get the less significant 32 bit int from a double.  */

#define GET_LOW_WORD(i,d)         \
do {                \
  ieee_double_shape_type gl_u;          \
  gl_u.value = (d);           \
  (i) = gl_u.parts.lsw;           \
} while (0)

/* Set the more significant 32 bits of a double from an int.  */

#define SET_HIGH_WORD(d,v)          \
do {                \
  ieee_double_shape_type sh_u;          \
  sh_u.value = (d);           \
  sh_u.parts.msw = (v);           \
  (d) = sh_u.value;           \
} while (0)

/* Set the less significant 32 bits of a double from an int.  */

#define SET_LOW_WORD(d,v)         \
do {                \
  ieee_double_shape_type sl_u;          \
  sl_u.value = (d);           \
  sl_u.parts.lsw = (v);           \
  (d) = sl_u.value;           \
} while (0)

static double copysign(double x, double y)
{
  uint32_t hx,hy;
  GET_HIGH_WORD(hx,x);
  GET_HIGH_WORD(hy,y);
  SET_HIGH_WORD(x,(hx&0x7fffffff)|(hy&0x80000000));
        return x;
}

double scalbn(double x, int n)
{
  int32_t  k,hx,lx;
  EXTRACT_WORDS(hx,lx,x);
        k = (hx&0x7ff00000)>>20;    /* extract exponent */
        if (k==0) {       /* 0 or subnormal x */
            if ((lx|(hx&0x7fffffff))==0) return x; /* +-0 */
      x *= two54;
      GET_HIGH_WORD(hx,x);
      k = ((hx&0x7ff00000)>>20) - 54;
            if (n< -50000) return tiny*x;   /*underflow*/
      }
        if (k==0x7ff) return x+x;   /* NaN or Inf */
        k = k+n;
        if (k >  0x7fe) return huge*copysign(huge,x); /* overflow  */
        if (k > 0)        /* normal result */
      {SET_HIGH_WORD(x,(hx&0x800fffff)|(k<<20)); return x;}
        if (k <= -54) {
            if (n > 50000)  /* in case integer overflow in n+k */
    return huge*copysign(huge,x); /*overflow*/
      else return tiny*copysign(tiny,x);  /*underflow*/
      }
        k += 54;        /* subnormal result */
  SET_HIGH_WORD(x,(hx&0x800fffff)|(k<<20));
        return x*twom54;
}
