This program is an improvement over the standard Linux `help` utility.
Instead of just printing out a static list of built-in commands with no descriptions, `betterhelp` runs on all files in `/bin` by default and uses data from man files.

# Arguments

- `-n`/`--nocache`: Disables caching. This option can also be controlled with a #define.
- `-r`/`--rebuildcache`: Rebuilds the description (.bhmans) cache
- `-v`/`--verbose`: Adds extra logging
- Positional argument (without a flag): Display help for the given command

# Dependencies

- [Boost](https://www.boost.org/)
- [Argengine](https://github.com/juzzlin/Argengine)
