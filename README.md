# Overview

The main aim is to create a small header-only class that handles basic command-line parameter parsing.
This class should simplify the process of grabbing optional settings and parameterised intructions that are passed to the executable.

## Examples of Use
```
:\>MyApp.exe param1 param2
```
```
:\>MyApp.exe --firstOption="1234" -f 1234
```
```
:\>MyApp.exe --secondOption:1234 -s1234
```