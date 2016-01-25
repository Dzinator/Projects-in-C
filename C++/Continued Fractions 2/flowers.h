/*
 * flowers.h
 */

#include <iostream>
#include <vector>
#include <list>

#ifndef FLOWERS_H_
#define FLOWERS_H_


/** 0-credit questions */

struct ContinuedFraction {
	   std::vector<int> fixedPart;
	   std::vector<int> periodicPart;
};

/* definitions in flowersExamples.cpp*/
unsigned int spit(ContinuedFraction &,unsigned int);
unsigned int spitGolden(unsigned int);
unsigned int spitSqrt(unsigned int);
unsigned int spitEuler(unsigned int);

/* TODO - in flowersStudent.cpp*/

/** QUESTION 1 ***/

ContinuedFraction fractionalPart(ContinuedFraction &f);

/** QUESTION 2 ***/

unsigned int spitSqrt8(unsigned int);

/** QUESTION 3 ***/

unsigned int spitEulerSquare(unsigned int);


/** QUESTION 4 ***/

struct Fraction {
    int numerator;
    int denominator;
};

Fraction getApproximation(ContinuedFraction &fr, unsigned int n);

double getAngle(ContinuedFraction &theta, int k);

/** QUESTION 5 ***/

struct Seed {
	float x, y;
};

Seed getSeed(ContinuedFraction &theta, int k);

/** QUESTION 6 ***/

void pushSeed(std::list<Seed> &flower, ContinuedFraction &fr);

/** QUESTION 7 ***/

struct MagicBox {
    ContinuedFraction boxedFraction;
    int i = 0, j = 1, k = 1, l = 0;
    int indexInBoxedFraction = 0;
};

int spitNextMagicBox(MagicBox &box);
ContinuedFraction getCFUsingMB(ContinuedFraction &f, int a, int b, int l);



#endif /* FLOWERS_H_ */
