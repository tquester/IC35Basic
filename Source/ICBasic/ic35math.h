#include <ic35sdk.h>

extern void abs(double,double *);
extern void sqrt(double,double *);
extern void trunc(double,double *);
extern void round(double,double *);
extern void floor(double,double *);
extern void ceil(double,double *);
extern void modf(double,double *,double*);
extern void mod(double,double,double*);

extern void log(double,double *);
extern void log10(double,double *);
extern void exp(double,double *);
extern void pow(double,double,double*);
extern void sinh(double,double *);
extern void cosh(double,double *);
extern void tanh(double,double *);

extern void asinh(double,double *);
extern void acosh(double,double *);
extern void atanh(double,double *);

extern void sin(double,double *);
extern void cos(double,double *);
extern void tan(double,double *);
extern void asin(double,double *);
extern void acos(double,double *);
extern void atan(double,double *);

extern double test(double);

// extern void poly(U8,double *, double, double *);

extern void doerr(char *, char *, U8);


#define INF       2.5108406935700716506019546019e+58

#define  SQRT_MAXDOUBLE  1.584563e29
#define  LOGE_MAXDOUBLE  134.4705530
#define  LOGE_MINDOUBLE  -135.162


#define  PI			     3.1415926535897932384
#define  TWOPI           (2.0 * PI)
#define  HALFPI          (PI / 2.0)
#define  FOURTHPI        (PI / 4.0)
#define  SIXTHPI         (PI / 6.0)
#define  LOG2E           1.4426950408889634074   /* Log to base 2 of e */
#define  LOG10E          0.4342944819032518276
#define  SQRT2           1.4142135623730950488
#define  SQRT3           1.7320508075688772935
#define  LN2             0.6931471805599453094
#define  LNSQRT2         0.3465735902799726547

#define EDOM            33
#define ERANGE          34

