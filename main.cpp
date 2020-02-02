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

  std::cout << "Arguments : ";
  for (int i = 1; i < argc; ++i)
    std::cout << argv[i] << " ";
  std::cout << "\n";

  ProcessOptions(argc, argv)({
    BADOPTIONS(
    {
      std::cout << "\nBad options : ";
      for (auto& arg : option.arguments)
        std::cout << arg << " ";
      std::cout << "(we could print usage and exit program)";
    }),

    OPTION("-a", 
    {
      std::cout << "\noption -a with argument ";
      for (auto& arg : option.arguments)
        std::cout << arg << " ";
    }),

    OPTION2("-b", "--blong", 
    {
      std::cout << "\noption "<< option.name << " with argument ";
      for (auto& arg : option.arguments)
        std::cout << arg << " ";
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
    std::cout << "\r" << load(progress);
    progress += 0.03;

    std::this_thread::sleep_for(100ms);
  }
  std::cout << "\r" << load(1);

  //
  // Custom loading bar
  //

  std::cout << "\n\nCustom loading bar :\n";

  Spinner spinner({ "=>", "=<>", "===<", "====<", "===<",  "=<>", "=>" });
  LoadingBar loadCustom(' ', '-', 60, false, spinner);

  progress = 0;
  while (progress < 1)
  {
    std::cout << "\r" << loadCustom(progress);
    progress += 0.03;

    std::this_thread::sleep_for(100ms);
  }
  std::cout << "\r" << loadCustom(1);

  getchar();

  return 0;
}