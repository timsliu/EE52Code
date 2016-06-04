/* math.h - Math function declarations and macro definitions. */
/* Copyright 1988 Intel Corporation */

#if !defined(_MATHH)
#define _MATHH

/* iC-86 V4.0 interface macros & #pragma's */
#define acos	acos_
#define asin	asin_
#define atan	atan_
#define atan2	atan2_
#define cabs	cabs_
#define cos	cos_
#define cosh	cosh_
#define ceil	ceil_
#define exp	exp_
#define fabs	fabs_
#define floor	floor_
#define frexp	frexp_
#define hypot	hypot_
#define j0	j0_
#define j1	j1_
#define jn	jn_
#define ldexp	ldexp_
#define log	log_
#define log10	log10_
#define modf	modf_
#define pow	pow_
#define sin	sin_
#define sinh	sinh_
#define sqrt	sqrt_
#define tan	tan_
#define tanh	tanh_

#pragma VARPARAMS(acos_)
#pragma VARPARAMS(asin_)
#pragma VARPARAMS(atan_)
#pragma VARPARAMS(atan2_)
#pragma VARPARAMS(cabs_)
#pragma VARPARAMS(ceil_)
#pragma VARPARAMS(cos_)
#pragma VARPARAMS(cosh_)
#pragma VARPARAMS(exp_)
#pragma VARPARAMS(fabs_)
#pragma VARPARAMS(floor_)
#pragma VARPARAMS(frexp_)
#pragma VARPARAMS(hypot_)
#pragma VARPARAMS(j0_)
#pragma VARPARAMS(j1_)
#pragma VARPARAMS(jn_)
#pragma VARPARAMS(ldexp_)
#pragma VARPARAMS(log_)
#pragma VARPARAMS(log10_)
#pragma VARPARAMS(modf_)
#pragma VARPARAMS(pow_)
#pragma VARPARAMS(sin_)
#pragma VARPARAMS(sinh_)
#pragma VARPARAMS(sqrt_)
#pragma VARPARAMS(tan_)
#pragma VARPARAMS(tanh_)

/* Definitions for math functions. */
#define HUGE_VAL	1e+37	/* Infinity */
#define L2I		127.0	/* log2(infinity) */
#define L10P		17	/* log10(precision) */
#define L2L2P		6	/* log2(log2(precision)) */

/* Constants. */
#define PI	0.31415926535897932e+01
#define SQRT2	0.14142135623730950e+01
#define LOG2B10	0.30102999566398119e+00
#define LOG10BE	0.23025850929940456e+01
#define LOG10B2	0.33219280948873623e+01
#define LOGEB2	0.14426950408889634e+01

/* Complex variables. */
typedef struct cpx {
	double	z_r;
	double	z_i;
} CPX;

/* Internal functions. */
double	_pol();
double	_two();

/* Function definitions. */
double	acos(double);
double	asin(double);
double	atan(double);
double	atan2(double, double);
double	cabs(CPX);		/* non-ANSI function */
double	ceil(double);
double	cos(double);
double	cosh(double);
double	exp(double);
double	fabs(double);
double	floor(double);
double	frexp(double value, int *expr);
double	hypot(double, double);	/* non-ANSI function */
double	j0(double);		/* non-ANSI function */
double	j1(double);		/* non-ANSI function */
double	jn(int, double);	/* non-ANSI function */
double	ldexp(double, int);
double	log(double);
double	log10(double);
double	modf(double, double *);
double	pow(double, double);
double	sin(double);
double	sinh(double);
double	sqrt(double);
double	tan(double);
double	tanh(double);
#endif
