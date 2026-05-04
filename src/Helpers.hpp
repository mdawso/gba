#pragma once

#include "Types.hpp"

inline HalfWord MakeHalfword(Byte low, Byte high) {
    return static_cast<HalfWord>((high << 8) | low);
}

inline Word MakeWord(HalfWord low, HalfWord high) {
    return static_cast<Word>((high << 16) | low);
}

inline void SplitHalfword(HalfWord input, Byte& low, Byte& high) {
    low  = static_cast<Byte>(input & 0xFF);
    high = static_cast<Byte>((input >> 8) & 0xFF);
}

inline void SplitWord(Word input, HalfWord& low, HalfWord& high) {
    low  = static_cast<HalfWord>(input & 0xFFFF);
    high = static_cast<HalfWord>((input >> 16) & 0xFFFF);
}