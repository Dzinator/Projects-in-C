
#include "continued.h"

void printCF(ContinuedFraction, int);
ContinuedFraction *getCFBAD(unsigned int , ContinuedFraction *, ContinuedFraction *);
ContinuedFraction *getCF(int *array, int num_items);

using namespace std;

int main() {
	// create the golden ratio and print it
	ContinuedFraction goldenRatio;
	goldenRatio.head = 1;
	goldenRatio.tail = &goldenRatio;
	cout << "Golden ratio's continued fraction is: ";
	printCF(goldenRatio,10);

	//create another continued fraction from array of integers
	int someArray[] = { 16, 2, 77, 40, 12071 };
	ContinuedFraction *someCF = getCF(someArray,5);
	cout << "You just created ";
	printCF(*someCF,5);

	// get approximation
	Fraction testFraction = getApproximation(someCF,5);
	cout << "..and its approximation is " << testFraction.numerator << "/" << testFraction.denominator << endl;
	cout << "The continued fraction of the above approximation is ";
	printCF(*getCF(testFraction.numerator, testFraction.denominator),5);

	//test periodic continued fractions
	int tesHead = 1;
	int fixedArray[] = {3,2,1};
	int periodArray[] = {3,5,2};
	printCF(*getCF(tesHead,getCF(fixedArray,3), getCF(periodArray,3)),20);




	return 0;
}

/**
 * method prints a given continued fraction.
 * If the fraction has length larger than max_length,
 * the method only print max_lenth number of digits and ...
 * to signal that there are more to come
 */
void printCF(ContinuedFraction fr, int max_length) {
	//always print the integer part of the number
	std::cout << "[" << fr.head;
	// print the rest
	if(fr.tail != nullptr && max_length-- > 1) {
		//print second integer
		std::cout << "; ";
		fr = *(fr.tail);
		std::cout << fr.head;
		// print the rest separated by comma
		while(fr.tail != nullptr && (--max_length > 0)) {
			std::cout << ", ";
			fr = *(fr.tail);
			std::cout << fr.head;
		}
		// check to see whether anything was left out
		if(fr.tail != nullptr) std::cout << ",...";
	}
	std::cout << "]" << std::endl;
}


ContinuedFraction *getCFBAD(unsigned int head, ContinuedFraction *fixed, ContinuedFraction *period) {
	auto toRet = new ContinuedFraction;
	toRet->head = head;
	toRet->tail = fixed;
	auto tmp = toRet;
	// go to the end of fixed
	while(tmp->tail != nullptr) tmp = tmp->tail;
	// attach end of fixed to period
	tmp->tail = period;
	// go to the end of period
	while(tmp->tail != nullptr) tmp = tmp->tail;
	// attach the end of period to the beginning of period
	tmp->tail = period;
	return toRet;
}


/** OTHER USEFUL METHODS */

/**
 * Generates a continued fraction with elements coming
 * from an array of integers
 */
ContinuedFraction *getCF(int *array, int num_items) {
	auto toRet = new ContinuedFraction;
	toRet->head = array[0];
	ContinuedFraction *last = toRet;
	for (int ada = 1; ada < num_items; ++ada) {
		last->tail = new ContinuedFraction;
		last = last->tail;
		last->head = array[ada];
	}
	last->tail = nullptr;
	return toRet;
}
