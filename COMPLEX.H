#include <math.h>

typedef struct FCOMPLEX {double r,i;} fcomplex;

/*       This is a complex mathematics for any C compilers
         It is for calculating Fast-Fourier-Transforms and
         other complex calculations.
         By Kijun Park
         Department of Applied Physics
         Inha University                    June 20th,1990
*/

fcomplex Cadd(a,b)            /* Complex sum of two complex numbers. */
fcomplex a,b;
{
   fcomplex z;
   z.r = a.r + b.r;
   z.i = a.i + b.i;
   return z;
}


fcomplex Csub(a,b)           /* Complex difference of two complex numbers. */
fcomplex a,b;
{
   fcomplex z;
   z.r = a.r - b.r;
   z.i = a.i - b.i;
   return z;
}


fcomplex Cmul(a,b)            /* Complex product of two complex numbers */
fcomplex a,b;
{
   fcomplex z;
   z.r = a.r*b.r - a.i*b.i;
   z.i = a.i*b.r + a.r*b.i;
   return z;
}


fcomplex Cdiv(a,b)            /* The complex quotient of two complex numbers. */
fcomplex a,b;
{
   fcomplex z;
   double r,den;
   if (fabs(b.r) >= fabs(b.i)) {
      r   = b.i/b.r;
      den = b.r + r*b.i;
      z.r = (a.r + r*a.i)/den;
      z.i = (a.i - r*a.r)/den;
   }
   else  {
      r   = b.r/b.i;
      den = b.i + r*b.r;
      z.r = (a.r*r + a.i)/den;
      z.i = (a.i*r - a.r)/den;
   }
   return z;
}


fcomplex Cmplx(re,im)       /* Return complex number of specific numbers. */
double re,im;
{
   fcomplex z;
   z.r = re;
   z.i = im;
   return z;
}


fcomplex Cmplxrt(r,the)
double r,the;
{
   fcomplex z;
   z.r = r * cos(the);
   z.i = r * sin(the);
   return z;
}


double Cre(z)                  /* Returns real part of a complex number. */
fcomplex z;
{
   double x;
   x = z.r;
   return x;
}


double Cim(z)                  /* Returns imaginary part of a complex number. */
fcomplex z;
{
   double y;
   y = z.i;
   return y;
}


fcomplex Cconjg(z)             /* The complex conjugate of complex number. */
fcomplex z;
{
   fcomplex w;
   w.r = z.r;
   w.i = -z.i;
   return w;
}


double Carg(z)
fcomplex z;
{
   double theata;
   if (z.r == 0.0)
   {
      if (z.i > 0)
          theata = 3.141592654 / 2;
      else
          theata = -3.141592654 / 2;
   }
   else
   {
       theata = atan2(z.i,z.r);
   }
   return theata;
}


double Cabs(z)
fcomplex z;
{
   double r;
   r = sqrt(z.r*z.r + z.i*z.i);
   return r;
}


fcomplex Cremul(z,a)           /* Complex product of a real number. */
fcomplex z;
double a;
{
   fcomplex w;
   w.r = z.r * a;
   w.i = z.i * a;
   return w;
}


fcomplex Csqrt(z)             /* Square root of a complex number. */
fcomplex z;
{
   fcomplex c;
   double x,y,w,r;
   if ((z.r == 0.0) && (z.i == 0.0))
   {
      c.r = 0.0;
      c.i = 0.0;
      return c;
   }
   else  {
      x = fabs(z.r);
      y = fabs(z.i);
      if (x >= y) {
         r = y/x;
         w = sqrt(x) * sqrt(0.5 * (1.0 + sqrt(1.0 + r*r)));
      }
      else {
         r = x/y;
         w = sqrt(y) * sqrt(0.5 * (r + sqrt(1.0 + r*r)));
      }
      if (z.r >= 0.0) {
         c.r = w;
         c.i = z.i/(2*w);
      }
      else {
         c.i = (z.i >= 0.0) ? w : -w;
         c.r = z.i/(2.0*w);
      }
      return c;
   }
}



fcomplex Cexp(z)
fcomplex z;
{
   fcomplex w;
   w.r = exp(z.r) * cos(z.i);
   w.i = exp(z.r) * sin(z.i);
   return w;
}


fcomplex Clog(z)
fcomplex z;
{
   fcomplex w;
   double r;
   r = sqrt(z.r*z.r + z.i*z.i);
   w.r = log(r);
   w.i = atan2 (z.i,z.r);
   return w;
}

fcomplex Cpow(z,n)
fcomplex z;
double n;
{
   fcomplex w;
   double r,the,rn;
   r   = sqrt(z.r*z.r + z.i*z.i);
   rn  = pow(r,n);
   the = atan(z.i/z.r);
   w.r = rn * cos(n*the);
   w.i = rn * sin(n*the);
   return w;
}

