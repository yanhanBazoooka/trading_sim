#pragma once
#include "core/tick.hpp"
#include "core/order.hpp"
#include <optional>

class StrategyEngine {
public:
    virtual ~StrategyEngine() = default;
    virtual Order generate_order(const Tick& tick, int tick_index) = 0;
};