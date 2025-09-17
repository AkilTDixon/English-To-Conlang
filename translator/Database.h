#pragma once
#include <jdbc/cppconn/driver.h>
#include <jdbc/cppconn/exception.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <vector>
#include <string>
using namespace std;
using namespace sql;

class Database
{
public:
	Database();
	~Database();

	Driver* d;
	Connection* c;
	Statement* s;
	ResultSet* r;
	PreparedStatement* ps;

	string getEnglish(string erindin);
	string getErindin(string english);
	
	string getType(string erindin);
	//vector<string> getAllVerbs();
	

	void populateDatabase();
	void adjustVerbs();
private:
	string specialSuffix[10] = { "(v)", "(s)", "(o)", "(f)", "(p)", "(pl)", "(adj)", "(sp)", "(pp)", "(q)"};
	bool cleanWord(string&);
};

string getType(string s);
string getConjugation(string verb);