// Syllable_counter.cpp
/* 
This is a very crude implementation of a syllable-counting algorithm, pending
an implementation based on actual English morphology of words. 
*/

#include "Syllable_counter.h"

#include <string>
#include <cctype>
#include <cstring>
using namespace std;

int syllable_count(const string& word);
bool isvowel(char);
void add_check_from_front(int& counter, const string& word, const char * pattern);
void sub_check_from_front(int& counter, const string& word, const char * pattern);
void add_check_at_end(int& counter, const string& word, const char * pattern);
void sub_check_at_end(int& counter, const string& word, const char * pattern);
string get_word (const string& instring, int& start_pos);



/* count_total_syllables
Externally-available function.

Find each word in a string, and total the syllables found, +1 for each word
to represent the inter-word spacing.
above should be a different isolated-word style; 
for continuous speech, only the number of syllables without inter-word spacer syllable
this code was imported from GLEAN ...  - dk 04/11/2011

*/

int count_total_syllables(const string& input)
{
	
	int start_pos = 0;
	string word;
	int total = 0;
	
	while (start_pos < int(input.length())) {
		word = get_word(input, start_pos);
		if (word.length() > 0) {
			total += syllable_count(word);
//			total++; // add one syllable for each word found - dk 04/11/2011
			}
		}
	return total;
}
	
/* get_word
This function finds a word in an input string, starting from the supplied
start_pos, which gets updated.
string of length zero is returned if no word is found.
 
*/

string get_word (const string& instring, int& start_pos)
{
	
	const char * p_start = instring.c_str();	// instring is not modified here!
	const char * p = p_start + start_pos;
	
	// scan to where a word starts
	while (*p && (isspace(*p) || ispunct(*p)) )
		p++;
		

	string outstr;
	// collect characters in the string until end of string or space or punct
	while (*p && (!isspace(*p) && !ispunct(*p)) ) {
		outstr += *p;
		p++;
		}
	
	start_pos = int(p - p_start);
	return outstr;
}

/* syllable_count
This is a very crude implementation of a syllable-counting algorithm, pending
an implementation based on actual English morphology of words. 
An approximate number of syllables is counted for an English word, but 
digits are assumed to be spoken separately. If the word contains only uppercase
letters, as in an acronym, the letters are assumed to be spoken separately.
*/


int syllable_count(const string& word)
{

	// fix upper case acronym recognition
	string::size_type i;
	bool all_digits = true;
	bool in_vowel_cluster = false;
	bool all_upper = true;
	int all_upper_syllable_count = 0;
	int digit_syllable_count = 0;
	int syllable_count = 0;

	for (i = 0; i < word.length(); i++) {
		char c = word[i];
		// check for an acronym - either all upper case or digits
		if(isupper(c)) {
			if (c == 'W')
				all_upper_syllable_count += 3;
			else
				all_upper_syllable_count++;
			}
		else if (!isdigit(c))	// if not a digit, no acronym
			all_upper = false;
		
		// digits are counted at separate syllables
		if (isdigit(word[i])) {
			if (word[i] == '0' || word[i] == '7')
				digit_syllable_count += 2;
			else 
				digit_syllable_count++;
			}
		// count the number of vowel clusters
		else if(isvowel(word[i])) {
			if (!in_vowel_cluster) {
				in_vowel_cluster = true;
				syllable_count++;
				}
			all_digits = false;
			}
		else {
			in_vowel_cluster = false;
			all_digits = false;
			}
		}
	
	if(all_digits)
		return digit_syllable_count;
	if(all_upper)
		return all_upper_syllable_count + digit_syllable_count;
		
	// adjust for exceptions to simple vowel cluster rule
	// assume one exception of the type appears per word
	// note order sensitivities
	add_check_from_front(syllable_count, word, "ious");
	add_check_from_front(syllable_count, word, "iat");
	add_check_from_front(syllable_count, word, "iet");
	add_check_from_front(syllable_count, word, "ien");
	add_check_from_front(syllable_count, word, "ua");
	sub_check_from_front(syllable_count, word, "qua");
	
	// digit syllables could be included
	
	// if syllable count is somehow zero, return one
	if (syllable_count == 0)
		return 1 + digit_syllable_count;
	// if only one syllable, return without further checking
	if (syllable_count == 1)
		return syllable_count + digit_syllable_count;
	// check end cases only if current count > 1
	// deduct most common cases first
	sub_check_at_end(syllable_count, word, "e");
	sub_check_at_end(syllable_count, word, "ed");
	sub_check_at_end(syllable_count, word, "es");
	sub_check_at_end(syllable_count, word, "ely");
	// then add for special cases
	add_check_at_end(syllable_count, word, "ted");
	add_check_at_end(syllable_count, word, "ded");
	add_check_at_end(syllable_count, word, "ee");
	add_check_at_end(syllable_count, word, "ue");
	add_check_at_end(syllable_count, word, "ees");
	add_check_at_end(syllable_count, word, "ies");
	add_check_at_end(syllable_count, word, "ied");
	add_check_at_end(syllable_count, word, "ses");
	add_check_at_end(syllable_count, word, "ces");
	add_check_at_end(syllable_count, word, "ges");
	add_check_at_end(syllable_count, word, "les");
	add_check_at_end(syllable_count, word, "hes");
	add_check_at_end(syllable_count, word, "le");
	sub_check_at_end(syllable_count, word, "ile");

	return syllable_count + digit_syllable_count;
}

	
void add_check_from_front(int& counter, const string& word, const char * pattern)
{
	string::size_type pos = word.find(pattern);
	if (pos != string::npos)
		counter++;
}

void sub_check_from_front(int& counter, const string& word, const char * pattern)
{
	string::size_type pos = word.find(pattern);
	if (pos != string::npos)
		counter--;
}

void add_check_at_end(int& counter, const string& word, const char * pattern)
{
	string::size_type pos = word.rfind(pattern);
	// pattern appears at end if pos + strlen(pattern) = word.length
	if (pos != string::npos && (pos + strlen(pattern) == word.length()) )
		counter++;
}

void sub_check_at_end(int& counter, const string& word, const char * pattern)
{
	string::size_type pos = word.rfind(pattern);
	// pattern appears at end if pos + strlen(pattern) = word.length
	if (pos != string::npos && (pos + strlen(pattern) == word.length()) )
		counter--;
}

bool isvowel(char c)
{
	static char vowels[6] = {'a', 'e', 'i', 'o', 'u', 'y'};
	static int n_vowels = 6;
	c = char(tolower(c));
	
	for (int i = 0; i < n_vowels; i++) 
		if (c == vowels[i])
			return true;
	
	return false;
}

	

