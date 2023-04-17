#pragma once

#include <string>
#include <iostream>

#include "Item.h"

class Drink : public Item {

public:

	std::string name;
	char temp;	// h = hot, e = extra hot, i = iced
	int size; // 0 = small, 1 = medium, 2 = large
	std::string milkType;

	// drink constructor
	Drink(/*std::string n, */char t, int s, std::string m) {
		//name = n;
		temp = t;
		size = s;
		milkType = m;

		std::cout << name;
	}

};