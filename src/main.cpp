#include "../headers/string_functions.h"
#include "../headers/A1Program.h"
#include "../headers/ErrorHandler.h"

#include <iosstream>

using std::cout;
using std::vector;
using std::string;
using std::endl;

vector <string> cmdArgs;

struct Flag
{
      string path;
      vector<string> file;
      bool debug = false;
      bool version = false;
      bool interpreted = true;
      string cppOutFile = "";
      string binOutFile = "";
      bool compiled = false;
      bool error = false;
};

Flag getFlags(int argc, char ** argv);

int main(int argc, char ** argv)
{
    Flag flags = getFlags(argc, argv);

    if (flags.flagError)
    {
      cout << "Error!" << endl;
      cout << "Type 'A1 -h' to get a list of instructions"  <<endl;
      return 0;
    }

    if (flags.help)
    {
      cout << "A1 Version" << VERSION_X << "." << VERSION_Y << endl;
      cout << "How To: A1` [options] [file name] [options]" << endl;
      cout << "Help Options:" << endl;
      cout << "'-v' or  '-version' displays the current version of A1 on your computer." << endl;
      cout << "'d' or 'debug' displays various debug constants before compilation." << endl;
      cout << "'-r' or 'run' runs the program in an interpreted fashion." << endl;
      cout << "Currently, no transpiling commands are present and run is active by default. " << endl;
  		cout << "Thus, anything after -r is ignored" << endl;
  		cout << "'-cpp [file' transpile to C++ and save the output in the given file" << endl;
  		cout << "'-bin [file]'transpile, compile with GCC and save the binary" << endl;
  		cout << "'-e' or '-execute' transpiles, compile and execute the binary" << endl;
  		cout << "any combination of -cpp, -bin and -e can be used" << endl;
  		cout << "like -r, anything after -e is ignored" << endl;
  		cout << "'-h' or '-help' displays these set of instructions." << endl;
  		cout << endl;
  		cout << endl;

  		return 0;
    }

    if (flags.version)
    {
        cout << "A1 version " << VERSION_X << "." << VERSION_Y << endl;
        return 0;
    }

    A1Program program;

    if (flags.inFiles.empty())
    {
      cout << "No source file specified" << endl;
      cout << "Try 'A1 -h' for help" << endl;
      return 0;
    }

    else if (flags.inFiles.size()>1)
    {
      cout << "A1 does not currently support multiple files." << endl;
      cout << "Try 'A1 -h' for help" << endl;
      return 0;
    }
    program.resolveProgram(flags.inFiles[0], flags.debug);

    if (flags.runInterpreted)
{
  if (error.getIfErrorLogged())
  {
    if (flags.debug)
      cout << endl << ">>>>>>execution aborted due to previous error<<<<<<" << endl;
    else
      cout << "Program not compiled due to errors." << endl;
  }
  else
  {
    if (flags.debug)
      cout << endl << "Program is running...." << endl << endl;

    program.execute();
  }
}

if (!flags.cppOutFile.empty() || !flags.binOutFile.empty() || flags.runCompiled)
{
  string cppCode=program.getCpp();

  if (error.getIfErrorLogged())
  {
    if (flags.debug)
      cout << endl << ">>>>>>Transpiling to C++ failed<<<<<<" << endl;
    else
      cout << "Transpiling Failed" << endl;
  }
  else
  {
    string cppFileName=flags.cppOutFile;

    if (cppFileName.empty())
      cppFileName="tmp_ad_transpiled.cpp";

    if (flags.debug)
      cout << endl << putStringInBox(cppCode, "C++ code", true, false, -1) << endl;

    writeFile(cppFileName, cppCode, flags.debug);

    if (!flags.binOutFile.empty() || flags.runCompiled)
    {
      string binFileName=flags.binOutFile;

      if (binFileName.empty())
        binFileName="tmp_ad_compiled";

      string cmd;
      cmd="g++ -std=c++11 '"+cppFileName+"' -o '"+binFileName+"'";

      if (flags.debug)
        cout << "running '"+cmd+"'" << endl;

      runCmd(cmd, true);

      if (flags.runCompiled)
      {
        if (flags.debug)
          cout << endl;

        cmd = "./"+binFileName + " --running-from-A1 " + str::join(cmdLineArgs, " ", false);

        if (flags.debug)
          cout << "running '"+cmd+"'" << endl << endl;

        runCmd(cmd, true);
      }

      if (flags.binOutFile.empty())
        remove(binFileName.c_str());
    }

    if (flags.cppOutFile.empty())
      remove(cppFileName.c_str());
  }
}

if (flags.debug)
  cout << endl << "all done" << endl;

return 0;
}

Flags getFlags(int argc, char ** argv)
{
bool after = false;
Flags flags;

for (int i=1; i<argc; i++)
{
  string arg(argv[i]);
  if (!after)
  {
    if (arg.size()>1 && arg[0]=='-')
    {
      string flag=arg.substr(1, string::npos);

      if (flag=="d" || flag=="debug")
      {
        flags.debug=true;
      }
      else if (flag=="v" || flag=="version")
      {
        flags.version=true;
      }
      else if (flag=="h" || flag=="help")
      {
        flags.help=true;
      }
      else if (flag=="r" || flag=="run")
      {
        flags.runCompiled=false;
        flags.runInterpreted=true;
        after = true;
      }
      else if (flag=="cpp")
      {
        if (i+1>=argc)
        {
          cout << "output file must follow '-cpp' flag";
          flags.flagError=true;
        }

        flags.runInterpreted=false;

        flags.cppOutFile=string(argv[i+1]);

        i++;
      }
      else if (flag=="bin")
      {
        if (i+1>=argc)
        {
          cout << "output file must follow '-bin' flag";
          flags.flagError=true;
        }

        flags.runInterpreted=false;

        flags.binOutFile=string(argv[i+1]);

        i++;
      }
      else if (flag=="e" || flag=="execute")
      {
        flags.runCompiled=true;
        flags.runInterpreted=false;
        after = true;
      }
      else
      {
        cout << "unknown flag '"+flag+"'" << endl;
        flags.flagError=true;
      }
    }
    else
    {
      flags.inFiles.push_back(arg);
      cmdLineArgs.push_back(arg);
    }
  }
  else
  {
    cmdLineArgs.push_back(arg);
  }
}

return flags;
}
