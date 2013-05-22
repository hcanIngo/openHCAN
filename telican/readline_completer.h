#ifndef READLINE_COMPLETER_H
#define READLINE_COMPLETER_H

#include <vector>
#include <readline_completion_generator.h>

using namespace std;

namespace readlinepp
{
	class completer
	{
		private:
			static vector<completion_generator*> m_generators;
			static completion_generator *m_current_generator;
			static char** completion(const char * text, 
					int start,  int end);
			static char* completion_generator_func(
					const char* text, int state);
		public:
			completer();
			virtual ~completer();
			void add(completion_generator &gen);

	};
}

#endif
