#ifndef CLIANIMATION_H
#define CLIANIMATION_H

#include <vector>
#include <sstream>

struct Spinner
{
  int _i = 0;
  std::vector<std::string> _sprites;
  Spinner(std::vector<std::string> sprites) : _sprites(sprites) {}
  std::string operator()()
  {
    return _sprites.size() == 0
      ? ""
      : _sprites[_i++ % _sprites.size()];
  }
  int spriteSize()
  {
    return _sprites.size() == 0
      ? 0
      : _sprites[_i % _sprites.size()].size();
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
    : _spin({ "-", "\\", "|", "/" })
    //: _spin({})
    //: _spin({ "|-  |","| - |","|  -|","| - |" })
    //: _spin({ "", "~", "<>", "<~>", "<==>", "<=~=>", "<-==->", "<=~=>", "<==>", "<~>", "<>", "{" })
    , _length(20)
    , _showPercentage(true)
    , _showSpinner(true)
    , _loadedSymbol('|')
    , _notLoadedSymbol(' ')
  {}

  LoadingBar(char loadedSymbol, char notLoadedSymbol = ' ', int length = 20, bool showPercentage = true, Spinner spinner = Spinner({}))
    : _spin(spinner)
    , _length(length)
    , _showPercentage(showPercentage)
    , _showSpinner(_spin._sprites.size() > 0)
    , _loadedSymbol(loadedSymbol)
    , _notLoadedSymbol(notLoadedSymbol)
  {}


  std::string operator()(float progress, std::string message = "")
  {
    std::stringstream out;

    if (_showPercentage)
    {
      std::string perc = std::to_string(static_cast<int>(progress * 100));

      while (perc.size() < 3)
        perc.insert(0, " ");

      out << perc.substr(0, 3) << "% ";
    }

    out << '[';

    int currentProgress = progress * _length;

    int effectiveSpinnerSize = 0;

    if (_showSpinner)
    {
      effectiveSpinnerSize =
        _length - currentProgress > _spin.spriteSize()
        ? _spin.spriteSize()
        : _length - currentProgress;

      for (int i = 0; i < currentProgress; ++i)
        out << _loadedSymbol;

      std::string str = _spin().substr(0, effectiveSpinnerSize);
      out << str;
    }
    else
    {
      for (int i = 0; i < currentProgress; ++i)
        out << _loadedSymbol;
    }

    int notLoaded =
      _length - effectiveSpinnerSize - currentProgress < 0
      ? 0
      : _length - effectiveSpinnerSize - currentProgress;

    for (int i = 0; i < notLoaded; ++i)
      out << _notLoadedSymbol;

    out << "] ";
    if (message.size())
    {
      out << ' ' << message;
    }

    return out.str();
  }

};

#endif

