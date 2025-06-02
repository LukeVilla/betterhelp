This program is an improvement over the standard Linux `help` utility.
Instead of just printing out a static list of built-in commands with no descriptions, `betterhelp` runs on all files in `/bin` and uses data from man files.
To compile it, just run `g++ -o betterhelp betterhelp.cpp`.
The only dependency is [Boost](https://www.boost.org/).