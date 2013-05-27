#ifndef READLINE_COMPLETION_GENERATOR_H
#define READLINE_COMPLETION_GENERATOR_H

#include <string>

using namespace std;

/**
 * Diese Klasse ist eine Basisklasse, welche
 * in Zusammenarbeit mit dem readline Klasse Command Completion
 * ermoeglicht.
 * 
 * In der Praxis wird diese Klasse nie direkt, sondern immer spezialisier
 * verwendet. Der Normalfall ist die Verwendung von
 * readline_completion_by_list
 */

namespace readlinepp
{
	class completion_generator
	{
		public:
			virtual size_t completions_count(const string &line) = 0;
			virtual string get_completion(const string &line, size_t index) = 0;
			virtual ~completion_generator();
	};
}

#endif
