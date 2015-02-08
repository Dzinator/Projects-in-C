/*
  continued.h
 */

#include <iostream>
#ifndef CONTINUED_H_
#define CONTINUED_H_


/** 0-credit questions */

unsigned int gcd(unsigned int, unsigned int);
unsigned int gcdFaster(unsigned int, unsigned int);

/** QUESTION 3 ***/

struct ContinuedFraction {
	int head;
	ContinuedFraction *tail;
};

ContinuedFraction *getCFlargerThanOne(unsigned int, unsigned int);


/** QUESTION 4 ***/

ContinuedFraction *getCF(unsigned int, unsigned int);

/** QUESTION 5 ***/

ContinuedFraction *getCF(unsigned int, ContinuedFraction*, ContinuedFraction*);

/** QUESTION 6 ***/

struct Fraction {
	int numerator;
	int denominator;
};

Fraction getApproximation(ContinuedFraction *fr, unsigned int n);

#endif /* CONTINUED_H_ */
