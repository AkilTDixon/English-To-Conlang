#include <iostream>
#include <windows.h>
#include "Database.h"
#include "Processor.h"
using namespace std;


int main()
{
	SetConsoleOutputCP(CP_UTF8);

	string negation[5] = { "isn't", "not", "don't", "cannot", "won't" };
	bool negative = false;
	string input;
	vector<string> sentence;

	cout << "Enter a sentence: ";
	getline(cin, input);

	//'to have' verb is special
	auto i = input.find("have(v)");
	if (i != string::npos)
		input.insert(i+7, " with");



	for (auto word : negation) {
		auto i = input.find(word);
		if (string::npos != i) {
			negative = !negative;

			if (word == "cannot") 
				input.erase(i + 3, (word.length() - 3));	
			else if (word == "isn't")
				input.erase(i + 2, (word.length() - 2));
			else
				input.erase(i, word.length()+1);
		}
	}

	while (input.length() > 0)
	{
		auto i = input.find(" ");
		if (i != string::npos)
		{
			sentence.push_back(input.substr(0, i));
			input.erase(0,i+1);
		}
		else
		{
			sentence.push_back(input);
			input = "";
			break;
		}
	}
	
	Processor p(sentence, negative);
	p.wordOrder();
	p.directTranslate();
	p.print();
	

	return 0;
}



