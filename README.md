# Overview

A small header-only class that handles basic command-line optional parameter parsing.
This class should simplify the process of grabbing optional settings and parameterised intructions that are passed to the executable.

# Content
The `cmdparse.h` file is a header file in C++ that provides a basic command-line parser for C++ applications. It is part of the `WGT` namespace and contains two main classes: `cmdOption` and `cmdParse`.

1. `cmdOption`: This struct encapsulates a single command-line option. It has several member variables to store the long name, short name, default value, and parameter value of the option. 

2. `cmdParse`: This is the main command-line options handler class. It provides methods to initialize the command-line handler with the arguments given to the application


## Examples of Use
```
:\>MyApp.exe --firstOption="1234" -f 1234
```
```
:\>MyApp.exe --secondOption:1234 -s1234
```

## References
See: [main function](https://learn.microsoft.com/en-us/cpp/cpp/main-function-command-line-args?view=msvc-170)