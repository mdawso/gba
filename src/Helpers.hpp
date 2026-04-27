#pragma once

#include "Types.hpp"

Halfword MakeHalfword(Byte low, Byte high) {
    return static_cast<Halfword>((high << 8) | low);
}

Word MakeWord(Halfword low, Halfword high) {
    return static_cast<Word>((high << 16) | low);
}

void SplitHalfword(Halfword input, Byte& low, Byte& high) {
    low  = static_cast<Byte>(input & 0xFF);
    high = static_cast<Byte>((input >> 8) & 0xFF);
}

void SplitWord(Word input, Halfword& low, Halfword& high) {
    low  = static_cast<Halfword>(input & 0xFFFF);
    high = static_cast<Halfword>((input >> 16) & 0xFFFF);
}