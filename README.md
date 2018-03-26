
AutoGRP
==========

AutoGoReviewPartner: A tool to simplify and automate the analysis of go
(weiqi/baduk) games by wrapping [goreviewpartner](https://github.com/pnprog/goreviewpartner)'s
excellent analysis tools.

The tool currently supports automatically downloading games from
[online-go.com]() and analyzing them with [Leela](https://www.sjeng.org/leela.html)
and [Leela Zero](https://github.com/gcp/leela-zero). Support for AQ, Ray, and
GNU Go can be added easily.

Getting Started
---------------
Once AutoGRP is built or downloaded, it needs to be configured. It looks for
`config.ini` and `client_config.ini` in the same directory as the executable,
example files with the necessary documentation are in the project root.

### Single-Computer Use

Just run the `autogrp` executable.

If you follow the advice in the example
config and set `run_local_worker` to false for the first run, remember to
change it back. If you want to analyze a game that has already been downloaded,
simply delete the sgf in `games_dir` and run `autogrp` again.

I know that's a bit clunky and plan to add a real UI to this in the future,
but it works for now.

### Distributed Setup

Run the `autogrp` executable on a central server. If the server is not capable
of (or simply should not be) running game analysis, set `run_local_worker` to
false in `config.ini`.

On each of the client machines, you need the `client` executable,
`client-config.ini`, all of the available bots for analysis (the ones set in
`config.ini` on the server), and goreviewparter. Once all of that is done,
just run `client`.


Building
========

### Dependencies
* [Curl](https://curl.haxx.se/libcurl/) (MIT derivative)
* [Boost](http://www.boost.org/) **1.66.0** (Boost)
* [{fmt}](https://github.com/fmtlib/fmt) (BSD 2-Clause)
* [http-parser](https://github.com/nodejs/http-parser) (MIT)
* [rapidjson](https://github.com/Tencent/rapidjson) (MIT)
* [restinio](https://github.com/Stiffstream/restinio) (BSD 3-Clause)
* [inih](https://github.com/benhoyt/inih) (BSD 3-Clause)
* [CMake](https://cmake.org/) 1.9+
* C++17-compliant compiler. Only gcc tested.

Curl and Boost 1.66.0 must be installed before building. All other dependencies
are included in the project tree.

Once the dependencies are in place:

### Linux (macOS?)

```
mkdir build
cd build
cmake .. && make -j4
```

Or whatever your preferred invocation method for CMake and whichever build tool
files it generates.

I don't have an Apple system to test on, but it should work just fine.

### Windows

There is a binary available on the GitHub Releases section.

AutoGRP can be built on Windows, but it is quite laborious. To get the compiler
and Boost, I recommend Stephan Lavavej's [MinGW Distro](https://nuwen.net/mingw.html).

To get Curl (and openSSL), I recommend [these builds](https://github.com/vszakats/curl-for-win).

Change src/CMakeLists.txt link_directories line to point to the folder
containing libcurl.a/libcurl.dll.a.

After that, the build might work. If it does, put the necessary DLLs in a
folder with the executable and cross your fingers.

##### Why is the binary so big?

Boost is statically linked.