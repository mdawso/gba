#pragma once

#include "Types.hpp"

class IBus;

class PPU {
private:
    Word _todo;

public:

    PPU(IBus* bus);

    void Clock();
};