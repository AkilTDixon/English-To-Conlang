#include "Database.h"
#include <fstream>
#include <string>
#include <vector>



Database::Database()
{
	d = get_driver_instance();
	c = d->connect("tcp://localhost:3306", "root", "");
	c->setSchema("dictionary");
}
Database::~Database()
{
	delete c;
	delete ps;
	delete r;
}




void Database::populateDatabase()
{
	string filename = "Assets/erindin.txt";
	string line;
	string type;
	vector<string> erindin, english;
	ifstream file(filename);

	if (!file.is_open())
		return;

	while (getline(file, line))
		erindin.push_back(line);

	file.close();
	filename = "Assets/english.txt";
	file.open(filename);
	if (!file.is_open())
		return;

	while (getline(file, line))
		english.push_back(line);

	try {


		ps = c->prepareStatement("INSERT INTO dict(ID, erindin, english, type) VALUES (?, ?, ?, ?)");
		for (int i = 0; i < erindin.size(); i++) {

			type = getType(english[i]);
			ps->setInt(1, i);
			ps->setString(2, erindin[i]);
			ps->setString(3, english[i]);
			ps->setString(4, type);
			ps->execute();

		}
		delete ps;
	}
	catch (sql::SQLException& e) {
		cerr << "ERROR: SQLException: " << e.what() << endl;
		cerr << "MySQL error code: " << e.getErrorCode() << endl;
		cerr << "SQLState: " << e.getSQLState() << endl;
	}

}
string Database::getEnglish(string erindin)
{
	s = c->createStatement();
	r = s->executeQuery("SELECT english FROM `dict` WHERE erindin = '" + erindin + "'");
	delete s;
	if (r->next())
		return r->getString("english");
	
}
string Database::getErindin(string english)
{
	//transform(english.begin(), english.end(), english.begin(), [](unsigned char c) {return tolower(c); });
	//bool plural = false;
	//bool adjective = false;
	bool singularPersonal = false;
	bool pluralPersonal = false;
	//bool comparator = false;
	//bool superlative = false;

	//if (string::npos != english.find("(pl)"))
	//	plural = true;
	//else if (string::npos != english.find("(adj)"))
	//	adjective = true;

	
	//if (string::npos != english.find("(cmp)"))
	//	comparator = true;
	//else if (string::npos != english.find("(spl)"))
	//	superlative = true;

	if (string::npos != english.find("(pp)"))
		pluralPersonal = true;
	else if (string::npos != english.find("(sp)"))
		singularPersonal = true;
	bool verb = cleanWord(english);
	

	string query;
	if (verb)
		query = "SELECT erindin FROM (SELECT * FROM `dict` WHERE type = 'verb') as Sub WHERE english REGEXP '.*\\\\b" + english + "\\\\b.*'";
	else if (pluralPersonal)
		query = "SELECT erindin FROM `dict` WHERE type = 'plural personal' AND english REGEXP '\\\\b" + english + "\\\\b'";
	else if (singularPersonal)
		query = "SELECT erindin FROM `dict` WHERE type = 'singular personal' AND english REGEXP '\\\\b" + english + "\\\\b'";
	else
		query = "SELECT erindin FROM `dict` WHERE english REGEXP '\\\\b" + english + "\\\\b'";
	s = c->createStatement();
	r = s->executeQuery(query);
	delete s;
	if (r->next())
	{	
		string ret = r->getString("erindin");
	/*	if (adjective)
			ret += "(adj)";
		else if (plural)
			ret += "(pl)";
		else if (comparator)
			ret += "(cmp)";
		else if (superlative)
			ret += "(spl)";*/
		
		return ret;
		
	}
	else
		return "";



}
void Database::adjustVerbs()
{
	s = c->createStatement();
	r = s->executeQuery("SELECT ID, TRIM(LEADING 'to ' FROM english) AS base_verb FROM `dict` WHERE type = 'verb' AND english LIKE 'to %'");
	vector<string> allVerbs;
	vector<int> allIDs;

	while (r->next()) {
		allVerbs.push_back(r->getString("base_verb"));
		allIDs.push_back(r->getInt("ID"));
	}
	ps = c->prepareStatement("UPDATE `dict` SET english = ? WHERE ID = ?");
	for (int i = 0; i < allVerbs.size(); i++)
	{

		string entry = getConjugation(allVerbs[i]);
		ps->setString(1, entry);
		ps->setInt(2, allIDs[i]);
		auto rows = ps->executeUpdate();
	}
	delete ps;
	delete s;

}
bool Database::cleanWord(string& word)
{
	bool ret = false;
	auto j = word.find("(");
	while (j != string::npos)
	{		
		auto k = word.find(")");
		if (word.substr(j, (k - j)+1) == "(v)")
			ret = true;
		word.erase(j, (k - j) + 1);
		j = word.find("(");

	}
	return ret;
}
string Database::getType(string erindin)
{
	s = c->createStatement();
	r = s->executeQuery("SELECT type FROM `dict` WHERE erindin = '" + erindin + "'");

	if (r->next())
		return r->getString("type");

	delete s;
}
string getConjugation(string verb)
{
	string ed = verb + "ed";
	string ing = verb + "ing";
	string s = verb + "s";

	return verb + ", " + ed + ", " + ing + ", " + s;
}



string getType(string s)
{
	string ret = "noun";

	if (s.find("(adjective)") != string::npos)
		ret = "adjective";
	else if (s.find("to ") != string::npos)
		ret = "verb";


	return ret;
}