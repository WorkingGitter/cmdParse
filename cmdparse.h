/*
*	Author : Richard Chin
*	Date   : 2024-April
*	VERSION: 1.0
* 
*	Basic command-line parser for C++ applications.
* 
*	TODO: 
*		- Make compatible with wchar_t configuration
*/

#pragma once
#include "string_utils.h"
#include <cassert>
#include <exception>
#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <algorithm>

namespace WGT
{

	/*!	@brief Encapsulates a command-line option
	* 
	*	Basic structure that holds the potential parameter option 
	*   for a command-line application.
	*/
	struct cmdOption
	{
		cmdOption() = default;

		// Construct a command-line option:
		//
		// e.g.
		// 'auto cmd = cmdOption("BufferSize", "1000", 'b');'

		/*!	@brief Constructor for a command-line option
		* 
		*   Example: 
		*   ```cpp
		*	auto cmd = cmdOption("BufferSize", "1000", 'b');
		*	```
		*	The client can activate by passing the following:
		* 
		*	```
		*   > myapp.exe --BufferSize=1000
		*   ```
		*/
		cmdOption(std::string optionName, std::string defaultValue = "", std::string optionNameShort = "")
			:longName{ optionName }, shortName{ optionNameShort }, defaultValue{ defaultValue }
			{
				if( WGT::string_utils::is_blank(shortName)) {
					shortName = longName;
				}
			}

		// Ensure that our comparison is case-insensitive
		bool operator<(const cmdOption & obj) const {

			auto lowercaseA = WGT::string_utils::make_lower(this->longName);
			auto lowercaseB = WGT::string_utils::make_lower(obj.longName);

			return lowercaseA < lowercaseB;
		}

		template <typename T>
		T get_value() {

			std::istringstream ss(paramValue);
			T retVal;
			ss >> retVal;
			return retVal;
		}

		// Specializations:
		template<>
		int get_value<int>()
		{
			return std::stoi(paramValue.c_str());
		}

		template<>
		double get_value<double>()
		{
			return std::stod(paramValue.c_str());
		}

		bool has_value() const {
			return WGT::string_utils::is_blank(paramValue);
		}
		
		std::string longName{ "" };
		std::string shortName{ "" };
		std::string defaultValue{ "" };
		std::string paramValue{""};
	};


	/*!	@brief Command-line options handler class
	* 
	*/
	class cmdParse
	{
	public:
		cmdParse() = default;
		
		/*!	@brief Alternative constructor that takes preprepared options
		* 
		*   Example:
		*	```cpp
		*	cmdParse cmd( { {"optionA", "10", "a"}, {"backColour", "#FFFFFF", "b"} } );
		*	cmd.init(argc, argv);
		*	```
		*/
		cmdParse(std::vector<cmdOption> optionVec) noexcept {
			for (auto& o : optionVec) {
				add_param_option(o);
			}
		}

		/*!	@brief Initialize the command-line handler with the arguments given to the application
		* 
		*	Before calling, please add all the options that are to be supported by the application.
		* 
		*   Example:
		*   ```cpp
		*   int main(int argc, char *argv[])
		*   {
		*		cmdParse cmd;
		* 
		*		// Add supported options
		*       cmd.add_param_option(cmdOption("BufferSize", "1000", 'b'));
		*       cmd.add_param_option(cmdOption("OutputFile", "output.txt", 'o'));
		* 
		*		// Initialize with arguments
		*       cmd.init(argc, argv);
		*   }
		*   ```
		* 
		*/
		bool init(int argc, const char* argv[]) { 
			if((argc <= 0) ) {
				logError("No arguments given to application");
				return false;
			}

			m_executable_name = argv[0];

			for(int n = 1; n < argc; n++) {
				
				std::string arg_str = argv[n];
				WGT::string_utils::trim(arg_str);
				m_arguments.push_back(arg_str);
			}

			return parseOptions();
		}

		/*!	@brief Returns the list of arguments that was supplied to the application
		* 
		*   This does not include the name of the client executable.
		*/
		std::vector<std::string> get_arguments() noexcept {
			return m_arguments;
		}

		/*!	@brief Add a command-line option that the application will support
		* 
		*	@return false, if unable to set option. This could be due to 
		*   conflict in the option name provided. Check any reported errors if false.
		* 
		*	@sa get_errors
		*/
		bool add_param_option(cmdOption paramOption) {
			
			auto result = m_parameter_options.insert(paramOption);
			if(!result.second) {
				logError("Option already exists: " + paramOption.longName);
			}

			return result.second;
		}

		/*!	@brief Returns the number of command-line options that have been added
		* 
		*	Use @c add_param_option() to add more options to the handler.
		* 
		*	@sa add_param_option
		*	@sa has_param_option
		*/
		int get_param_option_count() const noexcept {
			return static_cast<int>(m_parameter_options.size());
		}

		/*!	@brief Test if a command-line option has been set
		*/
		bool has_param_option(std::string optionName) const {
			
			auto itF = m_parameter_options.find(cmdOption(optionName));
			if(itF == m_parameter_options.end()) {
				return false;
			}

			return true;
		}

		/*!	@brief Returns the option that matches the given name
		* 
		*	@param optionStr The **full** option name
		*/
		cmdOption get_param_option(std::string optionStr) {
			auto itF = m_parameter_options.find(cmdOption(optionStr));
			if (itF == m_parameter_options.end()) {
				return {};
			}

			return *itF;
		}

		/*!	@brief returns a short overview of the options for the application
		* 
		*   Example, for the following options:
		*   ```
		*       cmd.add_param_option(cmdOption("BufferSize", "1000", 'b'));
		*       cmd.add_param_option(cmdOption("OutputFile", "output.txt", 'o'));
		*   ```
		* 
		*   The help string shoudl be:
		*   ```
		*   MyApplication.exe [options]
		*   where options are:
		*			 -b, --BufferSize
		*			 -o, --OutputFile
		*   ```
		*/
		std::string get_helpstring() const {
			std::string helpString = m_executable_name + " [options]\n where options are:\n";

			for(auto& o : m_parameter_options) {
				helpString += "    -" + o.shortName + ", --" + o.longName + "\n";
			}

			helpString += "\n\n(version 1.0)";
			return helpString;
		}

		bool has_errors() const {
			return !m_errors.empty();
		}

		/*!	@brief Clears the list of errors that have accumulated
		*/
		void clear_errors() {
			m_errors.clear();
		}

		/*!	@brief returns the list of errors that have accumulated
		* 
		*   Please clear the errors after reading them.
		*/
		std::vector<std::string> get_errors() const {
			return m_errors;
		}

	private:

		// name of the calling executable
		std::string m_executable_name;

		// arguments : raw array given by user
		// options   : formatted array-values supplied to app.
		std::vector<std::string> m_arguments;
		std::set<cmdOption> m_parameter_options;
		std::vector<std::string> m_errors;

		void logError(std::string const & error) {
			m_errors.push_back(error);
		};

		/*!	@brief Returns the full option name of the given short name
		* 
		*	@return empty string if not found
		*/
		std::string getFullOptionName(std::string shortName) {
			for (auto& option : m_parameter_options) {
				if(option.shortName == shortName) {
					return option.longName;
				}
			}

			return "";
		}


		bool parseOptions() {

			auto isOptionPrefix = [](const std::string& s) {
								if (s[0] == '-') return true;
								return false;
								};

			auto cursor_iterator = m_arguments.begin();
			while( cursor_iterator != m_arguments.end() )
			{
				// Find sections between arguments with '--' or '-'
				//
				// e.g. {{"--firstOption"}, {"=1234"} , {"-s"}, {"--secondOp"}}
				//       ^------------------------------^
				//                section
				//
				auto start = std::find_if(cursor_iterator, m_arguments.end(), isOptionPrefix);
				auto end   = std::find_if(start+1, m_arguments.end(), isOptionPrefix);
				if(start == m_arguments.end())
					break;

				// Combine into single param string 
				// e.g.: "--firstOption=1234"
				std::string fullOptionString;
				for (start; start != end; start++)
				{
					fullOptionString += *start;
				}

				// set cursor to the next section
				cursor_iterator = end;

				// Tokenize
				assert((fullOptionString[0] == '-')); // TODO raise error if fails
				bool useFullOptionName = (fullOptionString[0] == '-') && (fullOptionString[1] == '-');
				WGT::string_utils::ltrim(fullOptionString, '-');
				auto value_iterator = std::find_if(fullOptionString.begin(), fullOptionString.end(), [](const char c){ 
																			return (c == ' ') || (c == ':') || (c == '='); }
																			);

				std::string name;
				std::string value;

				// get option name
				for(auto it = fullOptionString.begin(); it != value_iterator; it++)
					name += *it;

				// get option value
				for (auto it = value_iterator + 1; it != fullOptionString.end(); it++)
					value += *it;

				WGT::string_utils::trim(name);
				WGT::string_utils::trim(value);
				WGT::string_utils::trim(value, '\"');

				// If we have been given the short name, convert it to the full name
				if (!useFullOptionName)
				{
					name = getFullOptionName(name);
					if(name.empty()) {
						logError("Option not found: " + name);
						return false;
					}
				}

				// Update the option with our new value
				auto itF = m_parameter_options.find(cmdOption(name));
				if(itF == m_parameter_options.end()) {
					logError("Option not found: " + name);
					return false;
				}

				auto updatedOption = *itF;
				updatedOption.paramValue = value;
				m_parameter_options.erase(itF);
				m_parameter_options.insert(updatedOption);
			}


			return true;
		}
	};
}