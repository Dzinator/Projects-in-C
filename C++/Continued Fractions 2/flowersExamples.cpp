/*
 * flowerExamples.cpp
 */
#include "flowers.h"

unsigned int spit(ContinuedFraction &f, unsigned int index) {
    auto n = f.fixedPart.size();
    auto m = f.periodicPart.size();
    if(index < n) {
        return f.fixedPart[index];
    }else if (m != 0) {
        return f.periodicPart[(index - n) % m];
    }
    return -1;
}

unsigned int spitGolden(unsigned int index) {
	return 1;
}

unsigned int spitSqrt(unsigned int index) {
	return index == 0 ? 1 : 2;
}

unsigned int spitEuler(unsigned int index) {
	if(index == 0) return 2;
	if(index % 3 != 2) return 1;
	return (index / 3 + 1) * 2;
}