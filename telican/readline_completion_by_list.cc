#include "readline_completion_by_list.h"
#include <boost/regex.hpp>
#include <iostream>

using namespace std;
using namespace readlinepp;

completion_by_list::completion_by_list(const string &regex) :
	m_regex(regex)
{
}

completion_by_list::~completion_by_list()
{
}

void completion_by_list::add(const string &completion)
{
	m_completions.push_back(completion);
}

void completion_by_list::add(const vector<string> &completions)
{
	m_completions.insert(m_completions.begin(),
			completions.begin(),
			completions.end());
}


size_t completion_by_list::completions_count(const string &line)
{
	boost::regex re(m_regex);
	boost::cmatch matches;

	//cout << "regex = '" << m_regex << "', line = '" << line << "'" << endl;

	if (boost::regex_match(line.c_str(), matches, re))
	{
		//cout << "return size = " << m_completions.size() << endl;
		return m_completions.size();
	}
	else
	{
		return 0;
	}
}

string completion_by_list::get_completion(const string &line, 
		size_t index)
{
	assert(index < m_completions.size());

	//cout << "compl: " << m_completions[index] << endl;
	return m_completions[index];
}


