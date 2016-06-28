#ifndef PPS_FILE_PARSER_H
#define PPS_FILE_PARSER_H

//#include "Production_rule.h"
#include "PPS_exception.h"
#include "Clause.h"
#include "PPS_rule_compiler_classes.h"
#include "Parameter.h"
//#include "Command_Action.h"
#include "Symbol.h"

#include <iostream>
#include <string>
#include <list>
#include <map>

namespace Parsimonious_Production_System {

class File_token_stream {
public:
		
	File_token_stream(std::ifstream& ifs) :
		input_file(ifs), current_pos(0),next_pos(0), line_number(0), parentheses_count(0)
		{load_token_info();}
		
	std::string find_token(bool throw_if_not_found = true);
	std::string find_term(bool throw_if_not_found = true);
	
	void advance()
		{current_pos = next_pos;}
		
	void reset();
	
	void find_end_of_balanced_parentheses();
	
	// enable test of token stream for goodness in terms of the file stream being read
	operator bool () const;
		
	std::string get_status() const;
	void display_status() const;

private:
	std::ifstream& input_file;
	std::string current_token;
	std::size_t current_pos;
	std::size_t next_pos;
	int line_number;
	std::string line;
	int parentheses_count;	// counts number of '(' - number of ')' found so far
	
	void load_token_info();
	std::map<std::string, std::string> token_info;	// loaded in constructor

//	bool isolate_token(std::string& token);
	bool isolate_token();

	File_token_stream();
};


class PPS_file_parser {
public:
	PPS_file_parser(std::ifstream& ifs);
		
	bool parse_file(Compiler_rule_data_list_t& production_list, Clause_list_t& initial_memory_contents, 
		std::map<Symbol, Geometry_Utilities::Point>& named_locations, Parameter_specification_list_t& parameters, bool output_compiler_messages);
	
	void flush_past_error()
		{input.find_end_of_balanced_parentheses();}
	
	void display_status() const
		{input.display_status();}
			
private:
	File_token_stream input;
	void parse_definitions(Clause_list_t& initial_memory_contents, 
		std::map<Symbol, Geometry_Utilities::Point>& named_locations, Parameter_specification_list_t& parameters, bool output_compiler_messages);
	void parse_initial_memory_contents(Clause_list_t& initial_memory_contents, bool output_compiler_messages);
	void parse_clause(Clause_list_t& clause_list);
	void parse_named_location(std::map<Symbol, Geometry_Utilities::Point>& named_locations, bool output_compiler_messages);
	void parse_parameter_list(Parameter_specification_list_t& parameters, bool output_compiler_messages);
	Parameter_specification parse_parameter_spec(bool output_compiler_messages);
//	Compiler_rule_data parse_production(bool& result);
	void parse_production(Compiler_rule_data_list_t& production_list, bool output_compiler_messages);
	void parse_condition_list (Compiler_rule_data& production);
	void parse_condition_pattern (Compiler_rule_data& production);
	Symbol_list_t parse_negated_condition_pattern();
	void parse_action_list (Compiler_rule_data& production);
	void parse_action (Compiler_rule_data& production);
	Symbol_list_t parse_command();	// combines parse_motor_command() and parse_log_command(), which had identical code.  ajh & teh - 3/08
	Symbol_list_t parse_action_pattern();
	bool is_predicate(std::string& token) const;
	void load_predicate_info();

	// private embedded class
	struct Arg_requirements {
		int num_required_args;
		int num_required_vars;
		Arg_requirements(int in_num_required_args = 0, int in_num_required_vars = 0) :
			num_required_args(in_num_required_args), num_required_vars(in_num_required_vars)
			{}
		};
	Arg_requirements get_predicate_requirements(const std::string& token) const;
	
	std::map<std::string, Arg_requirements> predicate_info;	// loaded in constructor
};

class PPS_parser_exception : public PPS_exception {
public:
	PPS_parser_exception(): PPS_exception("Unspecified PPS Parser exception") {}
	PPS_parser_exception(const std::string& msg) : PPS_exception(msg) {}
	PPS_parser_exception(File_token_stream& input, const std::string& msg);
};

} // end namespace

#endif
