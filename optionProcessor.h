#ifndef OPTIONPROCESSOR_H
#define OPTIONPROCESSOR_H

#include <string>
#include <vector>
#include <functional>
#include <algorithm>

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

    // Shortcut for find/find_if algorithms
    #define ALL(x) x.begin(), x.end() 

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
    #undef ALL
  }
};

// Shortcut macros
#define OPTION(opt,lambda) { { opt }, [&](const Option & option) lambda }
#define OPTION2(opt1, opt2, lambda) { { opt1, opt2 }, [&](const Option & option) lambda }
#define BADOPTIONS(lambda) { {"Bad options"}, [&](const Option & option) lambda }

#endif // OPTIONPROCESSOR_H
