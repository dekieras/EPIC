#ifndef CLAUSE_H
#define CLAUSE_H

#include "Symbol.h"
#include "Symbol_utilities.h"
#include <list>
#include <string>
#include <iostream>

class Output_tee;

// Clause_state encapsulates an iterator pointing to a term, thus saving the state

class Clause_state {
public:
	Clause_state(Symbol_list_t::const_iterator in_term_it) :
		term_iterator(in_term_it)
		{}

private:
	Symbol_list_t::const_iterator term_iterator;

friend class Clause;

};


// Clause encapsulates the list of terms and an iterator for the current term
// It is used to represent both production rule condition patterns and memory clauses
// Situation where current term or tag is the first of an empty clause is undefined
class Clause {
public:
	// construct an empty clause - can't put anything into it
	Clause() :
//		clause_str(string()),
		clause (Symbol_list_t())
		{current_term = clause.begin();}
	
	// construct a clause from a Tag, object name, property name, property value
	Clause(const Symbol& tag_name, const Symbol& object_name, const Symbol& property_name,
		const Symbol& property_value);

	// construct a Clause from a Symbol_list
	explicit Clause (const Symbol_list_t& in_term_list) :
//		clause_str(string()),
		clause(in_term_list)
		{current_term = clause.begin();}
	
	// construct a Clause from a string - contents must be valid, or empty clause results
	explicit Clause(const std::string& in_string) :
//		clause_str(in_string),
		clause(parse_clause_string(in_string))
		{current_term = clause.begin();}
		
	// Construct a Clause from another clause - reset current term to beginning of new list
	Clause(const Clause& in_clause) :
		clause(in_clause.clause)
		{current_term = clause.begin();}

	// the "tag" of a clause is the first term - assuming here that it is present	
	const Symbol& get_tag() const
		{return clause.front();}
	bool at_end() const;
	
	void advance() const;

//	string get_clause_str() const
//		{return clause_str;}
		
	const Symbol_list_t& get_clause_term_list() const
		{return clause;}

	// result is undefined if at end!
	const Symbol& get_current_term() const;
		
	Clause_state get_state() const
		{return Clause_state(current_term);}
		
	void set_state(const Clause_state& st) const
		{current_term = st.term_iterator;}
		
	// Assign a Clause from another clause - reset current term to beginning of new list
	Clause& operator= (const Clause& rhs)
		{
			clause = rhs.clause;
			current_term = clause.begin();
			return *this;
		}
	// order Clauses by comparing corresponding terms using operator<
	bool operator< (const Clause& rhs) const;
	// output the clause and the current term
	void display(std::ostream& os) const;
	// output just the clause
	friend std::ostream& operator<< (std::ostream& os, const Clause& clause);
	// output the clause and the current term
	void display(Output_tee& ot) const;
	// output just the clause
//	friend Output_tee& operator<< (Output_tee& os, const Clause& clause);

private:
//	const string clause_str;
	Symbol_list_t clause;
	mutable Symbol_list_t::const_iterator current_term;

	static Symbol_list_t parse_clause_string(const std::string& in_string);
	static std::string get_term(const std::string& in_str, std::size_t& start_pos);
};

typedef std::list<Clause> Clause_list_t;

std::ostream& operator<< (std::ostream& os, const Clause& in_clause);



#endif
