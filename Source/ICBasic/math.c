/***********************************************************************/
/* Adapted mathlib for IC 35, original Source from calctool			   */
/* Michael Bruennert, 2001/04/25									   */
/***********************************************************************/


/*  @(#)mathlib.c 1.4 90/03/26
 *
 *  These are the mathematical routines used by calctool.
 *
 *  This is being done because libm.a doesn't appear to be as portable
 *  as originally assumed.
 *
 *  It is hoped that your system supplies all the mathematical functions
 *  required by calctool. If not then, it is possible to use the needed
 *  ones from this library. Look in mathlib.h for a set of definitions,
 *  and uncomment and set appropriately.
 *
 *  These routines are taken from two sources:
 *
 *  1/ Fred Fishs' portable maths library which was posted to the
 *     comp.sources.unix newsgroup on April 1987.
 *
 *     acos, acosh, asin, asinh, atan, atanh, cos, cosh, dabs,
 *     exp, log, log10, mod, poly, sin, sinh, sqrt, tan, tanh.
 *
 *  2/ The BSD4.3 maths library found on uunet in
 *     bsd_sources/src/usr.lib/libm.
 *
 *     pow, pow_p, scalb, logb, copysign, finite, drem, exp__E,
 *     log__L.
 *
 *  Customised and condensed by Rich Burridge,
 *                              Sun Microsystems, Australia.
 *
 *  Permission is given to distribute these sources, as long as the
 *  copyright messages are not removed, and no monies are exchanged.
 *
 *  No responsibility is taken for any errors or inaccuracies inherent
 *  either to the comments or the code of this program, but if
 *  reported to me then an attempt will be made to fix them.
 */

/************************************************************************
 *                                                                      *
 *                              N O T I C E                             *
 *                                                                      *
 *                      Copyright Abandoned, 1987, Fred Fish            *
 *                                                                      *
 *      This previously copyrighted work has been placed into the       *
 *      public domain by the author (Fred Fish) and may be freely used  *
 *      for any purpose, private or commercial.  I would appreciate     *
 *      it, as a courtesy, if this notice is left in all copies and     *
 *      derivative works.  Thank you, and enjoy...                      *
 *                                                                      *
 *      The author makes no warranty of any kind with respect to this   *
 *      product and explicitly disclaims any implied warranties of      *
 *      merchantability or fitness for any particular purpose.          *
 *                                                                      *
 ************************************************************************
 */

#include "ic35sdk.h"
#include "ic35math.h"


/* IC 35 double number format:												*/
/* x=sign(63) exponent(62-56) integer_part(55-48) fractional_part(47-0)		*/
/* The numbers are used big endian, the msb (bits 63-56) can be found at	*/
/* (U8 *)(&x), the lsb (bits 7-0) at ((U8 *)(&x))+7							*/
/* the value is:															*/
/* val(x)=sign*8^(exponent-65)*(integer_part+fractional_part*(2^-48))		*/


/****************************************************************************/
/* Constants																*/
/****************************************************************************/

extern double log_pcoeffs[4];
extern double log_qcoeffs[4];
extern double atan_coeffs[9];
extern double fpof2tbl[16];
extern double sin_pcoeffs[4];
extern double sin_qcoeffs[4];
extern double cos_pcoeffs[4];
extern double cos_qcoeffs[4];
extern double expe_p1;
extern double expe_p2;
extern double expe_q1;
extern double expe_q2;
extern double log_l_L[8];


/****************************************************************************/
/* Basic arithmetic functions												*/
/****************************************************************************/


/* return the absolute value of a number									*/
/* this is done by masking the sign bit										*/


void abs(double x,double *y)
{
	U8 *k;
	/* access the msb of the number */
	k=(U8 *)(&x);
	/* mask the sign bit */
	*k=*k & 127;
	*y=x;
}

/* return x with the sign of y												*/
double copysign(double x,double y)
{
  U8 *px = (U8 *) (&x),
     *py = (U8 *) (&y);


  *px = (*px & 127) | (*py & 128) ;
  return(x);
}


/* returns a number which has to be multiplied by 2^pot to get the input number	*/
double frexp(double x,S16 *pot)
{
	U8 *k,*k2;
	U8 i;
	/* access the msb of the number, it contains the power */
	*pot=*((U8 *)(&x)) & 127;
	k=(U8 *)(&x);					/* pointer to exponent */
	k2=k+1;							/* pointer to integer  */
									/* part				   */

	/* Calculate the power of 2	from the exponent */
	*pot=(*pot-64)*8;

	/* Our return number will be between 0.5 and 1, so the exponent must be */
	/* 40h and the upmost bit of the integer part must be set				*/
	*k=(*k & 128) | 64; /* set bit 6 clear all other, let sign (bit 7)as is	*/

	/* check, if we have a zero as input									*/
	if ((*k2)==0)
	{
		*pot=0;
		return (.0);
	}

	/* Shift the fraction until it's in the range between 0.5 and 1			*/
	/* which means that its msb must be set									*/
	
	while (*k2<128)
	{
		/* Shift the integer part */
		*k2=*k2<<1;
		/* Shift the fraction	  */
		for (i=1;i<=6;i++)
		{
			/* get bits falling from one register into another				*/
			if (*(k2+i)>127)
			{
				(*(k2+i-1))+=1;
				(*(k2+i))-=128;
			}
			/* shift the rest												*/
			*(k2+i)=*(k2+i)<<1;
		}
		/* adapt the power													*/
		(*pot)-=1;
	}
	return(x);
}

double ldexp(double x, S16 pow)
{
	U8 *k;
	S16 xexp;
	U8  i;

	/* access the numbers exponent											*/
	k=(U8 *)(&x);
	
	/* calculate the new exponent											*/
	xexp=(S16)(*k & 127) + pow/8;

	/* for the nagative exponents we need to get 8 more since the modulo	*/
	/* function will return positive numbers								*/
	/* We don't have to do this because the optimizer seems to replace the	*/
	/* division by 8 through a right shift, which automatically rounds to	*/
	/* the next smaller number, so -1/8=-1 instead of 0						*/
	//if (pow<0)
	//{
	//	xexp-=1;
	//}

	/* check for errors														*/
	if (xexp>127)
	{
        doerr("ldexp", "OVERFLOW", EDOM) ;
		return(INF);
	}
	if (xexp<0)
	{
        doerr("ldexp", "UNDERFLOW", EDOM) ;
		return(0.0);
	}
	
	/* Adapt the exponent													*/
	*k=(*k & 128) | (U8) xexp;

	/* Get the Rest of the power, which can't be done by exponent shift		*/
	/* This will return a positive number between 0 and 7, so we needed		*/
	/* the exponent correction for negative numbers, but only have to shift */
	/* in one direction														*/
	pow=pow & 7;
	while (pow)
	{
		/* check, if the highest bit of the mantissa is set, then we need	*/
		/* an exponent adaptation											*/
		if (*(k+1) & 128)
		{
			/* check for overflow											*/
			if ((*k & 127)<127)
			{
				*k+=1;
			}
			else
			{
		        doerr("ldexp", "OVERFLOW", EDOM) ;
				return(INF);
			}
			/* Shift the mantissa 1 byte to the right						*/
			for (i=6;i>=1;i--)
			{
				*(k+i+1)=*(k+i);
			}
			*(k+1)=0;
		}
		/* Now we should be able to shift our mantissa one byte to the left */
		/* without any overflow												*/
		/* shift the first mantissa byte, no overflow possible				*/
		
		*(k+1)<<=1;

		/* shift the rest of the mantisa bytes								*/
		for (i=2;i<=7;i++)
		{
			/* put bits falling out of the left side into the next byte		*/
			if(*(k+i) & 128)
			{
				*(k+i-1)+=1;
			}
			*(k+i)<<=1;
		}
		/* We have increased our number by the factor of two, so our work has */
		/* decreased														*/
		pow-=1;
	}
	return(x);
}

/* returns the integer part of a number										*/
void trunc(double x,double *y)

{
	U8 e;
	U8 *ptr;
	/* Get the exponent of the number										*/
	e=*((U8 *)&x) & 127;
	/* Check if number is smaller than 1, then it can't have an integer part*/
	if (e<65)
	{
		*y=0.0;
	}
	/* Check if number is grater than 8^5, then our number format isn't		*/
	/* capeable to store any fractional part, so we can return the number   */
	/* as it is																*/
	else if (e>70)
	{
		*y=x;
	}
	/* we have to remove the fractional part								*/
	else
	{
		*y=x;
		ptr=(U8 *)y+(e-63);
		/* set all bytes belonging to the fractional part to 0				*/
		memset(ptr,0,71-e);
	}
}

/* round up to the next grater integer number								*/
void ceil(double x,double *y)
{
	/* get the integer part of the number									*/
	trunc(x,y);
	/* for positive numbers +1 must be added								*/
	if (!(*((U8 *)&x) &128))
	{
		*y+=1.0;
	}

}

/* round to the next smaller integer number									*/
void floor(double x,double *y)
{
	/* get the integer part of the number									*/
	trunc(x,y);
	/* for negative numbers -1 must be added								*/
	if (*((U8 *)&x) &128)
	{
		*y-=1.0;
	}

}

/* break a number into integer and fractional part, returning both as double*/
void modf(double x,double *intp, double *fracp)
{
	U8 e;
	U8 i;
	U8 *ptr,*ptr2;

	/* Get the exponent of the number										*/
	e=*((U8 *)&x) & 127;

	/* Check if number is smaller than 1, then it can't have an integer part*/
	if (e<65)
	{
		*intp=0.0;
		*fracp=x;
	}
	/* Check if number is grater than 8^5, then our number format isn't		*/
	/* capeable to store any fractional part, so we can return the number   */
	/* as it is as integer part and fractional part =0						*/
	else if (e>70)
	{
		*intp=x;
		*fracp=0.0;
	}
	/* we have to remove the fractional part								*/
	else
	{
		*intp=x;
		i=e-63;

		ptr=(U8 *)intp+i;
		ptr2=(U8 *)fracp;
		
		/* Find the first byte in the fractional part of the mantissa		*/
		/* which is different from 0										*/
		while (i<8 && !(*ptr))
		{
			ptr++;
			i++;
		}

		/* Check, if we had a factional part of 0							*/
		if (i==8)
		{
			/* We can return it and don't have to remove anything from		*/
			/* our given number to get the integer part						*/
			*fracp=0.0;
			*intp=x;
		}
		else
		{
			/* clear the fracp variable										*/ 
			memset(ptr2,0,8);
			/* copy the fractional part found to the mantissa of the fracp	*/
			/* variable														*/
			memcpy(ptr2+1,ptr,8-i);
			/* Clear the fractional part in the intp-variable to have a pure*/
			/* integer value												*/
			memset(ptr,0,8-i);
			/* Set the exponent and copy the sign of the input into the		*/
			/* fracp variable												*/
			*ptr2=(1+e-i) | (*((U8 *)&x) & 128);
		}
	}
}

/* rounds the given number to the next integer								*/
void round(double x,double *y)

{
	U8 e;
	U8 *ptr;
	/* Get the exponent of the number										*/
	e=*((U8 *)&x) & 127;
	/* Check if number is smaller than 1/256, then it can't have to round to*/
	/* anything other than 0												*/
	if (e<64)
	{
		*y=0.0;
	}
	/* Check if number is grater than 8^5, then our number format isn't		*/
	/* capeable to store any fractional part, so we can return the number   */
	/* as it is																*/
	else if (e>70)
	{
		*y=x;
	}
	/* We have a number between 1/256 and 1 so we can have to round			*/
	else if (e==64)
	{
		/* Check if the number is grater than 0.5 which can be done by		*/
		/* looking at the first bit (msb) of the mantiss, since we don't	*/
		/* have an integer part												*/
		if (*(((U8 *)&x)+1) & 128)
		{
			/* So we have to round to +-1 according to the sign of x		*/
			*y=copysign(1.0,x);
		}
	}
	/* we have to remove the fractional part								*/
	else
	{
		*y=x;
		ptr=(U8 *)y+(e-63);
		/* Check, if the fractional lpart of the number is grater than 0.5  */
		/* which can be done by looking at the msb of the fractional part	*/
		if (*ptr & 128)
		{
			/* increase the absolute value of y by one						*/
			*y+=copysign(1.0,x);
		}
		/* set all bytes belonging to the fractional part to 0				*/
		memset(ptr,0,71-e);
	}
}



/*  FUNCTION
 *
 *      mod   double precision modulo
 *
 *  DESCRIPTION
 *
 *      Returns double precision modulo of two double
 *      precision arguments.
 *
 *  USAGE
 *
 *      double mod(value, base)
 *      double value ;
 *      double base ;
 */

void mod(double value,double base, double *result)
{
  value /= base ;
  modf(value, result, &value) ;
  *result=value*base ;
}


/*  FUNCTION
 *
 *      sqrt   double precision square root
 *
 *  DESCRIPTION
 *
 *      Returns double precision square root of double precision
 *      floating point argument.
 *
 *  USAGE
 *
 *      double sqrt(x)
 *      double x ;
 *
 *  REFERENCES
 *
 *      Fortran IV-PLUS user's guide, Digital Equipment Corp. pp B-7.
 *
 *      Computer Approximations, J.F. Hart et al, John Wiley & Sons,
 *      1968, pp. 89-96.
 *
 *  RESTRICTIONS
 *
 *      The relative error is 10**(-30.1) after three applications
 *      of Heron's iteration for the square root.
 *
 *      However, this assumes exact arithmetic in the iterations
 *      and initial approximation.  Additional errors may occur
 *      due to truncation, rounding, or machine precision limits.
 *
 *  INTERNALS
 *
 *      Computes square root by:
 *
 *        (1)   Range reduction of argument to [0.5,1.0]
 *              by application of identity:
 *
 *              sqrt(x)  =  2**(k/2) * sqrt(x * 2**(-k))
 *
 *              k is the exponent when x is written as
 *              a mantissa times a power of 2 (m * 2**k).
 *              It is assumed that the mantissa is
 *              already normalized (0.5 =< m < 1.0).
 *
 *        (2)   An approximation to sqrt(m) is obtained
 *              from:
 *
 *              u = sqrt(m) = (P0 + P1*m) / (Q0 + Q1*m)
 *
 *              P0 = 0.594604482
 *              P1 = 2.54164041
 *              Q0 = 2.13725758
 *              Q1 = 1.0
 *
 *              (coefficients from HART table #350 pg 193)
 *
 *        (3)   Three applications of Heron's iteration are
 *              performed using:
 *
 *              y[n+1] = 0.5 * (y[n] + (m/y[n]))
 *
 *              where y[0] = u = approx sqrt(m)
 *
 *        (4)   If the value of k was odd then y is either
 *              multiplied by the square root of two or
 *              divided by the square root of two for k positive
 *              or negative respectively.  This rescales y
 *              by multiplying by 2**frac(k/2).
 *
 *        (5)   Finally, y is rescaled by int(k/2) which
 *              is equivalent to multiplication by 2**int(k/2).
 *
 *              The result of steps 4 and 5 is that the value
 *              of y between 0.5 and 1.0 has been rescaled by
 *              2**(k/2) which removes the original rescaling
 *              done prior to finding the mantissa square root.
 *
 *  NOTES
 *
 *      The Convergent Technologies compiler optimizes division
 *      by powers of two to "arithmetic shift right" instructions.
 *      This is ok for positive integers but gives different
 *      results than other C compilers for negative integers.
 *      For example, (-1)/2 is -1 on the CT box, 0 on every other
 *      machine I tried.
 */

#define  ITERATIONS  3                        /* Number of iterations */
#define  P0          0.594604482              /* Approximation coeff  */
#define  P1          2.54164041               /* Approximation coeff  */
#define  Q0          2.13725758               /* Approximation coeff  */
#define  Q1          1.0                      /* Approximation coeff  */

void sqrt(double x,double *retval)
{
  U8 bugfix=2;
  U8 count;
  S16 k;
  auto double m, u, y ;

  if (x == 0.0)  *retval = 0.0 ;
  else if (x < 0.0)
    {
      doerr("sqrt", "DOMAIN", EDOM) ;
      *retval = 0.0 ;
    }
  else
    {
      m = frexp(x, &k) ;
      u = (P0 + (P1 * m)) / (Q0 + (Q1 * m)) ;
      for (count = 0, y = u; count < ITERATIONS; count++)
        y = 0.5 * (y + (m / y)) ;
	  /* odd exponent found ?							*/
	  if (k & 1)
	  {
		  if (k>0)
		  {
			  y *= SQRT2 ;
		  }
	 	  else
		  {
			  y /= SQRT2 ;
		  }
	  }

      *retval = ldexp(y, k / bugfix) ;
  }
}


/********************************************************/
/* Helper functions										*/
/********************************************************/

/*  FUNCTION
 *
 *      poly   double precision polynomial evaluation
 *
 *  DESCRIPTION
 *
 *      Evaluates a polynomial and returns double precision
 *      result.  Is passed a the order of the polynomial,
 *      a pointer to an array of double precision polynomial
 *      coefficients (in ascending order), and the independent
 *      variable.
 *
 *  USAGE
 *
 *      double poly(order, coeffs, x)
 *      int order ;
 *      double *coeffs ;
 *      double x ;
 *
 *  INTERNALS
 *
 *      Evalates the polynomial using the following form and evaluating the Horner-Sceme
 *
 *              P(x) = P0 + x(P1 + x(P2 +...x(Pn)))
 */

double poly(U8 order, double * coeffs, double x)
{
	double sum;
	U8 i;
	/* put the pointer to the last (Pn) coefficient						*/
	coeffs+=order;
	sum=*coeffs;
	coeffs--;
	for (i=order;i>0;i--,coeffs--)
	{
		sum=*coeffs+sum*x;
	}
	return(sum);
}

/****************************************************************/
/* Exponential functions										*/
/****************************************************************/

/*  FUNCTION
 *
 *      log   double precision natural log
 *
 *  DESCRIPTION
 *
 *      Returns double precision natural log of double precision
 *      floating point argument.
 *
 *  USAGE
 *
 *      double log(x)
 *      double x ;
 *
 *  REFERENCES
 *
 *      Computer Approximations, J.F. Hart et al, John Wiley & Sons,
 *      1968, pp. 105-111.
 *
 *  RESTRICTIONS
 *
 *      The absolute error in the approximating polynomial is
 *      10**(-19.38).  Note that contrary to DEC's assertion
 *      in the F4P user's guide, the error is absolute and not
 *      relative.
 *
 *      This error bound assumes exact arithmetic
 *      in the polynomial evaluation.  Additional rounding and
 *      truncation errors may occur as the argument is reduced
 *      to the range over which the polynomial approximation
 *      is valid, and as the polynomial is evaluated using
 *      finite-precision arithmetic.
 *
 *  INTERNALS
 *
 *      Computes log(X) from:
 *
 *        (1)   If argument is zero then flag an error
 *              and return minus infinity (or rather its
 *              machine representation).
 *
 *        (2)   If argument is negative then flag an
 *              error and return minus infinity.
 *
 *        (3)   Given that x = m * 2**k then extract
 *              mantissa m and exponent k.
 *
 *        (4)   Transform m which is in range [0.5,1.0]
 *              to range [1/sqrt(2), sqrt(2)] by:
 *
 *                      s = m * sqrt(2)
 *
 *        (5)   Compute z = (s - 1) / (s + 1)
 *
 *        (6)   Now use the approximation from HART
 *              page 111 to find log(s):
 *
 *              log(s) = z * ( P(z**2) / Q(z**2) )
 *
 *              Where:
 *
 *              P(z**2) =  SUM [ Pj * (z**(2*j)) ]
 *              over j = {0,1,2,3}
 *
 *              Q(z**2) =  SUM [ Qj * (z**(2*j)) ]
 *              over j = {0,1,2,3}
 *
 *              P0 =  -0.240139179559210509868484e2
 *              P1 =  0.30957292821537650062264e2
 *              P2 =  -0.96376909336868659324e1
 *              P3 =  0.4210873712179797145
 *              Q0 =  -0.120069589779605254717525e2
 *              Q1 =  0.19480966070088973051623e2
 *              Q2 =  -0.89111090279378312337e1
 *              Q3 =  1.0000
 *
 *              (coefficients from HART table #2705 pg 229)
 *
 *        (7)   Finally, compute log(x) from:
 *
 *              log(x) = (k * log(2)) - log(sqrt(2)) + log(s)
 */


void log(double x, double *retval)
{
/*  double log_pcoeffs[4];
  double log_qcoeffs[4];  */
  
  S16 k ;
  auto double pqofz, s, z, zt2 ;
 
  /*log_pcoeffs[0]=   -0.24013917955921050986e2;
  log_pcoeffs[1]=    0.30957292821537650062e2;
  log_pcoeffs[2]=   -0.96376909336868659324e1;
  log_pcoeffs[3]=    0.4210873712179797145;

  log_qcoeffs[0]=   -0.12006958977960525471e2;
  log_qcoeffs[1]=    0.19480966070088973051e2;
  log_qcoeffs[2]=   -0.89111090279378312337e1;
  log_qcoeffs[3]=    1.0000;*/



  if (x == 0.0)
    {
      doerr("log", "SINGULARITY", EDOM) ;
      *retval = -INF ;
    }
  else if (x < 0.0)
    {
      doerr("log", "DOMAIN", EDOM) ;
      *retval = -INF ;
    }
  else
    {
      s = SQRT2 * frexp(x, &k) ;
      z = (s - 1.0) / (s + 1.0) ;
      zt2 = z * z ;

      pqofz = z * (poly(3, log_pcoeffs, zt2) / poly(3, log_qcoeffs, zt2)) ;

      x = k * LN2 ;
      x -= LNSQRT2 ;
      x += pqofz ;
      *retval = x ;
    }
}

/*  FUNCTION
 *
 *      log10   double precision common log
 *
 *  DESCRIPTION
 *
 *      Returns double precision common log of double precision
 *      floating point argument.
 *
 *  USAGE
 *
 *      double log10(x)
 *      double x ;
 *
 *  REFERENCES
 *
 *      PDP-11 Fortran IV-plus users guide, Digital Equip. Corp.,
 *      1975, pp. B-3.
 *
 *  RESTRICTIONS
 *
 *      For precision information refer to documentation of the
 *      floating point library routines called.
 *
 *  INTERNALS
 *
 *      Computes log10(x) from:
 *
 *              log10(x) = log10(e) * log(x)
 */

void log10(double x, double *retval)
{
  log(x,retval);
  *retval*=LOG10E;
}

/*  FUNCTION
 *
 *      exp   double precision exponential
 *
 *  DESCRIPTION
 *
 *      Returns double precision exponential of double precision
 *      floating point number.
 *
 *  USAGE
 *
 *      double exp(x)
 *      double x ;
 *
 *  REFERENCES
 *
 *      Fortran IV plus user's guide, Digital Equipment Corp. pp B-3
 *
 *      Computer Approximations, J.F. Hart et al, John Wiley & Sons,
 *      1968, pp. 96-104.
 *
 *  RESTRICTIONS
 *
 *      Inputs greater than log(MAXDOUBLE) result in overflow.
 *      Inputs less than log(MINDOUBLE) result in underflow.
 *
 *      The maximum relative error for the approximating polynomial
 *      is 10**(-16.4).  However, this assumes exact arithmetic
 *      in the polynomial evaluation.  Additional rounding and
 *      truncation errors may occur as the argument is reduced
 *      to the range over which the polynomial approximation
 *      is valid, and as the polynomial is evaluated using
 *      finite precision arithmetic.
 *
 *
 *  INTERNALS
 *
 *      Computes exponential from:
 *
 *              exp(x)  =       2**y  *  2**z  *  2**w
 *
 *      Where:
 *
 *              y       =       int ( x * log2(e) )
 *
 *              v       =       16 * frac ( x * log2(e))
 *
 *              z       =       (1/16) * int (v)
 *
 *              w       =       (1/16) * frac (v)
 *
 *      Note that:
 *
 *              0 =< v < 16
 *
 *              z = {0, 1/16, 2/16, ...15/16}
 *
 *              0 =< w < 1/16
 *
 *      Then:
 *
 *              2**z is looked up in a table of 2**0, 2**1/16, ...
 *
 *              2**w is computed from an approximation:
 *
 *                      2**w    =  (A + B) / (A - B)
 *
 *                      A       =  C + (D * w * w)
 *
 *                      B       =  w * (E + (F * w * w))
 *
 *                      C       =  20.8137711965230361973
 *
 *                      D       =  1.0
 *
 *                      E       =  7.2135034108448192083
 *
 *                      F       =  0.057761135831801928
 *
 *              Coefficients are from HART, table #1121, pg 206.
 *
 *              Effective multiplication by 2**y is done by a
 *              floating point scale with y as scale argument.
 */

#define  C           20.8137711965230361973   /* Polynomial approx coeff. */
#define  D           1.0                      /* Polynomial approx coeff. */
#define  E           7.2135034108448192083    /* Polynomial approx coeff. */
#define  F           0.057761135831801928     /* Polynomial approx coeff. */

/************************************************************************
 *                                                                      *
 *      This table is fixed in size and reasonably hardware             *
 *      independent.  The given constants were generated on a           *
 *      DECSYSTEM 20 using double precision FORTRAN.                    *
 *                                                                      *
 ************************************************************************
 */

void exp(double x,double *retval)
{
  S16 ival, y; 
  auto double a, b, t, temp, v, w, wpof2, zpof2 ;

  if (x > LOGE_MAXDOUBLE)
    {
      doerr("exp", "OVERFLOW", ERANGE) ;
      *retval = INF;
    }
  else if (x <= LOGE_MINDOUBLE)
    {
      doerr("exp", "UNDERFLOW", ERANGE) ;
      *retval = 0.0 ;
    }
   else
    {
      t = x * LOG2E ;
      y=(S16)t;
      modf(t, &temp, &v);
	  v*=16.0;
	  abs(v,&temp);
      ival=(S16) temp;
      if (x < 0.0) zpof2 = 1.0 / fpof2tbl[ival] ;
      else         zpof2 = fpof2tbl[ival] ;
      modf(v, &temp,&w);
	  w/=16.0;
      a = C + (D * w * w) ;
      b = w * (E + (F * w * w)) ;
      wpof2 = (a + b) / (a - b) ;
      *retval = ldexp((wpof2 * zpof2), y) ;
    }
}

/* exp__E(x,c)
 * ASSUMPTION: c << x  SO THAT  fl(x+c)=x.
 * (c is the correction term for x)
 * exp__E RETURNS
 *
 *                       /  exp(x+c) - 1 - x ,  1E-19 < |x| < .3465736
 *       exp__E(x,c) =  |
 *                       \  0 ,  |x| < 1E-19.
 *
 * DOUBLE PRECISION (IEEE 53 bits, VAX D FORMAT 56 BITS)
 * KERNEL FUNCTION OF EXP, EXPM1, POW FUNCTIONS
 * CODED IN C BY K.C. NG, 1/31/85;
 * REVISED BY K.C. NG on 3/16/85, 4/16/85.
 *
 * Required system supported function:
 *      copysign(x,y)
 *
 * Method:
 *      1. Rational approximation. Let r=x+c.
 *         Based on
 *                                   2 * sinh(r/2)
 *                exp(r) - 1 =   ----------------------   ,
 *                               cosh(r/2) - sinh(r/2)
 *         exp__E(r) is computed using
 *                   x*x            (x/2)*W - ( Q - ( 2*P  + x*P ) )
 *                   --- + (c + x*[---------------------------------- + c ])
 *                    2                          1 - W
 *         where  P := p1*x^2 + p2*x^4,
 *                Q := q1*x^2 + q2*x^4 (for 56 bits precision, add q3*x^6)
 *                W := x/2-(Q-x*P),
 *
 *         (See the listing below for the values of p1,p2,q1,q2,q3. The poly-
 *          nomials P and Q may be regarded as the approximations to sinh
 *          and cosh :
 *              sinh(r/2) =  r/2 + r * P  ,  cosh(r/2) =  1 + Q . )
 *
 *         The coefficients were obtained by a special Remez algorithm.
 *
 * Approximation error:
 *
 *   |  exp(x) - 1                         |        2**(-57),  (IEEE double)
 *   | ------------  -  (exp__E(x,0)+x)/x  |  <=
 *   |       x                             |        2**(-69).  (VAX D)
 *
 * Constants:
 * The hexadecimal values are the intended ones for the following constants.
 * The decimal values may be used, provided that the compiler will convert
 * from decimal to binary accurately enough to produce the hexadecimal values
 * shown.
 */

double exp__E(double x,double c)
{
  double z, p, q, xp, xh, w ;

  if (copysign(x, 1.0) > 1.0E-19)
     {
       z = x * x ;
       p = z * (expe_p1 + z * expe_p2) ;

       q = z * (expe_q1 + z * expe_q2) ;
       xp = x * p ;
       xh = x * 0.5 ;
       w = xh - (q - xp) ;
       p += p ;
       c += x * ((xh * w - (q - (p + xp))) / (1.0 - w) + c) ;
       return(z * 0.5 + c) ;
     }

/* end of |x| > small */

   else
     {
       doerr("exp__E", "UNDERFLOW", ERANGE) ;
       return(0.0) ;
     }
}

/* log__L(Z)
 *              LOG(1+X) - 2S                          X
 * RETURN      ---------------  WHERE Z = S*S,  S = ------- , 0 <= Z <= .0294... *                    S                              2 + X
 *
 * DOUBLE PRECISION (VAX D FORMAT 56 bits or IEEE DOUBLE 53 BITS)
 * KERNEL FUNCTION FOR LOG; TO BE USED IN LOG1P, LOG, AND POW FUNCTIONS
 * CODED IN C BY K.C. NG, 1/19/85;
 * REVISED BY K.C. Ng, 2/3/85, 4/16/85.
 *
 * Method :
 *      1. Polynomial approximation: let s = x/(2+x).
 *         Based on log(1+x) = log(1+s) - log(1-s)
 *               = 2s + 2/3 s**3 + 2/5 s**5 + .....,
 *
 *         (log(1+x) - 2s)/s is computed by
 *
 *             z*(L1 + z*(L2 + z*(... (L7 + z*L8)...)))
 *
 *         where z=s*s. (See the listing below for Lk's values.) The
 *         coefficients are obtained by a special Remez algorithm.
 *
 * Accuracy:
 *      Assuming no rounding error, the maximum magnitude of the approximation
 *      error (absolute) is 2**(-58.49) for IEEE double, and 2**(-63.63)
 *      for VAX D format.
 *
 * Constants:
 * The hexadecimal values are the intended ones for the following constants.
 * The decimal values may be used, provided that the compiler will convert
 * from decimal to binary accurately enough to produce the hexadecimal values
 * shown.
 */


double log__L(double z)
{
  return(poly(7,log_l_L,z)) ;
}



/* pow_p(x,y) return x**y for x with sign = 1 and finite y */

double pow_p(double x,double y)
{
  #define SQRT0_5 	(1.0/SQRT2)
  #define LN2HI     6.9314718036912381649E-1    
  /* Hex  2^ -1   *  1.62E42FEE00000 */
  #define LN2LO     1.9082149292705877000E-10   
  /* Hex  2^-33   *  1.A39EF35793C76 */
  #define INVLN2    1.4426950408889633870E0     
  /* Hex  2^  0   *  1.71547652B82FE */

  double c, s, t, z, tx, ty ;

  double sx, sy ;
  long k = 0 ;
  S16 n, m,o ;

  if (x == 1.0) return(x) ;     /* if x = 1.0, return 1 since y is finite */

/* reduce x to z in [sqrt(1/2)-1, sqrt(2)-1] */

  /* Break x into mantissa from 0.5 .. 1 and dual exponent */
  z=frexp(x,&n);

  /* move the intervall from [0.5,sqrt(0.5)] to [1,sqrt(2)] */
  if (z <= SQRT0_5)
    {
      n -= 1 ;
      z *= 2 ;
    }
  z -= 1.0 ;

/* log(x) = nlog2 + log(1 + z) ~ nlog2 + t + tx */

  s = z / (2.0 + z) ;
  c = z * z * 0.5 ;
  tx = s * (c + log__L(s * s)) ;
  t = z - (c - tx) ;
  tx += (z - t) - c ;		/* tx==0 */

/* if y * log(x) is neither too big nor too small */
  (void) frexp(y,&m);
  (void) frexp(((double)n)+t,&o);

  if ((m+o) < 12)
    if ((m+o) > -60)
      {

		/* compute y * log(x) ~ mlog2 + t + c */

        s = y * ((double)n + INVLN2 * t) ;
        m = (S16) (s + copysign(0.5, s)) ;      /* m := nint(y * log(x)) */
		
		modf(y,&sx,&sy);
        if (sy==0.0)							/* if y is an integer */
          {
            k = (S32)m - ((S32) sy) * (S32)n ;
            sx = t ;
            tx += (t - sx) ;
          }
        else				                   /* if y is not an integer */
          {
            k = (S32) m ;
            tx += n * LN2LO ;
            c = (double)n * LN2HI;
			sx = c + t ;
            tx += (c - sx) + t ;				/* tx==0 !!! */
          }

		/* end of checking whether k == y */
        sy = y ;
        ty = y - sy ;							/* y ~ sy + ty, ty==0 */

        s = (double) sx * sy - (double)k * LN2HI ;  /* (sy + ty) * (sx + tx) - kln2 */

        z = (tx * ty - (double)k * LN2LO) ;
        tx = tx * sy ; ty = sx * ty ;
        t = ty + z ; t += tx ; t += s ;
        c = -((((t-s)-tx)-ty)-z) ;

/* return exp(y * log(x)) */

        t += exp__E(t,c) ;
        return(ldexp(1.0 + t, m)) ;
      }

/* end of if log(y * log(x)) > -60.0 */

	else                  /* exp(+- tiny) = 1 with inexact flag */
	{
		return(1.0) ;
    }
  else if (copysign(1.0, y) * (n + INVLN2 * t) < 0.0)
  {
      doerr("pow", "UNDERFLOW", ERANGE) ;
	  return(0.0) ;
  }
  else
  {
      doerr("pow", "OVERFLOW", ERANGE) ;
      return(INF) ;      
  }
}


/* POW(X,Y)
 * RETURN X**Y
 * DOUBLE PRECISION (VAX D format 56 bits, IEEE DOUBLE 53 BITS)
 * CODED IN C BY K.C. NG, 1/8/85;
 * REVISED BY K.C. NG on 7/10/85.
 *
 * Required system supported functions:
 *      scalb(x,n)
 *      logb(x)
 *      copysign(x,y)
 *      finite(x)
 *      drem(x,y)
 *
 * Required kernel functions:
 *      exp__E(a, c)    ...return  exp(a+c) - 1 - a*a/2
 *      log__L(x)       ...return  (log(1+x) - 2s)/s, s=x/(2+x)
 *      pow_p(x, y)     ...return  +(anything)**(finite non 
 )
 *
 * Method
 *      1. Compute and return log(x) in three pieces:
 *              log(x) = n*ln2 + hi + lo,
 *         where n is an integer.
 *      2. Perform y * log(x) by simulating muti-precision arithmetic and
 *         return the answer in three pieces:
 *              y * log(x) = m * ln2 + hi + lo,
 *         where m is an integer.
 *      3. Return x ** y = exp(y * log(x))
 *              = 2^m * (exp(hi + lo)).
 *
 * Special cases:
 *      (anything) ** 0  is 1 ;
 *      (anything) ** 1  is itself;
 *      (anything) ** NaN is NaN;
 *      NaN ** (anything except 0) is NaN;
 *      +-(anything > 1) ** +INF is +INF;
 *      +-(anything > 1) ** -INF is +0;
 *      +-(anything < 1) ** +INF is +0;
 *      +-(anything < 1) ** -INF is +INF;
 *      +-1 ** +-INF is NaN and signal INVALID;
 *      +0 ** +(anything except 0, NaN)  is +0;
 *      -0 ** +(anything except 0, NaN, odd integer)  is +0;
 *      +0 ** -(anything except 0, NaN)  is +INF and signal DIV-BY-ZERO;
 *      -0 ** -(anything except 0, NaN, odd integer)  is +INF with signal;
 *      -0 ** (odd integer) = -( +0 ** (odd integer) );
 *      +INF ** +(anything except 0,NaN) is +INF;
 *      +INF ** -(anything except 0,NaN) is +0;
 *      -INF ** (odd integer) = -( +INF ** (odd integer) );
 *      -INF ** (even integer) = ( +INF ** (even integer) );
 *      -INF ** -(anything except integer,NaN) is NaN with signal;
 *      -(x=anything) ** (k=integer) is (-1)**k * (x ** k);
 *      -(anything except 0) ** (non-integer) is NaN with signal;
 *
 * Accuracy:
 *      pow(x, y) returns x**y nearly rounded. In particular, on a SUN, a VAX,
 *      and a Zilog Z8000,
 *                      pow(integer, integer)
 *      always returns the correct integer provided it is representable.
 *      In a test run with 100,000 random arguments with 0 < x, y < 20.0
 *      on a VAX, the maximum observed error was 1.79 ulps (units in the
 *      last place).
 *
 * Constants :
 * The hexadecimal values are the intended ones for the following constants.
 * The decimal values may be used, provided that the compiler will convert
 * from decimal to binary accurately enough to produce the hexadecimal values
 * shown.
 */

void pow(double x,double y, double *retval)
{
	   if (y==0.0)
	   {
		   *retval=1.0;
	   }
       else if (x==0.0) 
	   {
		   *retval=0.0;
	   }
	   else if (y>=0.99999999999999 && y<=1.00000000000001)
	   {
		   *retval=x;
	   }
	   else if (y>=INF)
	   {
		   if(x>1.0)
		   {
			   *retval=copysign(INF,x);
  		       doerr("pow", "OVERFLOW", ERANGE) ;
		   }
		   else if (x<1.0)
		   {
			   *retval=0;
  		       doerr("pow", "UNDERFLOW", ERANGE) ;
		   }
		   else
		   {
			   *retval=copysign(1.0,x);
 		       doerr("pow", "NAN", EDOM) ;
		   }
	   }
	   else if (y<=-INF)
	   {
		   if(x>1.0)
		   {
			   *retval=0;
  		       doerr("pow", "UNDERFLOW", ERANGE) ;
		   }
		   else if (x<1.0)
		   {
			   *retval=copysign(INF,x);
  		       doerr("pow", "OVERFLOW", ERANGE) ;
		   }
		   else
		   {
			   *retval=1.0;
 		       doerr("pow", "NAN", EDOM) ;
		   }
	   }
	   else if (x>=INF)
	   {
		   *retval=INF;
	       doerr("pow", "NAN", EDOM) ;
	   }
	   else if (x<=-INF)
	   {
		   *retval=-INF;
		   doerr("pow", "NAN", EDOM) ;
	   }

	  else if (y>=1.99999999999999 && y<=2.00000000000001) 
	  {
		  *retval=x * x;
	  }
	  else if (y>= -1.00000000000001 && y<=-0.99999999999999) 
	  {

		  *retval= 1.0 / x;
	  }
	  /* positive number */
	  else if (!(*((U8 *)&x) & 128)) 
	  {
		  *retval=pow_p(x, y);
	  }

/* negative number */

	  else 
	  {
		  /* if y is an even integer */
		  mod(y,2.0,retval);
	      if (*retval == 0.0)
		  {
			  *retval=pow_p(-x, y);
		  }
		  /* if y is an odd integer */
		  else if (copysign(*retval, 1.0) == 1.0) 
		  {
			  *retval=-pow_p(-x, y) ;
		  }
		  /* Henceforth y is not an integer */
		  else 
		  {
			  *retval=INF;
   		       doerr("pow", "NAN", EDOM) ;
		  }
	  }
}




/************************************************************/
/* Trigonometric functions									*/
/************************************************************/

/*  FUNCTION
 *
 *      cos  -  double precision cosine
 *
 *  DESCRIPTION
 *
 *      Returns double precision cosine of double precision
 *      floating point argument.
 *
 *  USAGE
 *
 *      double cos(x)
 *      double x ;
 *
 *  REFERENCES
 *
 *      Computer Approximations, J.F. Hart et al, John Wiley & Sons,
 *      1968, pp. 112-120.
 *
 *  RESTRICTIONS
 *
 *      The sin and cos routines are interactive in the sense that
 *      in the process of reducing the argument to the range -PI/4
 *      to PI/4, each may call the other.  Ultimately one or the
 *      other uses a polynomial approximation on the reduced
 *      argument.  The sin approximation has a maximum relative error
 *      of 10**(-17.59) and the cos approximation has a maximum
 *      relative error of 10**(-16.18).
 *
 *      These error bounds assume exact arithmetic
 *      in the polynomial evaluation.  Additional rounding and
 *      truncation errors may occur as the argument is reduced
 *      to the range over which the polynomial approximation
 *      is valid, and as the polynomial is evaluated using
 *      finite-precision arithmetic.
 *
 *  INTERNALS
 *
 *      Computes cos(x) from:
 *
 *              (1)     Reduce argument x to range -PI to PI.
 *
 *              (2)     If x > PI/2 then call cos recursively
 *                      using relation cos(x) = -cos(x - PI).
 *
 *              (3)     If x < -PI/2 then call cos recursively
 *                      using relation cos(x) = -cos(x + PI).
 *
 *              (4)     If x > PI/4 then call sin using
 *                      relation cos(x) = sin(PI/2 - x).
 *
 *              (5)     If x < -PI/4 then call cos using
 *                      relation cos(x) = sin(PI/2 + x).
 *
 *              (6)     If x would cause underflow in approx
 *                      evaluation arithmetic then return
 *                      sqrt(1.0 - x**2).
 *
 *              (7)     By now x has been reduced to range
 *                      -PI/4 to PI/4 and the approximation
 *                      from HART pg. 119 can be used:
 *
 *                      cos(x) = ( p(y) / q(y) )
 *                      Where:
 *
 *                      y    =  x * (4/PI)
 *
 *                      p(y) =  SUM [ Pj * (y**(2*j)) ]
 *                      over j = {0,1,2,3}
 *
 *                      q(y) =  SUM [ Qj * (y**(2*j)) ]
 *                      over j = {0,1,2,3}
 *
 *                      P0   =  0.12905394659037374438571854e+7
 *                      P1   =  -0.3745670391572320471032359e+6
 *                      P2   =  0.134323009865390842853673e+5
 *                      P3   =  -0.112314508233409330923e+3
 *                      Q0   =  0.12905394659037373590295914e+7
 *                      Q1   =  0.234677731072458350524124e+5
 *                      Q2   =  0.2096951819672630628621e+3
 *                      Q3   =  1.0000...
 *                      (coefficients from HART table #3843 pg 244)
 *
 *
 *      **** NOTE ****    The range reduction relations used in
 *      this routine depend on the final approximation being valid
 *      over the negative argument range in addition to the positive
 *      argument range.  The particular approximation chosen from
 *      HART satisfies this requirement, although not explicitly
 *      stated in the text.  This may not be true of other
 *      approximations given in the reference.
 */


void cos(double x, double *retval)
{
  U8 sign;

  sign=0;
  /* Cosine is a symmetric function, so we can get rid of the sign, saves some comparisons */
  abs(x,&x);
  if (x > PI)
  {
      mod(x, TWOPI, &x) ;
      if (x > PI)  x = TWOPI-x ;
  }    
  if (x > HALFPI)   
  {
	x=PI-x;
	/* For this adaptation we must change the results sign */
	sign=1;
  }
  if (x > FOURTHPI)  
  {
	  sin(HALFPI - x,retval) ;
  }
  else if (x < 1e-16)
    sqrt(1.0 - (x * x),retval) ;
  else
    {
      x = x / FOURTHPI ;
      x =x*x ;
      *retval = poly(3, cos_pcoeffs, x) / poly(3, cos_qcoeffs, x) ;
    }
  /* Did we do a transformation, which needed a sign change ? */
  if (sign)
  {
	  *retval=-*retval;
  }
}

/*  FUNCTION
 *
 *  sin  double precision sine
 *
 *  DESCRIPTION
 *
 *  Returns double precision sine of double precision
 *  floating point argument.
 *
 *  USAGE
 *
 *  double sin(x)
 *  double x ;
 *
 *  REFERENCES
 *
 *  Computer Approximations, J.F. Hart et al, John Wiley & Sons,
 *  1968, pp. 112-120.
 *
 *  RESTRICTIONS
 *
 *  The sin and cos routines are interactive in the sense that
 *  in the process of reducing the argument to the range -PI/4
 *  to PI/4, each may call the other.  Ultimately one or the
 *  other uses a polynomial approximation on the reduced
 *  argument.  The sin approximation has a maximum relative error
 *  of 10**(-17.59) and the cos approximation has a maximum
 *  relative error of 10**(-16.18).
 *
 *  These error bounds assume exact arithmetic
 *  in the polynomial evaluation.  Additional rounding and
 *  truncation errors may occur as the argument is reduced
 *  to the range over which the polynomial approximation
 *  is valid, and as the polynomial is evaluated using
 *  finite-precision arithmetic.
 *  
 *  INTERNALS
 *
 *  Computes sin(x) from:
 *
 *    (1)  Reduce argument x to range -PI to PI.
 *
 *    (2)  If x > PI/2 then call sin recursively
 *    using relation sin(x) = -sin(x - PI).
 *
 *    (3)  If x < -PI/2 then call sin recursively
 *    using relation sin(x) = -sin(x + PI).
 *
 *    (4)  If x > PI/4 then call cos using
 *    relation sin(x) = cos(PI/2 - x).
 *
 *    (5)  If x < -PI/4 then call cos using
 *    relation sin(x) = -cos(PI/2 + x).
 *
 *    (6)  If x is small enough that polynomial
 *    evaluation would cause underflow
 *    then return x, since sin(x)
 *    approaches x as x approaches zero.
 *
 *    (7)  By now x has been reduced to range
 *    -PI/4 to PI/4 and the approximation
 *    from HART pg. 118 can be used:
 *
 *    sin(x) = y * ( p(y) / q(y) )
 *    Where:
 *
 *    y    =  x * (4/PI)
 *
 *    p(y) =  SUM [ Pj * (y**(2*j)) ]
 *    over j = {0,1,2,3}
 *
 *    q(y) =  SUM [ Qj * (y**(2*j)) ]
 *    over j = {0,1,2,3}
 *
 *    P0   =  0.206643433369958582409167054e+7
 *    P1   =  -0.18160398797407332550219213e+6
 *    P2   =  0.359993069496361883172836e+4
 *    P3   =  -0.2010748329458861571949e+2
 *    Q0   =  0.263106591026476989637710307e+7
 *    Q1   =  0.3927024277464900030883986e+5
 *    Q2   =  0.27811919481083844087953e+3
 *    Q3   =  1.0000...
 *    (coefficients from HART table #3063 pg 234)
 *
 *
 *  **** NOTE ****    The range reduction relations used in
 *  this routine depend on the final approximation being valid
 *  over the negative argument range in addition to the positive
 *  argument range.  The particular approximation chosen from
 *  HART satisfies this requirement, although not explicitly
 *  stated in the text.  This may not be true of other
 *  approximations given in the reference.
 */


void sin(double x, double *retval)
{
  double yt2 ;
  double sign1;

  /* Save the sign of x */
  sign1=copysign(1.0,x);
  /* and work with the absolute value of x, saves some comparisons */
  abs(x,&x);

  if (x > PI)
    {
      mod(x, TWOPI, &x) ;
           if (x > PI)  
		   {
			   x = TWOPI-x ;
			   /* x=x-2*PI, we don't want to get negative here, so use again sin(-x)=-sin(x) */
			   /* that means, we have to invert the sign found above						 */
			   sign1=-sign1;
			}
	}

   if (x > HALFPI)
	   {
		    /* use the symmetry of one sine half wave to PI/2 */
			/* we don't need a sign change here				  */
		    x=PI-x;
	   }

   if (x > FOURTHPI)  
   {
	   cos(HALFPI - x,retval) ;
   }
   else if (x < 1e-16)
   {
	   *retval = x ;
   }
  else
    {
      x/= FOURTHPI ;
      yt2 = x * x ;
      *retval = x * (poly(3, sin_pcoeffs, yt2) / poly(3, sin_qcoeffs, yt2)) ;
    }
  /* Apply the sign changes, we have made before */
  if (*((U8 *)&sign1) & 128)
  {
	  *retval=-*retval;
  }
}

/*  FUNCTION
 *
 *      tan   Double precision tangent
 *
 *  DESCRIPTION
 *
 *      Returns tangent of double precision floating point number.
 *
 *  USAGE
 *
 *      double tan(x)
 *      double x ;
 *
 *  INTERNALS
 *
 *      Computes the tangent from tan(x) = sin(x) / cos(x).
 *
 *      If cos(x) = 0 and sin(x) >= 0, then returns largest
 *      floating point number on host machine.
 *
 *      If cos(x) = 0 and sin(x) < 0, then returns smallest
 *      floating point number on host machine.
 *
 *  REFERENCES
 *
 *      Fortran IV plus user's guide, Digital Equipment Corp. pp. B-8
 */

void tan(double x, double *retval)
{
  double cosx;

  cos(x, &cosx) ;
  if (cosx == 0.0)
    {
      doerr("tan", "OVERFLOW", ERANGE) ;
      mod(x,TWOPI,&x);
	  if (x <= PI) *retval = INF;
      else         *retval = -INF;
    }
  else 
  {
	  sin(x,retval);
	  *retval /= cosx ;
  }
}


/************************************************************/
/* inverse Trigonometric functions							*/
/************************************************************/

/*  FUNCTION
 *
 *      atan   double precision arc tangent
 *
 *  DESCRIPTION
 *
 *      Returns double precision arc tangent of double precision
 *      floating point argument.
 *
 *  USAGE
 *
 *      double atan(x)
 *      double x ;
 *
 *  REFERENCES
 *
 *      Fortran 77 user's guide, Digital Equipment Corp. pp B-3
 *
 *      Computer Approximations, J.F. Hart et al, John Wiley & Sons,
 *      1968, pp. 120-130.
 *
 *  RESTRICTIONS
 *
 *      The maximum relative error for the approximating polynomial
 *      is 10**(-16.82).  However, this assumes exact arithmetic
 *      in the polynomial evaluation.  Additional rounding and
 *      truncation errors may occur as the argument is reduced
 *      to the range over which the polynomial approximation
 *      is valid, and as the polynomial is evaluated using
 *      finite-precision arithmetic.
 *
 *  INTERNALS
 *
 *      Computes arctangent(x) from:
 *
 *              (1)     If x < 0 then negate x, perform steps
 *                      2, 3, and 4, and negate the returned
 *                      result.  This makes use of the identity
 *                      atan(-x) = -atan(x).
 *
 *              (2)     If argument x > 1 at this point then
 *                      test to be sure that x can be inverted
 *                      without underflowing.  If not, reduce
 *                      x to largest possible number that can
 *                      be inverted, issue warning, and continue.
 *                      Perform steps 3 and 4 with arg = 1/x
 *                      and subtract returned result from
 *                      pi/2.  This makes use of the identity
 *                      atan(x) = pi/2 - atan(1/x) for x>0.
 *						The case of underflowing can't happen, since the
 *						range of small numbers is grater than the range of 
 *						large numbers in IC35 Floatpoint numbers 
 *						(Biggest number: ~256*8^62=2.51*10^58
 *						 inverse: 3.9827*10-59
 *						 smallest number 1*8^-65=1.99136*10^-59)
 *
 *              (3)     At this point 0 <= x <= 1.  If
 *                      x > tan(pi/12) then perform step 4
 *                      with arg = (x*sqrt(3)-1)/(sqrt(3)+x)
 *                      and add pi/6 to returned result.  This
 *                      last transformation maps arguments
 *                      greater than tan(pi/12) to arguments
 *                      in range 0...tan(pi/12).
 *
 *              (4)     At this point the argument has been
 *                      transformed so that it lies in the
 *                      range -tan(pi/12)...tan(pi/12).
 *                      Since very small arguments may cause
 *                      underflow in the polynomial evaluation,
 *                      a final check is performed.  If the
 *                      argument is less than the underflow
 *                      bound, the function returns x, since
 *                      atan(x) approaches asin(x) which
 *                      approaches x, as x goes to zero.
 *
 *              (5)     atan(x) is now computed by one of the
 *                      approximations given in the cited
 *                      reference (Hart).  That is:
 *
 *                      atan(x) = x * SUM [ C[i] * x**(2*i) ]
 *                      over i = {0,1,...8}.
 *
 *                      Where:
 *
 *                      C[0] =  .9999999999999999849899
 *                      C[1] =  -.333333333333299308717
 *                      C[2] =  .1999999999872944792
 *                      C[3] =  -.142857141028255452
 *                      C[4] =  .11111097898051048
 *                      C[5] =  -.0909037114191074
 *                      C[6] =  .0767936869066
 *                      C[7] =  -.06483193510303
 *                      C[8] =  .0443895157187
 *                      (coefficients from HART table #4945 pg 267)
 */

#define  LAST_BOUND  0.2679491924311227074725     /*  tan (PI/12) */

void atan(double x,double *retval)
{
  double t1, t2, xt2,sign ;
  U8 flag;
  flag=0;
  /* Save the sign of x */
  sign=copysign(1.0,x);
  /* use the absolute value of x */
  abs(x,&x);
  if (x > 1.0)
    {
	  flag=1;
	  x=1/x;
    }
  if (x > LAST_BOUND)
    {
      t1 = x * SQRT3 - 1.0 ;
      t2 = SQRT3 + x ;
      x=(t1 / t2);
	  /* Did this transformation give a negative result ? */
	  if (*((U8 *)&x) & 128)
	  {
		  /* remove the negative sign */
		  *((U8 *)&x)&=127;
		  /* We must do a different correction in this case at the end */
		  flag|=4;
	  }
	  else
	  {
		  flag|=2;
	  }
    }
  if (x < 1e-10)
    {
      *retval = x ;
    }
  else
    {
      xt2 = x * x ;
      *retval = x * poly(8, atan_coeffs, xt2) ;
    }

  if (flag)
  {
	  /* Did we have to do the transformation from tan(PI/12)..1 */
	  if (flag & 2)
	  {
		  /* Add PI/6 to the result */
		  *retval+=SIXTHPI;
	  }
	  else if (flag & 4)
	  {
		  /* correct the negative sign */
		  *retval=SIXTHPI-*retval;
	  }
	  /* Did we have to do the transformation from 1..inf */
	  if (flag &1)
	  {
		  /* Subtract result from PI/2 */
		  *retval=HALFPI-*retval;
	  }
  }

  /* Correct the sign */
  *retval=copysign(*retval,sign);
}

/*
 *  FUNCTION
 *
 *      asin   double precision arc sine
 *
 *  DESCRIPTION
 *
 *      Returns double precision arc sine of double precision
 *      floating point argument.
 *
 *      If argument is less than -1.0 or greater than +1.0, calls
 *      doerr with a DOMAIN error.  If doerr does not handle
 *      the error then prints error message and returns 0.
 *
 *  USAGE
 *
 *      double asin(x)
 *      double x ;
 *
 *  REFERENCES
 *
 *      Fortran IV-plus user's guide, Digital Equipment Corp. pp B-2.
 *
 *  RESTRICTIONS
 *
 *      For precision information refer to documentation of the floating
 *      point library primatives called.
 *
 *  INTERNALS
 *
 *      Computes arcsine(x) from:
 *
 *              (1)     If x < -1.0 or x > +1.0 then
 *                      call doerr and return 0.0 by default.
 *
 *              (2)     If x = 0.0 then asin(x) = 0.0
 *
 *              (3)     If x = 1.0 then asin(x) = PI/2.
 *
 *              (4)     If x = -1.0 then asin(x) = -PI/2
 *
 *              (5)     If -1.0 < x < 1.0 then
 *                      asin(x) = atan(y)
 *                      y = x / sqrt[1-(x**2)]
 */

void asin(double x,double *retval)
{
  if ( x > 1.0 || x < -1.0)
    {
      doerr("asin", "DOMAIN", EDOM) ;
      *retval = 0.0 ;
    }
  else if (x == 0.0) *retval = 0.0 ;
  else if (x == 1.0) *retval = HALFPI ;
  else if (x == -1.0) *retval = -HALFPI ;
  else 
  {
	  sqrt (1.0 - (x * x),retval);
	  atan(x / *retval,retval);
  }
}

/*  FUNCTION
 *
 *      acos   double precision arc cosine
 *
 *  DESCRIPTION
 *
 *      Returns double precision arc cosine of double precision
 *      floating point argument.
 *
 *  USAGE
 *
 *      double acos(x)
 *      double x ;
 *
 *  REFERENCES
 *
 *      Fortran IV-plus user's guide, Digital Equipment Corp. pp B-1.
 *
 *  RESTRICTIONS
 *
 *      The maximum relative error for the approximating polynomial
 *      in atan is 10**(-16.82).  However, this assumes exact arithmetic
 *      in the polynomial evaluation.  Additional rounding and
 *      truncation errors may occur as the argument is reduced
 *      to the range over which the polynomial approximation
 *      is valid, and as the polynomial is evaluated using
 *      finite-precision arithmetic.
 *
 *  INTERNALS
 *
 *      Computes arccosine(x) from:
 *
 *              (1)     If x < -1.0  or x > +1.0 then call
 *                      doerr and return 0.0 by default.
 *
 *              (2)     If x = 0.0 then acos(x) = PI/2.
 *
 *              (3)     If x = 1.0 then acos(x) = 0.0
 *
 *              (4)     If x = -1.0 then acos(x) = PI.
 *
 *              (5)     If 0.0 < x < 1.0 then
 *                      acos(x) = atan(Y)
 *                      Y = sqrt[1-(x**2)] / x
 *
 *              (6)     If -1.0 < x < 0.0 then
 *                      acos(x) = atan(Y) + PI
 *                      Y = sqrt[1-(x**2)] / x
 */

void acos(double x, double *retval)
{
  if (x > 1.0 || x < -1.0)
    {
      doerr("acos", "DOMAIN", EDOM) ;
      *retval = 0.0 ;
    }
  else if (x == 0.0)  *retval = HALFPI ;
  else if (x == 1.0)  *retval = 0.0 ;
  else if (x == -1.0) *retval = PI ;
  else
    {
	  sqrt(1.0 - (x * x),retval);
      atan( *retval/ x,retval);
      if (x < 0.0) *retval = *retval + PI  ;
    }    
}

/************************************************************/
/* Hyperbolic functions										*/
/************************************************************/

/*  FUNCTION
 *
 *      cosh   double precision hyperbolic cosine
 *
 *  DESCRIPTION
 *
 *      Returns double precision hyperbolic cosine of double precision
 *      floating point number.
 *
 *  USAGE
 *
 *      double cosh(x)
 *      double x ;
 *
 *  REFERENCES
 *
 *      Fortran IV plus user's guide, Digital Equipment Corp. pp B-4
 *
 *  RESTRICTIONS
 *
 *		Since cosh is a symmetric function if absolute value of input argument exceeds
 *		log(madouble), an overflow is generated 
 *
 *      For precision information refer to documentation of the
 *      floating point library routines called.
 *
 *  INTERNALS
 *
 *      Computes hyperbolic cosine from:
 *
 *              cosh(X) = 0.5 * (exp(X) + exp(-X))
 */


void cosh(double x, double *retval)
{
  abs(x,&x);
  if (x > LOGE_MAXDOUBLE)
    {
      doerr("cosh", "OVERFLOW", ERANGE) ;
      *retval = INF ;
    }
  else
    {
      exp(x,&x) ;
      *retval = 0.5 * (x + 1.0 / x) ;
    }
}


/*  FUNCTION
 *
 *      sinh   double precision hyperbolic sine
 *
 *  DESCRIPTION
 *
 *      Returns double precision hyperbolic sine of double precision
 *      floating point number.
 *
 *  USAGE
 *
 *      double sinh(x)
 *      double x ;
 *
 *  REFERENCES
 *
 *      Fortran IV plus user's guide, Digital Equipment Corp. pp B-5
 *
 *  RESTRICTIONS
 *
 *      Inputs greater than ln(MAXDOUBLE) result in overflow.
 *      Inputs less than ln(MINDOUBLE) result in underflow.
 *
 *      For precision information refer to documentation of the
 *      floating point library routines called.
 *
 *  INTERNALS
 *
 *      Computes hyperbolic sine from:
 *
 *              sinh(x) = 0.5 * (exp(x) - 1.0/exp(x))
 */

void sinh(double x, double *retval)
{
  if (x > LOGE_MAXDOUBLE)
    {
      doerr("sinh", "OVERFLOW", ERANGE) ;
      *retval = INF ;
    }
  else if (x < -LOGE_MAXDOUBLE)
    {
      doerr("sinh", "OVERFLOW", ERANGE) ;
      *retval = -INF ;
    }
  else
    {
      exp(x,&x) ;
      *retval = 0.5 * (x - 1.0 / x) ;
    }
}

/*  FUNCTION
 *
 *  tanh   double precision hyperbolic tangent
 *
 *  DESCRIPTION
 *
 *  Returns double precision hyperbolic tangent of double precision
 *  floating point number.
 *
 *  USAGE
 *
 *  double tanh(x)
 *  double x ;
 *
 *  REFERENCES
 *
 *  Fortran IV plus user's guide, Digital Equipment Corp. pp B-5
 *
 *  RESTRICTIONS
 *
 *  For precision information refer to documentation of the
 *  floating point library routines called.
 *  
 *  INTERNALS
 *
 *  Computes hyperbolic tangent from:
 *
 *    tanh(x) = 1.0 for x > TANH_MAXARG
 *      = -1.0 for x < -TANH_MAXARG
 *      =  sinh(x) / cosh(x) otherwise
 */

void tanh(double x, double *retval)
{
  double y;

  if (x > 40 || x < -40)
    {
      if (x > 0.0) *retval=1.0;
      else *retval=-1.0 ;
      doerr("tanh", "PLOSS", ERANGE) ;
    }
  else
  {
	    exp(x,&x) ;
		y=1.0/x;
		*retval = (x-y) / (x+y) ;
  }
}


/************************************************************/
/* inverse Hyperbolic functions								*/
/************************************************************/

/*  FUNCTION
 *
 *      asinh   double precision hyperbolic arc sine
 *
 *  DESCRIPTION
 *
 *      Returns double precision hyperbolic arc sine of double precision
 *      floating point number.
 *
 *  USAGE
 *
 *      double asinh(x)
 *      double x ;
 *
 *  RESTRICTIONS
 *
 *
 *      For precision information refer to documentation of the
 *      floating point library routines called.
 *
 *  INTERNALS
 *
 *      Computes asinh(x) from:
 *
 *
 *              1.      asinh(x) = sign(x)*(log [|x|+sqrt(x**2 + 1)])
 *				2.		asinh(x) ~ sign(x)*(log (|x|)+log(2)), x>>1
 */

void asinh(double x, double *retval)
{
  double sign=1.0;
  /* save the sign */
  sign=copysign(sign,x);
  /* The calculations can be done with the absolute value */
  abs(x,&x);
  if (x > 1e10)
    {
      log(x,retval);
	  *retval+=LN2;
	  *retval=copysign(*retval,sign);
    }
  else
  {
    sqrt(x*x + 1.0,retval);
    log(x + *retval,retval);
	*retval=copysign(*retval,sign);
  }
}

/*  FUNCTION
 *
 *      acosh   double precision hyperbolic arc cosine
 *
 *  DESCRIPTION
 *
 *      Returns double precision hyperbolic arc cosine of double precision
 *      floating point number.
 *
 *  USAGE
 *
 *      double acosh(x)
 *      double x ;
 *
 *  RESTRICTIONS
 *
 *      For precision information refer to documentation of the
 *      floating point library primatives called.
 *
 *  INTERNALS
 *
 *      Computes acosh(x) from:
 *
 *              1.      If x < 1.0 then report illegal
 *                      argument and return zero.
 *
 *              2.      acosh(x) = log [x+sqrt(x**2 - 1)]
 */

void acosh(double x, double *retval)
{

  if (x < 1.0)
    {
      doerr("acosh", "DOMAIN", ERANGE) ;
      *retval = 0.0 ;
    }
  else if (x > 1e10)
    {
      log(x,retval);
	  *retval+=LN2;
    }
  else 
  {
	  sqrt(x*x - 1.0,retval);
	  log(x + *retval,retval) ;
  }
}

/*  FUNCTION
 *
 *      atanh   double precision hyperbolic arc tangent
 *
 *  DESCRIPTION
 *
 *      Returns double precision hyperbolic arc tangent of double precision
 *      floating point number.
 *
 *  USAGE
 *
 *      double atanh(x)
 *      double x ;
 *
 *  RESTRICTIONS
 *
 *      The range of the atanh function is -1.0 to +1.0 exclusive.
 *      Certain pathological cases near 1.0 may cause overflow
 *      in evaluation of 1+x / 1-x, depending upon machine exponent
 *      range and mantissa precision.
 *
 *      For precision information refer to documentation of the
 *      other floating point library routines called.
 *
 *  INTERNALS
 *
 *      Computes atanh(x) from:
 *
 *              1.      If x <= -1.0 or x >= 1.0
 *                      then report argument out of range and return 0.0
 *
 *              2.      atanh(x) = 0.5 * log((1+x)/(1-x))
 */

void atanh(double x, double *retval)
{

  if (x <= -1.0 || x >= 1.0)
    {
      doerr("atan", "DOMAIN", ERANGE) ;
      *retval = 0.0 ;
    }
  else 
  {
	  log((1.0+x) / (1.0-x),retval);
	  *retval*=0.5;
  }
}
