//============================================================================
#include "flowers.h"
#include <iomanip>

using namespace std;


/** Other helper methods - implemeneted below */
ContinuedFraction getCF(unsigned int, unsigned int);
void printCF(ContinuedFraction &, unsigned int );
void printSeed(Seed);
void printFlower(list<Seed>);


int main() {
	// test Euler square
    cout << "Integers in the sequence of e^2: " << endl;
	for (unsigned int ada = 0; ada < 18; ++ada) {
        cout << spitEulerSquare(ada) << " ";
    }cout << endl;
    
    
    // test angles
    ContinuedFraction sqrt2;
    sqrt2.fixedPart = {1};
    sqrt2.periodicPart = {2};
    cout << "Square root of 2: " << endl;
    printCF(sqrt2, 10);
    
    cout << "Square root 2 approximated with first 5 ints in the cont.fra.: ";    auto apx = getApproximation(sqrt2, 5);
    cout << apx.denominator << " " << apx.numerator << endl;
    
    cout << "Angle after 3 cycles of sqrt{2} rotations: ";
	auto angleThreeTimesSqrt2 = getAngle(sqrt2, 3);
	cout << angleThreeTimesSqrt2 << endl;
    
    
	//test seeds
    cout << endl << "First ten seeds generated with sqrt{2} rotations: " << endl;
    for (unsigned int ada = 0; ada < 10; ++ada) {
		printSeed(getSeed(sqrt2, ada));
	}
    cout << endl << endl;

    //test flower
	list<Seed> flower;
	for (unsigned int ada = 0; ada < 10; ++ada) {
		pushSeed(flower, sqrt2);
	}
    cout << "sqrt{2}-flower of size 10: " << endl;
    printFlower(flower);

    
    // test Magic Box
    auto thrithyOverThirteen = getCF(30, 13);
    auto one25OverThirteen = getCFUsingMB(thrithyOverThirteen, 5, 2, 15);
    auto one25OverThirteen2 = getCF(125,13);
    cout << "125/13 as a continued fraction: ";
    printCF(one25OverThirteen, 20);
    cout << "125/13 using the magic box as 5 + 2 * 30/13: ";
    printCF(one25OverThirteen2, 20);
    
    
    auto sqrt8 = getCFUsingMB(sqrt2, 0, 2, 20);
    cout << "Square root of 8 using magic box as 0 + 2 * sqrt{2}: " << endl;
    printCF(sqrt8, 10);
    
    
	return 0;
}

/**Prints the first 'length' integers of a Continued Fraction Representation */
void printCF(ContinuedFraction &f, unsigned int length) {
	// check if 'length' is longer than the actual CF
    if((f.periodicPart).empty() && length > (f.fixedPart).size()) {
		length = f.fixedPart.size();
    }
    bool first = true; //true to separate the first item with a ;
    // do the fixed part first
	cout << "[";
	for(auto z : f.fixedPart) {
		cout << z;
		if(--length == 0) {
			cout << "]" << endl;
			return;
		}else {
			cout << ((first) ? "; " : ", ");
		}
		first = false;
	}
    // print the periodic part
    while(length > 0) {
        for(auto z : f.periodicPart) {
            cout << z;
            if(--length == 0) {
                cout << "]" << endl;
                break;
            }else {
                cout << ((first) ? "; " : ", ");
            }
            first = false;
        }
    }
	cout << flush;
}

/** Returns the Continued Fraction of b/a */
ContinuedFraction getCF(unsigned int b, unsigned int a)
{
	ContinuedFraction* result = new ContinuedFraction;
	for(int c; a != 0; c = b, b = a, a = c % a) {
		result->fixedPart.push_back(b / a);
	}
	return *result;
}
/** Prints one seed using precision set at 3*/
void printSeed(Seed s) {
    cout << setprecision(3) <<"(" << s.x << ", " << s.y << ")";
}

/** Prints a given flower */
void printFlower(list<Seed> f) {
	auto size = f.size();
	for(auto s : f) {
		printSeed(s);
		cout << ((--size == 0) ? "" : "; ");
    }
    cout <<endl;
}
