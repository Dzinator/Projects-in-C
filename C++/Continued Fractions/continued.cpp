#include "continued.h"

/*
	Assignment 1 - COMP 322
		Yanis Hattab
		260535922
*/

ContinuedFraction *copy(ContinuedFraction* cf);
Fraction operator+(Fraction a, Fraction b);
ContinuedFraction *addToCF(ContinuedFraction* a, ContinuedFraction* b);

unsigned int gcd(unsigned int a, unsigned int b){
	int t = 0;
	while(b != 0){
		t = a % b;
		a = b;
		b = t;
	}

	return a;
}

unsigned int gcdFaster(unsigned int a, unsigned int b){
	while(a != b){
		 if(a > b) a = a - b;
		 else b = b - a;

	}

	return a;
}

ContinuedFraction *getCFlargerThanOne(unsigned int b, unsigned int a) {
	ContinuedFraction* first = new ContinuedFraction;
    ContinuedFraction* iterator = first;

    unsigned int r = b % a;  //remainder
    unsigned int q = b / a;	 //quotient
    unsigned int oldR = r;

    iterator->head = q;
    iterator->tail = NULL;

    while(r != 0) // we can still add another one
    {
    	iterator->tail = new ContinuedFraction;
    	iterator = iterator->tail;

    	q = a / r;
    	r = a % r;

    	iterator->head = q;
    	a = oldR;
    	oldR = r;
    }
    return first; //return link to first element
}

ContinuedFraction *getCF(unsigned int b, unsigned int a) {
  if (b <  a){
  	ContinuedFraction* answer = getCFlargerThanOne(a,b);
  	ContinuedFraction* first = new ContinuedFraction;

  	first->head = 0;
  	first->tail = answer;

  	return first;

  }
  else{
  	return getCFlargerThanOne(b,a); //satisfies Question 3
  }
}


ContinuedFraction *getCF(unsigned int head, ContinuedFraction *fixed, ContinuedFraction *period) {

  	ContinuedFraction* first = new ContinuedFraction;

  	first->head = head;
  	
  	ContinuedFraction* fix = copy(fixed);
  	ContinuedFraction* per = copy(period);
  	ContinuedFraction* iterator = per;

  	while(iterator->tail != NULL){ //until the end
  		iterator = iterator->tail;
  	}

  	iterator->tail = per; //loop
  	first = addToCF(first, addToCF(fix, per)); //united list

  	return first;

}


Fraction getApproximation(ContinuedFraction *fr, unsigned int n) {
	ContinuedFraction* iterator = fr;
	Fraction answer;

  	int first[n]; // holds first n values
  	for(int i = 0; i < n; i++){
  		first[i] = iterator->head;
  		iterator = iterator->tail;
  	}

  	if(n==1){
  		answer.numerator = first[n];
  		answer.denominator = 1;
  		return answer;
  	}

  	Fraction smallest = {1, first[n-1]};
  	Fraction secondSmallest = {first[n -2], 1};
  	Fraction sum = smallest + secondSmallest;
  	answer = {sum.denominator, sum.numerator}; // inverted for next addition

  	for(int i = (n - 3); i >= 0; i--){//keep adding and inverting along the array
  		Fraction t = {first[i], 1};
  		Fraction s = t + answer;
  		answer = {s.denominator, s.numerator};
  	}
  	//final invert and answer return
  	answer = {answer.denominator, answer.numerator};
  	return answer;
}


//**-* Helper Functions *-**
ContinuedFraction *addToCF(ContinuedFraction* a, ContinuedFraction* b){
	//helper function to add a CF to the end of another one
	ContinuedFraction* iterator = a;

	while(iterator->tail != NULL){
		iterator = iterator->tail;
	}

	iterator->tail = b; 
	return a;
}

ContinuedFraction *copy(ContinuedFraction* cf){
	//helper function to copy a CF
	ContinuedFraction* returnedCF = new ContinuedFraction;

	ContinuedFraction* tempIterator = returnedCF;

	ContinuedFraction* iterator = cf;

	tempIterator->head = iterator->head;

	while(iterator->tail != NULL){
		//go through all the linked nodes
		iterator = iterator->tail;

		tempIterator->tail = new ContinuedFraction;
		tempIterator = tempIterator->tail;
		tempIterator->head = iterator->head;
	}

	return returnedCF;
}

Fraction operator+(Fraction a, Fraction b)
{
	//redifining operator to do fraction addition (without reducing)
	int n = (a.numerator * b.denominator) + (b.numerator * a.denominator);
	int d = (b.denominator * a.denominator);
	Fraction added = {n, d};
	return added;
}


//---*- The End ---*-