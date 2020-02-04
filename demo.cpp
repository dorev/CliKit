#include <iostream>
#include <thread>
#include <chrono>

#include "clikit.h"

using namespace std::chrono_literals;
using namespace clikit;

  
int main(int argc, char** argv)
{
	//
	// Showcasing option processing
	//
  ProcessOptions(argc, argv)({
    BADOPTIONS(
    {
      std::cout << "\nBad options detected : ";
      for (auto& arg : args)
        std::cout << arg << " ";
      std::cout << "(we could print usage and exit program)";
    }),
    OPTION("-a", 
    {
      std::cout << "\n-a option";
      if (!args.empty())
      {
        std::cout << " with " << args.size() << " argument(s) : ";
        for (auto& arg : args)
          std::cout << arg << " ";
      }
    }),
    OPTION2("-b", "--blong", 
    {
      std::cout << "\n-b/--blong option";
      if (!args.empty())
      {
        std::cout << " with arguments : ";
        for (auto& arg : args)
          std::cout << arg << " ";
      }
    })
  });

	if(argc == 1)
		std::cout << "No arguments to parse\n";
	
	//
	// Showcasing default loading bar and default command shell
	//
	std::cout << "\n\nDefault loading bar leading to default shell\n\n";
  LoadingBar load;

  float progress = 0;
  while (progress < 1)
  {
    std::cout << "\r" << load(progress += 0.04) << std::flush;
    std::this_thread::sleep_for(100ms);
  }
  std::cout << "\r" << load(1) << std::flush;

  CommandShell()({
    COMMAND("such", 
    {
      std::cout << "much " << (args.empty() ? "nothing" : args[0]);
    }),
    DEFAULT(
    {
      std::cout << "Default processing with argument(s) : ";
      for(auto& arg : args)
        std::cout << arg << " ";
    })
  });

	//
	// Showcasing customized loading bar with customized command shell
	//
  std::cout << "Custom loading bar leading to custom shell\n\n";

  Spinner spinner({ "=>", "=<>", "===<", "====<", "===<",  "=<>", "=>" });
  LoadingBar loadCustom(' ', '-', 40, false, spinner);

  progress = 0;
  while (progress < 1)
  {
    std::cout << "\r" << loadCustom(progress += 0.04) << std::flush;
    std::this_thread::sleep_for(100ms);
  }
  std::cout << "\r" << loadCustom(1) << std::flush;


  CommandShell("~| ", "aveda")({
    COMMAND("destroy", 
    {
      for(auto& arg : args)
        std::cout << arg << " will be destroyed\n";
    }),
    DEFAULT(
    {
      std::cout << "Default processing with argument(s) : ";
      for(auto& arg : args)
        std::cout << arg << " ";
    }),
    EXIT({
      std::cout << "KEDAVRA!! (leaving shell)";
    })
  });

	getchar();
  return 0;
}