# sndkit

sndkit is a sonic toolkit for everyone. It is a collection
of DSP algorithms written in a literate style placed in the
public domain. When tangled, the algorithms produce C89
code. When woven, they turn into a self-contained wiki,
which can be found [here](https://pbat.ch/sndkit) and
[here](paulbatchelor.github.io/sndkit).

## I don't see any code here?

The C code is actually contained inside of the org files in
the repository. This is how the literate programming style
works! It exists in this intermediate markup format, where
it can either be *tangled* into code, or *woven* into HTML.

Code in sndkit is tangled using [worgle](https://pbat.ch/proj/worgle).
For convenience, a local copy of worgle is provided so you
don't need install this.

Typically, each algorithm will generate one C file and one
header file, such as "osc.c" and "osc.h". These are designed
self-contained. These shouldn't need to rely on any other
files or external libraries (well, other than the standard
C library), unless very explicitely stated.

To tangle all the code, run `make tangle`. This will compile
worgle, then run worgle on all the org documents.

## Building and Installing

sndkit for the most part aims to be fairly self-contained.
A standard C compiler with Make should be all that is
needed.

Running "make" without any arguments will build
libsndkit.a and the sndkit executable, which is an
interpreter for LIL, with added sndkit functions.

To install, run "sudo make install".

## Example Usage

Many sndkit algorithms already exist pre-tangled in
[Soundpipe](https://pbat.ch/proj/soundpipe.html). Have a
look at that for some real-world usage.

It is highly encouraged to read the
[woven output](https://pbat.ch/sndkit/algos), as they will
document proper usage.

## Weaving the Wiki

Wiki Weaving is done via [weewiki](https://pbat.ch/wiki/weewiki),
so that needs to be installed.

The weewiki database can be generated with `make db`.

From there, it can be locally run with `weewiki server`,
where it can be browsed from `localhost:8080`.

To export to HTML, run `make export`.

## Licensing

The text in this wiki is licensed under the
[Creative Commons Zero](https://creativecommons.org/share-your-work/public-domain/cc0/) license, otherwise
known as "CC0".

The tangled output is dual-licensed under MIT or
the [Unlicense](https://unlicense.org/).