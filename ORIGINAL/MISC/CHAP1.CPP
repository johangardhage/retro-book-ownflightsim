#include <iostream.h>

void main()
{
	int intvar;
	int *pointvar=&intvar;
	*pointvar=3;

	cout << "\n\nThe value of INTVAR is " << intvar << endl;
	cout << "The value of *POINTVAR is " << *pointvar << endl;
	cout << "The address at which POINTVAR is pointing is " << pointvar << endl;
	cout << "The address represented by the variable is " << &intvar << endl;

	char c;
	cin.get(c);



}
