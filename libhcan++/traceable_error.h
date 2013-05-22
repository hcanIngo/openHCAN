#ifndef TRACEABLE_ERROR_H
#define TRACEABLE_ERROR_H

#include <vector>
#include <string>

using namespace std;

class traceable_error : public std::exception
{
	protected:
		string m_message;
		vector<string> m_trace;
		friend std::ostream &operator << (std::ostream &o, 
				const traceable_error &e);
		friend traceable_error &operator << (traceable_error &e, 
				const string &s);
		friend traceable_error &operator << (traceable_error &e, 
				const int i);
	public:
		traceable_error(const string &s);
		virtual ~traceable_error() throw ();
		static vector<string> create_stack_trace ();
		virtual const char* what() const throw ();
		const vector<string> trace () const;
};

traceable_error &operator << (traceable_error &e, const string &s);
traceable_error &operator << (traceable_error &e, const int i);
std::ostream &operator << (std::ostream &o, const traceable_error &e);

#endif
