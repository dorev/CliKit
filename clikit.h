#ifndef CLIKIT_H
#define CLIKIT_H

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <functional>
#include <algorithm>

namespace clikit
{
/*
  ProcessOptions is a functor constructed with argc/argv
  and given a list of OptionDefinitions used to parse argv. 
	
	CommandShell uses a similar pattern, it is constructed with
	its line beginning symbol and exit command name

*/

// Shortcut for find/find_if algorithms
#define ALL(x) x.begin(), x.end() 

using StringVector = std::vector<std::string>;

struct Option
{
  std::string name;
  StringVector arguments;
};

struct OptionDefinition
{
  StringVector names;
  std::function<void(const StringVector&)> lambda;
};

struct ProcessOptions
{
  int _argc;
  char** _argv;
  int _argvIndex;
	StringVector _commandLine;
  std::vector<Option> _options;

  ProcessOptions(int argc, char** argv)
    : _argc(argc)
    , _argv(argv)
    , _argvIndex(0)
  {
    // Skip argv[0]
    for (int i = 1; i < argc; ++i)
      _commandLine.push_back(argv[i]);

    // Parse
    while(_argvIndex < _argc - 1 && _commandLine[_argvIndex][0] == '-')
    {
      Option option { _commandLine[_argvIndex++] };

      // Extract each argv value with a '-' as an option 
      while (_argvIndex < _argc - 1 && _commandLine[_argvIndex][0] != '-')
        option.arguments.push_back(_commandLine[_argvIndex++]);

      _options.push_back(option);
    }
  }

  void operator()(std::vector<OptionDefinition> optionDefinitions)
  {
    StringVector definedOptions;

    for(auto& option : optionDefinitions)
      for (auto& name : option.names)
        definedOptions.push_back(name);

    // Check if "Bad Options" is present
    auto badOptionItr = std::find_if(ALL(optionDefinitions), [&](const OptionDefinition& definition)
    {
      return std::find(ALL(definition.names), "Bad options") != definition.names.end();
    });

    if (badOptionItr != optionDefinitions.end())
    {
      Option badOption { "Bad options" };

      // List bad options names as "Bad Options" arguments
      std::for_each(_commandLine.begin(), _commandLine.end(), [&](std::string& arg)
      {
        if (arg[0] == '-' && std::find(ALL(definedOptions), arg) == definedOptions.end())
          badOption.arguments.push_back(arg);
      });

      // Run "Bad Options" lambda
      if (badOption.arguments.size())
        badOptionItr->lambda(badOption.arguments);
    }

    // Process valid options
    for (auto& option : _options)
    {
      // For each defined options found in argv
      auto definitionItr = std::find_if(ALL(optionDefinitions), [&](const OptionDefinition& definition)
      {
        return std::find_if(ALL(definition.names), 
          [&](const std::string& definitionName) 
          { 
            return option.name == definitionName; 
          }) != definition.names.end();
      });

      // Run option lambda
      if (definitionItr != optionDefinitions.end())
        definitionItr->lambda(option.arguments);
    }
  }

	// Shortcut macros
	#define OPTION(opt,lambda)          { { opt }, [&](const StringVector & args) lambda }
	#define OPTION2(opt1, opt2, lambda) { { opt1, opt2 }, [&](const StringVector & args) lambda }
	#define BADOPTIONS(lambda)          { {"Bad options"}, [&](const StringVector& args) lambda }

};

/* 
	Helper function to split string on requested delimiter
*/
void strsplit(StringVector& out, std::string& str, std::string delim = " ")
{
  size_t last = 0;
  size_t next = 0;

  while ((next = str.find(delim, last)) != std::string::npos)
  {
    out.push_back(str.substr(last, next - last));
    last = next + delim.size();
  }

  if (last < str.size())
    out.push_back(str.substr(last));
}

struct CommandDefinition
{
  std::string name;
  std::function<void(const StringVector&)> lambda;
};

struct CommandShell
{
  std::string _shellSymbol;
  std::string _exitCommand;

  CommandShell()
    : _shellSymbol("> ")
    , _exitCommand("exit")
  {}

  CommandShell(std::string shellSymbol, std::string exitCommand = "exit")
    : _shellSymbol(shellSymbol)
    , _exitCommand(exitCommand)
  {}

  void operator()(std::vector<CommandDefinition> commandDefinitions)
  {
    std::string input = "";

		// Look for unknown commands processing
    auto defaultItr = std::find_if(ALL(commandDefinitions), [&](const CommandDefinition commandDefinition)
    {
      return commandDefinition.name == "Default command";
    });

		// Look for exit command
    auto exitItr = std::find_if(ALL(commandDefinitions), [&](const CommandDefinition commandDefinition)
    {
      return commandDefinition.name == "Exit command";
    });

    std::string line;
    StringVector splittedLine;
    
    // Start shell display
    std::cout << "\n";
    bool stayInShell = true;
    while(stayInShell)
    {      
      std::cout << _shellSymbol;
			
    	// Capture line on "Enter"
      std::getline(std::cin, line);
      
			// Reset an empty line
    	if(line.size() == 0)
        continue;

			// Split the line words
      strsplit(splittedLine, line);

			// Look for a known command
      auto commandItr = std::find_if(ALL(commandDefinitions), [&](const CommandDefinition commandDefinition)
      {
        return commandDefinition.name == splittedLine[0];
      });

			// Process input in the following order : 
    	// 1- exit command 
    	// 2- known command
    	// 3- unknown command
      if(splittedLine[0] == _exitCommand)
      {
        if(exitItr != commandDefinitions.end())
          exitItr->lambda({});
        stayInShell = false;
      }   
      else if(commandItr != commandDefinitions.end())
      {
				// Remove command word
        splittedLine.erase(splittedLine.begin());
        commandItr->lambda(splittedLine);
      }
      else if (defaultItr != commandDefinitions.end())
      {
        defaultItr->lambda(splittedLine);
      }

			// Clear and loop
      splittedLine.clear();
      line.clear();
      std::cout << "\n";
    }

    std::cout << "\n";
  }

	// Shortcut macros
	#define COMMAND(cmd, lambda)  { cmd, [&](const StringVector& args) lambda }
	#define DEFAULT(lambda)       { "Default command", [&](const StringVector& args) lambda }
	#define EXIT(lambda)          { "Exit command", [&](const StringVector& args) lambda }

};

struct Spinner
{
  int _i = 0;
	StringVector _sprites;
  bool _empty;

  Spinner(StringVector sprites) 
    : _sprites(sprites) 
    , _empty(sprites.empty())
  {}

	// Get next sprite
  std::string operator()()
  {
    return _empty ? "" : _sprites[_i++ % _sprites.size()];
  }

	// Get size of the next sprite to print
  int spriteSize()
  {
    return _empty ? 0 : _sprites[_i % _sprites.size()].size();
  }
};

struct LoadingBar
{
  Spinner _spin;
  int _length;
  bool _showPercentage;
  bool _showSpinner;
  char _loadedSymbol;
  char _notLoadedSymbol;

	// Default loading bar
  LoadingBar()
    : _spin({ "-", "\\", "|", "/" }), _length(20)
    , _showPercentage(true)
    , _showSpinner(true)
    , _loadedSymbol('|')
    , _notLoadedSymbol(' ')
  {}

	// Custom constructor
  LoadingBar( char loadedSymbol, 
              char notLoadedSymbol = ' ', 
              int length = 20, 
              bool showPercentage = true, 
              Spinner spinner = Spinner({}) )
    : _spin(spinner)
    , _length(length)
    , _showPercentage(showPercentage)
    , _showSpinner(!_spin._empty)
    , _loadedSymbol(loadedSymbol)
    , _notLoadedSymbol(notLoadedSymbol)
  {} 

  std::string operator()(float progress)
  {
    // Accumulator
    std::stringstream out;

    if (_showPercentage)
    {
      std::string perc(std::to_string(static_cast<int>(progress * 100)));

      // Left pad with spaces
      while (perc.size() < 3)
        perc.insert(0, " ");

      // Truncate overflow
      out << perc.substr(0, 3) << "% ";
    }

    // Loading bar left end
    out << '[';

    int currentProgress = progress * _length;
    int effectiveSpinnerSize = 0;

    if (_showSpinner)
    {
      // Calculated to truncate overflow at the end of the loading bar
      effectiveSpinnerSize = _length - currentProgress > _spin.spriteSize()
        ? _spin.spriteSize()
        : _length - currentProgress;

      // Print progress symbols
      for (int i = 0; i < currentProgress; ++i)
        out << _loadedSymbol;

      // Print potentially truncated spinner
      out << _spin().substr(0, effectiveSpinnerSize);
    }
    else
    {
      // Simply print progress symbols
      for (int i = 0; i < currentProgress; ++i)
        out << _loadedSymbol;
    }

    // Print remaining spaces with remainder symbol
    int remainingSpace = _length - effectiveSpinnerSize - currentProgress < 0
      ? 0
      : _length - effectiveSpinnerSize - currentProgress;

    for (int i = 0; i < remainingSpace; ++i)
      out << _notLoadedSymbol;

    // Loading bar right end
    out << "]";
    
    return out.str();
  }

};

#undef ALL

} // namespace clikit

#endif // CLIKIT_H
