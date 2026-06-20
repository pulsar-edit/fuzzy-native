# fuzzy-native

[![Build Status](https://travis-ci.org/hansonw/fuzzy-native.svg?branch=master)](https://travis-ci.org/hansonw/fuzzy-native)

Fuzzy string matching library package for Node. Implemented natively in C++ for speed with support for multithreading.

The scoring algorithm is heavily tuned for file paths, but should work for general strings - it also supports the same algorithm that was implemented in Fuzzaldrin library, the one that powers command-pallette and other fuzzy-finders in Pulsar

## API

Read `lib/main.d.ts` for the API of the `Matcher` class.

See also the [spec](spec/fuzzy-native-spec.js) for basic usage.

## Scoring algorithm

### Default
The _default scoring_ algorithm is mostly borrowed from @wincent's excellent [command-t](https://github.com/wincent/command-t) vim plugin; most of the code is from [his implementation in  match.c](https://github.com/wincent/command-t/blob/master/ruby/command-t/match.c).

Read [the source code](src/score_match.cpp) for a quick overview of how it works (the function `recursive_match`).

NB: [score_match.cpp](src/score_match.cpp) and [score_match.h](src/score_match.h) have no dependencies besides the C/C++ stdlib and can easily be reused for other purposes.

There are a few notable additional optimizations:

- Before running the recursive matcher, we first do a backwards scan through the haystack to see if the needle exists at all. At the same time, we compute the right-most match for each character in the needle to prune the search space.
- For each candidate string, we pre-compute and store a bitmask of its letters in `MatcherBase`. We then compare this the "letter bitmask" of the query to quickly prune out non-matches.

### Fuzzaldrin

Ported from Atom's Fuzzaldrin app - it's easier to read the original version than to read the C++ one, they are basically identic: see [scorer.coffee](https://github.com/atom/fuzzaldrin/blob/master/src/scorer.coffee) from Atom's archived repository.
