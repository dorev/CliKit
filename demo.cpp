#include <iostream>
#include <thread>
#include <chrono>

#include "clikit.h"

using namespace std::chrono_literals;
using namespace clikit;

  
int main(int argc, char** argv)
{
 std::cout << "Command line : ";
  for (int i = 0; i < argc; ++i)
    std::cout << argv[i] << " ";
  std::cout << "\n";

  ProcessOptions(argc, argv)({
    BADOPTIONS(
    {
      std::cout << "\nBad options detected : ";
      for (auto& arg : option.arguments)
        std::cout << arg << " ";
      std::cout << "(we could print usage and exit program)";
    }),
    OPTION("-a", 
    {
      std::cout << "\n-a option";
      if (option.arguments.size())
      {
        std::cout << " with arguments : ";
        for (auto& arg : option.arguments)
          std::cout << arg << " ";
      }
    }),
    OPTION2("-b", "--blong", 
    {
      std::cout << "\n-b/--blong option";
      if (option.arguments.size())
      {
        std::cout << " with arguments : ";
        for (auto& arg : option.arguments)
          std::cout << arg << " ";
      }
    })
  });

  std::cout << "\n\nDefault loading bar leading to default shell\n";
  LoadingBar load;

  float progress = 0;
  while (progress < 1)
  {
    std::cout << "\r" << load(progress += 0.04) << std::flush;
    std::this_thread::sleep_for(100ms);
  }
  std::cout << "\r" << load(1) << std::flush;

  Shell()({
    COMMAND("such", 
    {
      std::cout << "much " 
                << (arguments.size() < 1 
                ? "nothing" 
                : arguments[0]);
    }),
    DEFAULT(
    {
      std::cout << "Default processing with argument(s) : ";
      for(auto& args : arguments)
        std::cout << args << " ";
    }),
    EXIT({
      std::cout << "exit command";
    })
  });

  std::cout << "\nCustom loading bar leading to custom shell\n";

  Spinner spinner({ "=>", "=<>", "===<", "====<", "===<",  "=<>", "=>" });
  LoadingBar loadCustom(' ', '-', 40, false, spinner);

  progress = 0;
  while (progress < 1)
  {
    std::cout << "\r" << loadCustom(progress += 0.04) << std::flush;
    std::this_thread::sleep_for(100ms);
  }
  std::cout << "\r" << loadCustom(1) << std::flush;


  Shell("~§~ ", "aveda")({
    COMMAND("destroy", 
    {
      for(auto& args : arguments)
        std::cout << args << " will be destroyed\n";
    }),
    DEFAULT(
    {
      std::cout << "Default processing with argument(s) : ";
      for(auto& args : arguments)
        std::cout << args << " ";
    }),
    EXIT({
      std::cout << "KEDAVRA!! (leaving shell)";
    })
  });

  return 0;
}