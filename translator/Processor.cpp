#include "Processor.h"
#include <vector>
#include <unordered_set>
#include <stack>
using namespace std;


Database Processor::db;

Processor::Processor()
{
	future = false;
	past = false;
}


Processor::Processor(vector<string> s, bool n)
{
	negated = n;
	future = false;
	past = false;
	wordBuffer = s;

	wordOrder();
	
	
}
void Processor::initialize(vector<string> s, bool n)
{
	if (!wordBuffer.empty())
		wordBuffer.clear();
	if (!translatedBuffer.empty())
		translatedBuffer.clear();

	future = false;
	past = false;
	negated = n;
	for (auto word : s)
		wordBuffer.push_back(word);

	wordOrder();
	
}
Processor::~Processor()
{

}
void Processor::wordOrder()
{
	bool verb = false;
	bool subject = false;
	int start = 0;
	future = wordBuffer[wordBuffer.size() - 1].find("(f)") != string::npos;
	if (!future)
		past = wordBuffer[wordBuffer.size() - 1].find("(p)") != string::npos;

	

	//find all 'the'
	for (int i = 0; i < wordBuffer.size(); i++)
	{
		if (wordBuffer[i] == "the" || wordBuffer[i] == "The" || wordBuffer[i] == "our" || wordBuffer[i] == "Our") {
			if (string::npos != wordBuffer[i + 1].find("(spl)"))
				wordBuffer.erase(wordBuffer.begin()+i);
			else
				indicesOfThe.insert(wordBuffer[i+1]);
			
		}
	}

	//find the first verb
	for (int i = start; i < wordBuffer.size(); i++)
	{
		if (wordBuffer[i].find("(v)") != string::npos)
		{
			rotate(wordBuffer.begin(), wordBuffer.begin() + i, wordBuffer.begin() + i + 1);
			verb = true;
			break;
		}
	}
	//find the subject
	if (verb)
		start++;
	for (int i = start; i < wordBuffer.size(); i++)
	{
		if (wordBuffer[i].find("(s)") != string::npos)
		{
			auto middle = wordBuffer.begin() + i;
			if (indicesOfThe.contains(wordBuffer[i])) {
				middle = wordBuffer.begin() + i - 1;
				indicesOfThe.erase(wordBuffer[i]);
			}
			auto b = wordBuffer.begin() + 1;
			if (!verb)
				b = wordBuffer.begin();

			rotate(b, middle, middle + 1);
			subject = true;
			break;
		}
	}
	vector<int> objectIndices;
	if (subject)
		start++;
	//find the object
	for (int i = start; i < wordBuffer.size(); i++)
	{
		if (wordBuffer[i].find("(o)") != string::npos)
		{
			objectIndices.push_back(i);
			//break;
			
		}
	}
	stack<int> adjStack;
	//place all adjectives after object
	for (int i = 0; i < objectIndices.size(); i++)
	{
		for (int j = start; j < objectIndices[i]; j++)
		{
			if (wordBuffer[j].find("(adj)") != string::npos)
			{
				adjStack.push(j);
				if (wordBuffer[j + 1] == "and")
					adjStack.push(j + 1);
			}
		}
		start = objectIndices[i] + 1;
		while (!adjStack.empty())
		{
			swap(wordBuffer[adjStack.top()], wordBuffer[objectIndices[i]]);
			objectIndices[i] = adjStack.top();
			adjStack.pop();
		}
	}


	directTranslate();

}

void Processor::directTranslate()
{
	string subject = "";
	string compound[2] = { "", "" };
	bool imperative = false;
	bool gotAdjective = false;
	bool qualityN[2] = { false, false };
	for (auto word : wordBuffer) {

		if (word == "")
			continue;
		string toPush;
		//handle compounds in here
		if (string::npos != word.find("-"))
		{
			auto i = word.find("-");
			string w = word.substr(0, i);
			if (string::npos != w.find("(q)"))
				qualityN[0] = true;

			//negation
			if (w == "un")
				compound[0] = Processor::db.getErindin("no");
			else
				compound[0] = Processor::db.getErindin(w);

			word.erase(0, i+1);
			if (string::npos != word.find("(q)"))
				qualityN[1] = true;
			compound[1] = Processor::db.getErindin(word);


			
			if (w != "un")
			{
				//with the two compound words, determine their types
				string type0 = Processor::db.getType(compound[0]);
				string type1 = Processor::db.getType(compound[1]);


				if (type0 == "noun" && type1 == "verb")
				{
					if (!qualityN[1])
						swap(compound[0], compound[1]);
				}
				else if (type0 == "noun" && type1 == "noun")
				{
					if (qualityN[0])
						swap(compound[0], compound[1]);

				}
			}

			int index = compound[1].length() - 1;
			//if (string::npos != compound[1].find("(adj)"))
			//	index = compound[1].find("(adj)")-1;


			switch (compound[1][index])
			{
			case 'r':
				compound[1][index] = 'n';
				break;
			case 'i':
				compound[1].insert(index+1, "n");
				break;
			case 'a':
				//compound[1][index] = 'i';
				compound[1].insert(index+1, "n");
				break;

			}
			string ending = compound[0].substr(compound[0].length() - 2, 2);
			if (ending == "in" || ending == "an")
				compound[0].erase(compound[0].length() - 2, 2);
			else if (compound[0][compound[0].length() - 1] == 'i')
				compound[0].erase(compound[0].length() - 1, 1);


			toPush = compound[0] + compound[1];
		}
		else
			toPush = Processor::db.getErindin(word);
		
		if (string::npos != word.find("(")) {

			if (string::npos != word.find("(pl)"))
				translatedBuffer.push_back("mar");
			else if (string::npos != word.find("(cmp)")) 
				translatedBuffer.push_back(Processor::db.getErindin("more"));

			if (!gotAdjective && string::npos != word.find("(adj)")) {
				translatedBuffer.push_back("av");
				gotAdjective = true;
			}
			if (string::npos != word.find("(o)"))
				gotAdjective = false;

			if (string::npos != word.find("(spl)")) 
				translatedBuffer.push_back("te mesdi");			
			
			if (string::npos != word.find("(imp)"))
				imperative = true;

			if (string::npos != word.find("(s)"))
				subject = toPush;

		}

		
		translatedBuffer.push_back(toPush);
		if (imperative){
			translatedBuffer.push_back("nun");
			imperative = false;
		}
		
	}


	if (future || past)
	{
		int count = 0;
		while (translatedBuffer[count+1] != subject) {
			swap(translatedBuffer[count], translatedBuffer[count+1]);
			count++;
		}
		swap(translatedBuffer[count], translatedBuffer[count + 1]);
		count++;

		if (future) {
			translatedBuffer.push_back(Processor::db.getErindin("will"));
			rotate(translatedBuffer.begin(), translatedBuffer.end()-1, translatedBuffer.end());
		}
		else if (past) {
			translatedBuffer.push_back(Processor::db.getErindin("be"));
			rotate(translatedBuffer.begin(), translatedBuffer.end() - 1, translatedBuffer.end());
			translatedBuffer.insert(translatedBuffer.begin()+count+1, "efdi");

			
			
		}
	}

	//clean up any and all empty strings
	stack<int> emptyStrings;
	for (int i = 0; i < translatedBuffer.size(); i++)
	{
		if (translatedBuffer[i] == "")
			emptyStrings.push(i);
	}
	while (!emptyStrings.empty())
	{
		translatedBuffer.erase(translatedBuffer.begin() + emptyStrings.top());
		emptyStrings.pop();
	}
	
	applyRules();

}

void Processor::applyRules()
{
	if (translatedBuffer.empty())
		return;

	//if 'av' is before the definite article 'te', or before a possessive, remove 'av'



	//'mar' will erase prior 'te'
	int count = 0;
	stack<int> s;
	for (auto word : translatedBuffer)
	{
		if (translatedBuffer[count] == "te")
		{
			if (translatedBuffer[count + 1] == "mar")
				s.push(count);
		}
		else if (translatedBuffer[count] == "av")
		{
			string type;
			if (count + 1 < translatedBuffer.size())
			{
				type = Processor::db.getType(translatedBuffer[count + 1]);
				if (type == "")
					continue;
				if (string::npos != type.find("possessive") || type == "definite article")
					s.push(count);
			}
				
		}
		count++;
	}
	while (!s.empty()) {
		translatedBuffer.erase(translatedBuffer.begin() + s.top());
		s.pop();
	}
	count = 0;
	//move 'mar' ahead of possessives
	for (auto word : translatedBuffer)
	{
		if (translatedBuffer[count] == "mar")
		{
			if (count - 1 < 0)
				continue;
			if (translatedBuffer[count - 1] == "")
				continue;
			string type = Processor::db.getType(translatedBuffer[count - 1]);
			
			if (string::npos != type.find("possessive"))
				swap(translatedBuffer[count], translatedBuffer[count - 1]);
		}
		count++;
	}

	if (negated) {
		if (translatedBuffer[0] == "era")
			translatedBuffer[0] = Processor::db.getErindin("negation");
		else {
			translatedBuffer.push_back(Processor::db.getErindin("no"));
			rotate(translatedBuffer.begin(), translatedBuffer.end() - 1, translatedBuffer.end());
		}
	}


	translatedBuffer[0][0] = toupper(translatedBuffer[0][0]);
}

void Processor::print()
{

	for (auto word : translatedBuffer)
		cout << word << " ";

	cout << endl << endl;
}