[![License:MIT](https://img.shields.io/badge/License-MIT-blue?style=plastic)](LICENSE)
[![C++ CI build](../../actions/workflows/build.yml/badge.svg)](../../actions/workflows/build.yml)
### bandwidth_cpp

minimal bandwidth monitoring library for C++

### <sub>notes</sub>

due to name unifying, this replaces older [bandwidth](https://github.com/oskarirauta/bandwidth) which is now archived.

## <sub>Importing</sub>

bandwidth_cpp depends on [scanner_cpp](https://github.com/oskarirauta/scanner_cpp) and example
program depends also on [cmdparser_cpp](https://github.com/oskarirauta/cmdparser_cpp) which is
not a requirement if you just need library.

 - clone scanner_cpp as submodule to scanner
 - clone bandwidth_cpp as submodule to bandwidth
 - include scanner/Makefile.inc and bandwidth/makefile.inc in your Makefile
 - link with SCANNER_OBJS and BANDWIDTH_OBJS

Paths are modifiable, check Makefiles. For example program, remember to clone this
repository recursively with --recursive-submodules enabled for cloning.

## <sub>Example</sub>

Sample program for interface bandwidth monitoring, is provided. Clone recursively to build.
