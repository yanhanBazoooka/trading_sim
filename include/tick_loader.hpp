#pragma once
#include "tick.hpp"
#include <vector>
#include <string>

std::vector<Tick> load_ticks(const std::string& filepath);
