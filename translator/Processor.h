#pragma once
#include "Database.h"
#include <string>
#include <vector>
#include <unordered_set>

using namespace std;


class Database;

class Processor
{
public:

	Processor();
	Processor(vector<string>, bool);
	~Processor();

	bool past;
	bool future;
	bool negated;

	static Database db;
	vector<string> wordBuffer;
	vector<string> translatedBuffer;


	void initialize(vector<string>, bool);
	void directTranslate();
	void wordOrder();
	void applyRules();
	void print();

private:
	unordered_set<string> indicesOfThe;
	unordered_set<int> indicesToRemove;
	
};

