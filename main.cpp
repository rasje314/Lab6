#include <iostream>
#include <algorithm>
#include <functional>
#include <vector>
#include <string>
#include <iterator>
#include <fstream>
#include <cctype>
#include <map>
#include <iomanip>
#include <list>

using namespace std;

class CountElements 
{
private:
	vector<string> theVector;
	map<string, int> * theMap;

public:
	CountElements(vector<string> v, map<string, int> & m) :
	 theVector(v), theMap(&m) { }
	void operator() (string elem) const 
	{
		//insert into the map a unique string and a count of how many 
		//times the string appears. uses a map because duplicates not allowed
		int c = count(theVector.cbegin(), theVector.cend(), elem);
		std::map<string, int>::iterator it = (*theMap).begin();
		(*theMap).insert(it, pair<string, int>(elem, c));
	}
};

class CopySingle 
{
private:
	list<string> * theList;
	vector<string> theVector;

public:
	CopySingle(list<string> & l, vector<string> v) :
	 theList(&l), theVector(v) { }
	void operator() (string elem) const 
	{
		//push all the words to the list
		(*theList).push_back(elem);
	}
};

bool isInValid(string&);
void toLower(string&);
void removeJunk(string&);

int main(int argc, char* argv[])
{
	//step 0: read file from console and determine output choice
	if(argc == 1)
	{
		cout << "Error: No arguments given." << endl;
		cout << "Usage: a.out FILE [-a] [-f] [-o N]" << endl;
	}
	else if(argc == 2)
	{
		cout << "Error: Second argument missing or invalid" << endl;
		cout << "Usage: a.out FILE [-a] [-f] [-o N]" << endl;
	}
	else
	{
		string filename{};
		string ch{};
		int choice{};
		int o{};

		filename = argv[1];
		ch = argv[2];

		if(ch == "-a")
		{
			choice = 1;
		}
		else if(ch == "-f")
		{
			choice = 2;
		}
		else if(ch == "-o")
		{
			choice = 3;
			if(argc == 4)
			{
				string ch2 = argv[3];
				o = atoi(ch2.c_str());
			}
		}

		//step 1: read in words from file
		ifstream infile(filename);
		vector<string> colla;
		copy(istream_iterator<string>(infile),
			istream_iterator<string>(),
			back_inserter(colla));

		//step 2: lower case
		for_each(colla.begin(), colla.end(), toLower);

		//step 3: cut leading and trailing junk
		void(*fun_ptr)(string&) = &removeJunk;
		for_each(colla.begin(), colla.end(), (*fun_ptr));
	
		//step 4: remove invalid words
		auto newEnd = remove_if(colla.begin(), colla.end(), isInValid);
		vector<string> validWords(distance(colla.begin(), newEnd));
		copy(colla.begin(), newEnd, validWords.begin());

		//step 5: print in alphabetical order and frequency of the word
		// -a
		if (choice == 1)
		{
			map<string, int> aCollection;
			for_each(validWords.cbegin(), validWords.cend(), 
			 CountElements(validWords, aCollection));

			for_each(aCollection.cbegin(), aCollection.cend(),
			[](pair<string, int> elem)
			{
				cout << left << setw(6) << elem.first << right 
				<< setw(4) << elem.second <<  endl;
			});
		}
		//step 6: print in decreasing frequency secondly alphabetical order
		// -f
		else if (choice == 2)
		{
			map<string, int> aCollection;
			for_each(validWords.cbegin(), validWords.cend(), 
			 CountElements(validWords, aCollection));

			vector<pair<string, int>> bCollection;
			copy(aCollection.cbegin(), aCollection.cend(), 
			 back_inserter(bCollection));

			sort(bCollection.begin(), bCollection.end(),
			[](const pair<string, int> &elem1, const pair<string, int> &elem2)
			{
				return elem1.second > elem2.second;
			});

			for_each(bCollection.cbegin(), bCollection.cend(),
			[](pair<string, int> elem)
			{
				cout << right << setw(6) << elem.first << setw(4) 
				<< elem.second << endl;
			});
		}
		//step 7: print in same order as the file and only o many chars per line
		// -o 9
		else if (choice == 3)
		{
			list<string> cCollection;
			for_each(validWords.cbegin(), validWords.cend(),
				CopySingle(cCollection, validWords));

			string stw = "";
			for_each(cCollection.cbegin(), cCollection.cend(),
			[&stw, &o](string elem)
			{
			
				if (stw == "")
				{
					stw = elem + " ";
					cout << stw;
				}
				else if (stw.size() + elem.size() >= o)
				{
					cout << endl;
					stw = elem + " ";
					cout << stw;
				}
				else
				{
					stw.insert(stw.size(), elem + " ");
					cout << elem << " ";
				}
			});
			cout << endl;
		}
	}

	cout << endl;
	
    return 0;
}

bool isInValid(string& elem)
{
	//3 characters long
	if (elem.size() < 3)
	{
		return true;
	}

	//hyphens doesn't occur in the beginning
	auto it = find(elem.cbegin(), elem.cend(), '-');
	if (it == elem.begin())
	{
		return true;
	}

	//hyphens doesn't occur in the end
	if (elem.at(elem.size() - 1) == '-')
	{
		return true;
	}

	//hyphens are not consecutive --
	auto a = -1;
	a = elem.find("--");
	if (a != -1)
	{
		return true;
	}

	//all characters are alphabetical
	int b = 0;
	for_each(elem.cbegin(), elem.cend(),
	[&b](char el)
	{
		if (!isalpha(el))
		{
			if (el != '-')
			{
				b = 1;
			}
		}
	});
	if (b == 1)
	{
		return true;
	}

	return false;
}

void toLower(string& elem)
{
	//transoform all characters in a word to lower case
	transform(elem.begin(), elem.end(), elem.begin(), ::tolower);
}

void removeJunk(string& elem)
{
	//cut off leading junk
	//reverse
	//cut off trailing junk
	auto found = elem.find_first_not_of("!'\"(");
	elem = elem.substr(found, elem.size());
	reverse(elem.begin(), elem.end());
	found = elem.find_first_not_of("!?;,:.'\")");
	elem = elem.substr(found, elem.size());
	reverse(elem.begin(), elem.end());
	if (elem.size() > 2)
	{
		if (elem.at(elem.size() - 2) == '\'' && 
		    elem.at(elem.size() - 1) == 's')
		{
			elem.erase(elem.size() - 2);
		}
	}
}