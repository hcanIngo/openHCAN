#ifndef READLINE_COMPLETION_BY_LIST_H
#define READLINE_COMPLETION_BY_LIST_H

#include <readline_completion_generator.h>
#include <string>
#include <vector>

using namespace std;

namespace readlinepp
{
	class completion_by_list : public completion_generator
	{
		private:
			string m_regex;
			vector<string> m_completions;
		protected:
			virtual size_t completions_count(const string &line);
			virtual string get_completion(const string &line, size_t index);
		public:
			completion_by_list(const string &regex);
			virtual ~completion_by_list();
			void add(const string &completion);
			void add(const vector<string> &completions);
	};
}

#endif
