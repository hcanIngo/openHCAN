#ifndef READLINEPP_H
#define READLINEPP_H

#include <string>
#include <stdexcept>

using namespace std;

namespace readlinepp
{
	string read_line(const string &prompt);

	struct end_of_input : public std::exception
	{
	};

	class history
	{
		private:
			string m_histfile;
		public:
			history(const string &filename);
			virtual ~history();
			void add (const string &line);
	};
};


#endif
