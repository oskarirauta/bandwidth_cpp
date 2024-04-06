[![License:MIT](https://img.shields.io/badge/License-MIT-blue?style=plastic)](LICENSE)
[![C++ CI build](../../actions/workflows/build.yml/badge.svg)](../../actions/workflows/build.yml)
### bandwidth_cpp

minimal bandwidth monitoring library for C++

### <sub>notes</sub>

due to name unifying, this replaces older [bandwidth](https://github.com/oskarirauta/bandwidth) which is now archived.

## <sub>Percentage</sub>

Percentage is based on comparison to highest recorded value. In the beginning even very small
transmissions will show high percentage values even on a high speed broadband connection.
To get accurate percentage, one should stress connection to maximum rate - for example, by
using [speedtest.net](https://speedtest.net) for more accurate results.

### <sub>percentage of example program</sub>
example program is fixed to show 0.01% even when it should show 0, if rate exceeds 4kb
to show atleast some percentage even on very small transmissions when higher speed
connection is being used - even though in calculus it's in-accurate.

## <sub>Importing</sub>

bandwidth_cpp depends on [common_cpp](https://github.com/oskarirauta/common_cpp) and example
program depends also on [cmdparser_cpp](https://github.com/oskarirauta/cmdparser_cpp) which is
not a requirement if you just need library.

 - clone common_cpp as submodule to scanner
 - clone bandwidth_cpp as submodule to bandwidth
 - include common/Makefile.inc and bandwidth/makefile.inc in your Makefile
 - link with COMMON_OBJS and BANDWIDTH_OBJS

Paths are modifiable, check Makefiles. For example program, remember to clone this
repository recursively with --recursive-submodules enabled for cloning.

## <sub>Example</sub>

Sample program for interface bandwidth monitoring, is provided. Clone recursively to build.
