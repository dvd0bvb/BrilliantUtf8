[![CMake on multiple platforms](https://github.com/dvd0bvb/BrilliantUtf8/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/dvd0bvb/BrilliantUtf8/actions/workflows/cmake-multi-platform.yml)

# BrilliantUtf8

A simple C++ library which decodes UTF8 bytes into code points.

Decode UTF8 encoded test in one line. Just provide storage for the decoded code units.
```
#include "BrilliantUtf8.hpp"

constexpr auto str = u8"¯\_(ツ)_/¯";
std::vector<std::uint32_t> output;
auto result = brilliant::utf8::decode(std::begin(str), std::end(str), std::back_inserter(output));
```
For thow the output type is u32.

The return value of `decode()` is a struct containing an enum describing an error and an iterator into the input range where the error occurred. In the case that there was no error, the error value is `DecodeError::OK` and the iterator is equal to `std::end(inputRange)`. This allows the caller to determine what went wrong with decoding, where it went wrong, and how to handle it. 
```
if (result.error != brilliant::utf8::DecodeError::OK)
{
  const auto size = brilliant::utf8::getCodePointSize(*result.resultIter);
  output.push_back('\ufffd'); //insert a replacement codepoint
  //continue decoding
  result = brilliant::utf8::decode(result.resultIter + 1, std::end(str), std::back_inserter(output);

  //...
}
```

Further improvement goals include:
- A CodePoint type to distinguish between code points and u32 types
  - std::format machinery for these
- Range based use of `decode()` eg: `decode(u8"some text", std::back_inserter(output))`
- Sized output range to allow `decode(bufferOfUtf8Bytes, aBufferWithLimitedCapacity, size)`
  - with specializations for `std::array` and C arrays since we know the size of them
- An equivalent to std::cout to allow printing to console
