#include <iostream>
#include <thread>
#include <chrono>

#include "optionProcessor.h"
#include "cliAnimations.h"

using namespace std::chrono_literals;

int main(int argc, char** argv)
{
  //
  // Setup and process arguments
  //
  getchar();

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
   
  //
  // Default loading bar
  //

  std::cout << "\n\nDefault loading bar :\n";
  LoadingBar load;

  float progress = 0;
  while (progress < 1)
  {
    std::cout << "\r" << load(progress) << std::flush;
    progress += 0.05;

    std::this_thread::sleep_for(100ms);
  }
  std::cout << "\r" << load(1) << std::flush;

  //
  // Custom loading bar
  //

  std::cout << "\n\nCustom loading bar :\n";

  Spinner spinner({ "=>", "=<>", "===<", "====<", "===<",  "=<>", "=>" });
  LoadingBar loadCustom(' ', '-', 40, false, spinner);

  progress = 0;
  while (progress < 1)
  {
    std::cout << "\r" << loadCustom(progress) << std::flush;
    progress += 0.05;

    std::this_thread::sleep_for(100ms);
  }
  std::cout << "\r" << loadCustom(1) << std::flush;

  getchar();

  return 0;
}