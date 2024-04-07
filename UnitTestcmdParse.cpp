#include "pch.h"
#include "CppUnitTest.h"
#include "cmdparse.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestcmdParse
{
	TEST_CLASS(UnitTestcmdParse)
	{
	public:
		

		#if false
		// int main(int argc, char *argv[]);
		TEST_METHOD(GivenNoParameters_ExpectZeroParamCount)
		{
			const char *argv [] = { "sample.exe", "\0"};
			WGT::cmdParse cmd;
			cmd.init(1, argv);
			
			auto vec = cmd.get_arguments();
			Assert::AreEqual(0, static_cast<int>(vec.size()));
		}


		TEST_METHOD(GivenMultipleParameters_ExpectMatchingParamCount)
		{
			const char* arg1[] = { "sample.exe" , "opt1", "\0" };
			WGT::cmdParse cmd1;
			Assert::IsTrue( cmd1.init( 2, arg1 ) );
			auto vec = cmd1.get_arguments();
			Assert::AreEqual(1, static_cast<int>(vec.size()));

			const char* arg2[] = { "sample.exe" , "opt1", "opt2", "\0" };
			WGT::cmdParse cmd2;
			Assert::IsTrue( cmd2.init( 3, arg2 ) );
			vec = cmd2.get_arguments();
			Assert::AreEqual(2, static_cast<int>(vec.size()));


			const char* arg3[] = { "sample.exe" , "opt1", "opt2", "opt3", "\0" };
			WGT::cmdParse cmd3;
			Assert::IsTrue( cmd3.init( 4, arg3 ) );
			vec = cmd3.get_arguments();
			Assert::AreEqual(3, static_cast<int>(vec.size()));

			const char* arg4[] = { "sample.exe" , "opt1", "opt2", "opt3", "opt4", "\0" };
			WGT::cmdParse cmd4;
			Assert::IsTrue( cmd4.init( 5, arg4 ) );
			vec = cmd4.get_arguments();
			Assert::AreEqual(4, static_cast<int>(vec.size()));
		}

		#endif


		TEST_METHOD(GivenArgumentCountZero_ExpectFailure)
		{
			const char* argv[] = { "", "\0" };
			WGT::cmdParse cmd;
			auto status = cmd.init(0, argv);
			Assert::IsFalse(status);
		}

		TEST_METHOD(GivenTwoArgs_ExpectOptionMatch)
		{
			const char* argv[] = { "Sample.exe", "--BufferSize:23", "--OutputFile=\"C://Temp//\"" };

			WGT::cmdParse cmd;

			// Add supported options
			cmd.add_param_option(WGT::cmdOption("BufferSize", "1000", "b"));
			cmd.add_param_option(WGT::cmdOption("OutputFile", "output.txt", "o"));

			// Initialize with arguments
			cmd.init(3, argv);

			Assert::IsTrue( cmd.get_param_option_count() == 2 );
			Assert::IsTrue( cmd.has_param_option("BufferSize"));
			Assert::IsTrue( cmd.has_param_option("buffersize"));
			Assert::IsTrue( cmd.has_param_option("bufferSIZE"));
			Assert::IsTrue( cmd.has_param_option("OutputFile"));
			Assert::IsTrue( cmd.has_param_option("outputfile"));
			Assert::IsFalse( cmd.has_param_option("NotAnOption"));
		}


		TEST_METHOD(GivenTwoArgs_ExpectValueMatch)
		{
			const char* argv[] = { "Sample.exe", "--BufferSize:23", "--OutputFile=\"C://Temp//\"" };

			WGT::cmdParse cmd;

			// Add supported options
			cmd.add_param_option(WGT::cmdOption("BufferSize", "1000", "b"));
			cmd.add_param_option(WGT::cmdOption("OutputFile", "output.txt", "o"));

			// Initialize with arguments
			cmd.init(3, argv);

			Assert::IsTrue(cmd.get_param_option_count() == 2);

			// Check for values
			Assert::AreEqual(23, cmd.get_param_option("BufferSize").get_value<int>());

			std::string val = cmd.get_param_option("OutputFile").get_value<std::string>();
			Assert::IsTrue(val == "C://Temp//");

			auto helpString = cmd.get_helpstring();
		}

		TEST_METHOD(GivenShortOptionArgs_ExpectOptionMatch)
		{
			const char* argv[] = { "Sample.exe", "-a:16", "-b 6.3" };

			WGT::cmdParse cmd;

			// Add supported options
			cmd.add_param_option(WGT::cmdOption("optionA", "3", "a"));
			cmd.add_param_option(WGT::cmdOption("optionB", "45.6", "b"));

			Assert::IsTrue(!cmd.has_errors());

			// Initialize with arguments
			Assert::IsTrue( cmd.init(3, argv) );

			Assert::IsTrue(!cmd.has_errors());

			Assert::IsTrue(cmd.get_param_option_count() == 2);
			Assert::IsTrue(cmd.has_param_option("optionA"));
			Assert::IsTrue(cmd.has_param_option("optionB"));
		}

		TEST_METHOD(Given5Options_ExpectOptionCountMatch)
		{
			WGT::cmdParse cmd;
			const char* argv[] = { "Sample.exe", "--option1:16", "--option2 6" };

			// Add supported options
			cmd.add_param_option(WGT::cmdOption("option1", "1"));
			cmd.add_param_option(WGT::cmdOption("option2", "2"));
			cmd.add_param_option(WGT::cmdOption("option3", "3"));
			cmd.add_param_option(WGT::cmdOption("option4", "4"));
			cmd.add_param_option(WGT::cmdOption("option5", "5"));

			// Initialize with arguments
			cmd.init(3, argv);
			Assert::IsTrue(cmd.get_param_option_count() == 5);
		}

		TEST_METHOD(GivenOptions_ExpectToGetSameOption)
		{
			WGT::cmdParse cmd;
			const char* argv[] = { "Sample.exe", "--option1:16", "--option2 6" };

			// Add supported options
			cmd.add_param_option(WGT::cmdOption("option1", "1"));
			cmd.add_param_option(WGT::cmdOption("option2", "2"));
			cmd.add_param_option(WGT::cmdOption("option3", "3"));
			cmd.add_param_option(WGT::cmdOption("option4", "4"));
			cmd.add_param_option(WGT::cmdOption("option5", "5"));

			Assert::IsTrue( !cmd.has_errors() );

			// Initialize with arguments
			cmd.init(3, argv);
			auto opt1 = cmd.get_param_option("option1");
			Assert::IsTrue( opt1.longName == "option1");
			Assert::IsTrue( opt1.get_value<int>() == 16);
			Assert::IsTrue( opt1.defaultValue == "1");

			auto opt5 = cmd.get_param_option("option5");
			Assert::IsTrue(opt5.longName == "option5");
			Assert::IsTrue(opt5.paramValue == "");
			Assert::IsTrue(opt5.defaultValue == "5");

			auto helpString = cmd.get_helpstring();
		}

	};
}
