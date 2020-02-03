#ifndef CLIANIMATION_H
#define CLIANIMATION_H

#include <vector>
#include <sstream>

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

#endif

