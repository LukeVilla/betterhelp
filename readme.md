This program is an improvement over the standard Linux `help` utility.
Instead of just printing out a static list of built-in commands with no descriptions, `betterhelp` runs on all files in `/bin` by default and uses data from man files.

# Arguments

- `-n`/`--nocache`: Disables caching. This option can also be controlled with a #define.
- `-r`/`--rebuildcache`: Rebuilds the cache

# Dependencies

- [Boost](https://www.boost.org/)
- [Argengine](https://github.com/juzzlin/Argengine)
