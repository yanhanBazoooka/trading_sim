#pragma once

#include "core/simulator.hpp"
#include "strategy/strategy_engine.hpp"
#include <vector>
#include <memory>

// Mock strategy for testing
class MockStrategy : public StrategyEngine {
public:
    Order generate_order(const Tick& tick, int tick_index) override;
    void set_should_generate_order(bool should_generate) { should_generate_order_ = should_generate; }
    void set_order_size(int size) { order_size_ = size; }
    void set_order_side(OrderSide side) { order_side_ = side; }

private:
    bool should_generate_order_ = true;
    int order_size_ = 100;
    OrderSide order_side_ = OrderSide::BUY;
};

class SimulatorTest {
public:
    void run_all_tests();

private:
    // Test helper methods
    std::vector<Tick> create_test_ticks(int count);
    void assert_condition(bool condition, const std::string& message);
    
    // Individual test cases
    void test_single_tick_processing();
    void test_no_orders_generated();
    void test_multiple_ticks_processing();
    void test_pnl_calculation();
    void test_order_execution();
    
    // Test result tracking
    int tests_passed_ = 0;
    int tests_failed_ = 0;
    void report_results() const;
}; 