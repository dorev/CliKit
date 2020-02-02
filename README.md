# CliKit
A collection of useful and useless things to support C++ CLI design

![Demo](https://i.gyazo.com/8e4415c66ea0d16dce88e127792525a9.gif)


# Why
I hate `getopt`, I wanted the cleanest argument parser possible, so I wrote this first version of `optionProcessor.h`. I thought it would fit well in a toolkit to support CLI designs so I figured I might starting gathering a few things here for that purpose.

# What's in it?
This is not "low-level zero overhead black-magic" code. The mindset here is to promote readability and flexibility. So far, the elements contained in the kit are :
* Option processor, to crunch your `argc` and `argc` buddies
* A spinner, with customizable sprites
* A loading bar, in which you can put a spinner!

## Option processor
Include `optionProcessor.h` to your project to make your life simpler. 

    int main(int argc, char** argv)
    {
      ProcessOptions(argc, argv) ({
        BADOPTIONS (               { usage(); }),
        OPTION     ("-a",          { flag_a = true; }),
        OPTION2    ("-b", "--bye", { prepare(option.arguments); })
        });

      ...

* Any argument of your program starting with a `-` will be detected as an option and the following words as the option's argument.
* Options are defined with the `OPTION` and `OPTION2` macros, which should be provided one or two string and a function body.
* Within that function body, `option.arguments` contains a string vector of option arguments.
* `BADOPTIONS` is optional and is called if a non-defined options are detected. Their names are provided in `option.arguments`.

## CLI animations
The following elements can be found in `cliAnimations.h`

### Spinner
A `Spinner` object to load with the sprite sequence you want it to show. Sprites are `std::string`s and can be of different length.

    Spinner spin({ "-", "\\", "|", "/" });
    while(1) { cout << '\r' << spin() << flush; this_thread::sleep_for(100ms); }
    
![you spin me right round baby right round...!](https://i.gyazo.com/e2e125edb8b0fb0fe4e4f5c3872b5c94.gif)

### Loading bar
The `LoadingBar` object has a few options :
* Adjustable length
* Toggle percentage display
* Customizable loaded/not-loaded symbols
* Can have a `Spinner`

Sample loader :

    Spinner spinner({ "|X  |","| X |","|  X|","| X |" });
    LoadingBar loader('X', '-', 60, true, spinner);

    while (progress < 1)
    {
      cout << '\r' << loader(progress);
      progress += 0.03;
      this_thread::sleep_for(100ms);
    }
    cout << '\r' << loader(1) << flush;
    
![ths is safe for work](https://i.gyazo.com/1939e04f22e6c99e58c1963d39d2ba1c.gif)
