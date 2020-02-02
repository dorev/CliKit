#ifndef OPTIONPROCESSOR_H
#define OPTIONPROCESSOR_H

#include <string>
#include <vector>
#include <functional>
#include <algorithm>

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
  std::vector<std::string> _cl;
  std::vector<Option> _options;
  int _i = 0;
  int _argc;
  char** _argv;

  ProcessOptions() {}

  ProcessOptions(int argc, char** argv)
    : _argc(argc)
    , _argv(argv)
  {
    for (int i = 1; i < argc; ++i)
      _cl.push_back(argv[i]);

    // Parse
    while(_i < _argc - 1 && _cl[_i][0] == '-')
    {
      Option option;
      option.name = _cl[_i];
      ++_i;

      while (_i < _argc - 1 && _cl[_i][0] != '-')
      {
        option.arguments.push_back(_cl[_i]);
        ++_i;
      }
      _options.push_back(option);
    }
  }

  void operator()(std::vector<OptionDefinition> optionDefinitions)
  {
    std::vector<std::string> definedOptions;

    for(auto& option : optionDefinitions)
    {
      for (auto& name : option.names)
        definedOptions.push_back(name);
    }

    // check if bad option is present
    #define ALL(x) x.begin(), x.end() 
    auto badOptionItr = std::find_if(ALL(optionDefinitions), [&](const OptionDefinition& definition)
    {
      return std::find(ALL(definition.names), "Bad options") != definition.names.end();
    });

    if (badOptionItr != optionDefinitions.end())
    {
      Option badOption{ "Bad options" };

      std::for_each(_cl.begin(), _cl.end(), [&](std::string& arg)
      {
        if (arg[0] == '-' && std::find(ALL(definedOptions), arg) == definedOptions.end())
          badOption.arguments.push_back(arg);
      });

      if (badOption.arguments.size())
        badOptionItr->lambda(badOption);
    }

    // Process valid options
    for (auto& option : _options)
    {
      auto definitionItr = std::find_if(ALL(optionDefinitions), [&](const OptionDefinition& definition)
      {
        return std::find_if(ALL(definition.names), [&](const std::string& definitionName) { return option.name == definitionName; }) != definition.names.end();
      });

      if (definitionItr != optionDefinitions.end())
        definitionItr->lambda(option);
    }
    #undef ALL
  }
};


#define OPTION(opt,lambda) { { opt }, [&](const Option & option) lambda }
#define OPTION2(opt1, opt2, lambda) { { opt1, opt2 }, [&](const Option & option) lambda }
#define BADOPTIONS(lambda) { {"Bad options"}, [&](const Option & option) lambda }

#endif // OPTIONPROCESSOR_H
