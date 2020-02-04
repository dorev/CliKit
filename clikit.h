#ifndef OPTIONPROCESSOR_H
#define OPTIONPROCESSOR_H

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <functional>
#include <algorithm>


namespace clikit
{
  // Shortcut for find/find_if algorithms
  #define ALL(x) x.begin(), x.end() 




/*
  ProcessOptions is a functor constructed with argc/argv
  and given a list of OptionDefinitions used to parse argv. 
*/

struct Option
{
  std::string name;
  std::vector<std::string> arguments;
};

struct OptionDefinition
{
  std::vector<std::string> names;
  std::function<void(const Option&)> lambda;
};

struct ProcessOptions
{
  int _argc;
  char** _argv;
  int _argvIndex;
  std::vector<std::string> _commandLine;
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
    std::vector<std::string> definedOptions;

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
        badOptionItr->lambda(badOption);
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
        definitionItr->lambda(option);
    }
  }

// Shortcut macros
#define OPTION(opt,lambda)          { { opt }, [&](const Option & option) lambda }
#define OPTION2(opt1, opt2, lambda) { { opt1, opt2 }, [&](const Option & option) lambda }
#define BADOPTIONS(lambda)          { {"Bad options"}, [&](const Option & option) lambda }

};




struct CommandDefinition
{
  std::string name;
  std::function<void(const std::vector<std::string>&)> lambda;
};



void strsplit(std::vector<std::string>& out, std::string& str, std::string delim = " ")
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


struct Shell
{
  std::string _shellSymbol;
  std::string _exitCommand;

  Shell()
    : _shellSymbol("> ")
    , _exitCommand("exit")
  {}

  Shell(std::string shellSymbol, std::string exitCommand = "exit")
    : _shellSymbol(shellSymbol)
    , _exitCommand(exitCommand)
  {}

  void operator()(std::vector<CommandDefinition> commandDefinitions)
  {
    std::string input = "";

    auto defaultItr = std::find_if(ALL(commandDefinitions), [&](const CommandDefinition commandDefinition)
    {
      return commandDefinition.name == "Default command";
    });

    auto exitItr = std::find_if(ALL(commandDefinitions), [&](const CommandDefinition commandDefinition)
    {
      return commandDefinition.name == "Exit command";
    });

    bool stayInShell = true;
    std::string line;
    std::vector<std::string> splittedLine;
    
    std::cout << "\n";
      
    while(stayInShell)
    {
      std::cout << _shellSymbol;
      
      std::getline(std::cin, line);
      if(line.size() == 0)
        continue;

      strsplit(splittedLine, line);

      auto commandItr = std::find_if(ALL(commandDefinitions), [&](const CommandDefinition commandDefinition)
      {
        return commandDefinition.name == splittedLine[0];
      });

      if(splittedLine[0] == _exitCommand)
      {
        if(exitItr != commandDefinitions.end())
          exitItr->lambda({});
        stayInShell = false;
      }   
      else if(commandItr != commandDefinitions.end())
      {
        splittedLine.erase(splittedLine.begin());
        commandItr->lambda(splittedLine);
      }
      else if (defaultItr != commandDefinitions.end())
      {
        defaultItr->lambda(splittedLine);
      }

      splittedLine.clear();
      line.clear();
      std::cout << "\n";
    }

    std::cout << "\n";
  }

// Shortcut macros
#define COMMAND(cmd, lambda)  { cmd, [&](const  std::vector<std::string>& arguments) lambda }
#define DEFAULT(lambda)       { "Default command", [&](const std::vector<std::string>& arguments) lambda }
#define EXIT(lambda)          { "Exit command", [&](const std::vector<std::string>& arguments) lambda }

};










struct Spinner
{
  int _i = 0;
  bool _empty;
  std::vector<std::string> _sprites;

  Spinner(std::vector<std::string> sprites) 
    : _sprites(sprites) 
    , _empty(sprites.size() == 0)
  {}

  std::string operator()()
  {
    return _empty ? "" : _sprites[_i++ % _sprites.size()];
  }
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

  LoadingBar()
    : _spin({ "-", "\\", "|", "/" }), _length(20)
    , _showPercentage(true)
    , _showSpinner(true)
    , _loadedSymbol('|')
    , _notLoadedSymbol(' ')
  {}

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

#endif // OPTIONPROCESSOR_H
