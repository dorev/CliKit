# CliKit
A header-only collection of useful and useless things to support C++ CLI design

![Demo](https://i.imgur.com/AH3a7Ua.gif)

# Why
I don't really like `getopt`, I wanted the cleanest argument parser possible, so I wrote a first version of `ProcessOptions` and then a few other things. I gathered these together here, and then CliKit was born!

# What's in it?
This is not "low-level zero overhead black-magic" code. The mindset here is to promote readability and flexibility. So far, the elements contained in the kit are :
* Option processor, to crunch your `argc` and `argv` buddies
* Command shell, to input runtime commands
* A spinner, with customizable sprites
* A loading bar, in which you can put a spinner!

## Option processor
A tool to launch your project quickly and make your life simpler. 

    int main(int argc, char** argv)
    {
      ProcessOptions(argc, argv) ({
        BADOPTIONS (               { usage(); }),
        OPTION     ("-a",          { flag_a = true; }),
        OPTION2    ("-b", "--bye", { prepare(args); })
      });

      ...

* Any argument of your program starting with a `-` will be detected as an option and the following words as option arguments.
* Options are defined with the `OPTION` and `OPTION2` macros, which should be provided one or two string and a function body.
* Within that function body, `args` contains a string vector of option arguments.
* `BADOPTIONS` is optional and is called if non-defined options are detected. Their names are provided in `args`.

## Command shell

The `CommandShell` uses a similar pattern :

    CommandShell("> ", "exit") ({
      COMMAND ("commandWord", { commandAction(args); }),
	  DEFAULT ({ processLine(args); }),
	  EXIT	  ({ exit(0); })
	});

* The (optional) constructor parameters define the shell symbol and the exit command word
* Single-word commands can be provided in the COMMAND macro with a function body
* Any non-defined command will be processed by the function body provided in the (optional) DEFAULT macro
* In every function body, `args` contains a string vector of the rest of the line's words
* The exit command has it's own macro for a different processing

Arrow-key detection (to bring up previous commands) is not currently supported to avoid external dependencies.

## CLI animations
The following elements can be found in `cliAnimations.h`

### Spinner
A `Spinner` object to load with the sprite sequence of your choice. Sprites can be strings of different length.

    Spinner spin({ "-", "\\", "|", "/" });
    while(1) 
    { 
      cout << '\r' << spin() << flush; 
      sleep_for(100ms); 
    }
    
![you spin me right round baby right round...!](https://i.gyazo.com/e2e125edb8b0fb0fe4e4f5c3872b5c94.gif)

### Loading bar
The `LoadingBar` object has a few options :
* Adjustable length
* Togglable percentage display
* Customizable loaded/not-loaded symbols
* Can have a `Spinner`

Sample loader :

    Spinner spinner({ "|X  |","| X |","|  X|","| X |" });
    LoadingBar loader('X', '-', 60, true, spinner);

    while (progress < 1)
    {
      cout << '\r' << loader(progress) << flush;
      progress += 0.03;
      sleep_for(100ms);
    }
    cout << '\r' << loader(1) << flush;
    
![loading demo](https://i.gyazo.com/1939e04f22e6c99e58c1963d39d2ba1c.gif)
