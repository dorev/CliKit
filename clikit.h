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

     using namespace std; // yeah yeah I know...

    // Shortcut for find/find_if algorithms
    #define ALL(x) x.begin(), x.end() 

    /*
        Helper function to split string on requested delimiter
    */
    void strsplit(vector<string>& out, string& str, string delim = " ")
    {
        size_t last = 0;
        size_t next = 0;

        while ((next = str.find(delim, last)) != string::npos)
        {
            out.push_back(str.substr(last, next - last));
            last = next + delim.size();
        }

        if (last < str.size())
        {
            out.push_back(str.substr(last));
        }
    }

    struct Option
    {
        string name;
        vector<string> arguments;
    };

    struct OptionDefinition
    {
        vector<string> names;
        function<void(const vector<string>&)> lambda;
    };

    struct ProcessOptions
    {
        int _argc;
        char** _argv;
        int _argvIndex;
        vector<string> _commandLine;
        vector<Option> _options;
        Option _firstArgs;

        ProcessOptions(int argc, char** argv)
            : _argc(argc)
            , _argv(argv)
            , _argvIndex(0)
            , _firstArgs({ "First args" })
        {
            // Skip argv[0]
            for (int i = 1; i < argc; ++i)
            {
                _commandLine.push_back(argv[i]);
            }

            // Parse arguments
            while(_argvIndex < _argc - 1)
            {
                if (_commandLine[_argvIndex][0] == '-')
                {
                    Option option { _commandLine[_argvIndex++] };

                    // Extract each following argv value as argument for option
                    while (_argvIndex < _argc - 1 && _commandLine[_argvIndex][0] != '-')
                    {
                        option.arguments.push_back(_commandLine[_argvIndex++]);
                    }
                    _options.push_back(option);
                }
                else
                {
                    // store as FirstArgs
                    _firstArgs.arguments.push_back(_commandLine[_argvIndex++]);
                }
            }
        }

        void operator()(vector<OptionDefinition> optionDefinitions)
        {
            vector<string> definedOptions;

            for (auto& option : optionDefinitions)
            {
                for (auto& name : option.names)
                {
                    definedOptions.push_back(name);
                }
            }
            // Check if "First args" is present
            auto firstArgsItr = find_if(ALL(optionDefinitions), [&](const OptionDefinition& definition)
            {
                return find(ALL(definition.names), "First args") != definition.names.end();
            });

            if (firstArgsItr != optionDefinitions.end())
            {
                firstArgsItr->lambda(_firstArgs.arguments);
            }

            // Check if "Bad Options" is present
            auto badOptionItr = find_if(ALL(optionDefinitions), [&](const OptionDefinition& definition)
            {
                return find(ALL(definition.names), "Bad options") != definition.names.end();
            });

            if (badOptionItr != optionDefinitions.end())
            {
                Option badOption{ "Bad options" };

                // List bad options names as "Bad Options" arguments
                for_each(_commandLine.begin(), _commandLine.end(), [&](string& arg)
                {
                    if (arg[0] == '-' && find(ALL(definedOptions), arg) == definedOptions.end())
                    {
                        badOption.arguments.push_back(arg);
                    }
                });

                // Run "Bad Options" lambda
                if (badOption.arguments.size())
                {
                    badOptionItr->lambda(badOption.arguments);
                }
            }

            // Process valid options
            for (auto& option : _options)
            {
                // For each defined options found in argv
                auto definitionItr = find_if(ALL(optionDefinitions), [&](const OptionDefinition& definition)
                {
                    return find_if(ALL(definition.names), 
                        [&](const string& definitionName) 
                        { 
                            return option.name == definitionName; 
                        }) != definition.names.end();
                });

                // Run option lambda
                if (definitionItr != optionDefinitions.end())
                {
                    definitionItr->lambda(option.arguments);
                }
            }
        }

        // Shortcut macros
        #define FIRSTARGS(lambda)                    { { "First args" }, [&](const vector<string> & args) lambda }
        #define OPTION(opt,lambda)                    { { opt }, [&](const vector<string> & args) lambda }
        #define OPTION2(opt1, opt2, lambda) { { opt1, opt2 }, [&](const vector<string> & args) lambda }
        #define BADOPTIONS(lambda)                    { {"Bad options"}, [&](const vector<string>& args) lambda }

    };

    struct CommandDefinition
    {
        string name;
        function<void(const vector<string>&)> lambda;
    };

    struct CommandShell
    {
        string _shellSymbol;
        string _exitCommand;

        CommandShell()
            : _shellSymbol("> ")
            , _exitCommand("exit")
        {}

        CommandShell(string shellSymbol, string exitCommand = "exit")
            : _shellSymbol(shellSymbol)
            , _exitCommand(exitCommand)
        {}

        void operator()(vector<CommandDefinition> commandDefinitions)
        {
            string input = "";

            // Look for unknown commands processing
            auto defaultItr = find_if(ALL(commandDefinitions), [&](const CommandDefinition commandDefinition)
            {
                return commandDefinition.name == "Default command";
            });

            // Look for exit command
            auto exitItr = find_if(ALL(commandDefinitions), [&](const CommandDefinition commandDefinition)
            {
                return commandDefinition.name == "Exit command";
            });

            string line;
            vector<string> splittedLine;
        
            // Start shell display
            cout << "\n";
            bool stayInShell = true;
            while(stayInShell)
            {            
                cout << _shellSymbol;
            
                // Capture line on "Enter"
                getline(cin, line);
            
                // Reset an empty line
                if (line.size() == 0)
                {
                    continue;
                }

                // Split the line words
                strsplit(splittedLine, line);

                // Look for a known command
                auto commandItr = find_if(ALL(commandDefinitions), [&](const CommandDefinition commandDefinition)
                {
                    return commandDefinition.name == splittedLine[0];
                });

                // Process input in the following order : 
                // 1- exit command 
                // 2- known command
                // 3- unknown command
                if(splittedLine[0] == _exitCommand)
                {
                    if (exitItr != commandDefinitions.end())
                    {
                        exitItr->lambda({});
                    }
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
                cout << "\n";
            }

            cout << "\n";
        }

        // Shortcut macros
        #define COMMAND(cmd, lambda)    { cmd, [&](const vector<string>& args) lambda }
        #define DEFAULT(lambda)         { "Default command", [&](const vector<string>& args) lambda }
        #define EXIT(lambda)            { "Exit command", [&](const vector<string>& args) lambda }

    };

    struct Spinner
    {
        int _i = 0;
        vector<string> _sprites;
        bool _empty;

        Spinner(vector<string> sprites) 
            : _sprites(sprites) 
            , _empty(sprites.empty())
        {}

        // Get next sprite
        string operator()()
        {
            return _empty ? "" : _sprites[_i++ % _sprites.size()];
        }

        // Get size of the next sprite to print
        size_t spriteSize()
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

        string operator()(float progress)
        {
            // Accumulator
            stringstream out;

            if (_showPercentage)
            {
                string perc(to_string(static_cast<int>(progress * 100)));

                // Left pad with spaces
                while (perc.size() < 3)
                {
                    perc.insert(0, " ");
                }

                // Truncate overflow
                out << perc.substr(0, 3) << "% ";
            }

            // Loading bar left end
            out << '[';

            float currentProgress = progress * _length;
            int effectiveSpinnerSize = 0;

            if (_showSpinner)
            {
                // Calculated to truncate overflow at the end of the loading bar
                effectiveSpinnerSize = _length - currentProgress > _spin.spriteSize()
                    ? static_cast<int>(_spin.spriteSize())
                    : static_cast<int>(_length - currentProgress);

                // Print progress symbols
                for (int i = 0; i < currentProgress; ++i)
                {
                    out << _loadedSymbol;
                }

                // Print potentially truncated spinner
                out << _spin().substr(0, effectiveSpinnerSize);
            }
            else
            {
                // Simply print progress symbols
                for (int i = 0; i < currentProgress; ++i)
                {
                    out << _loadedSymbol;
                }
            }

            // Print remaining spaces with remainder symbol
            int remainingSpace = _length - effectiveSpinnerSize - currentProgress < 0
                ? 0
                : static_cast<int>(_length - effectiveSpinnerSize - currentProgress);

            for (int i = 0; i < remainingSpace; ++i)
            {
                out << _notLoadedSymbol;
            }

            // Loading bar right end
            out << "]";
        
            return out.str();
        }

    };

    #undef ALL

} // namespace clikit

#endif // CLIKIT_H
