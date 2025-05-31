#include "simulator_test.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

Order MockStrategy::generate_order(const Tick& tick, int tick_index) {
    if (!should_generate_order_) {
        return Order{-1, "", OrderType::MARKET, OrderSide::BUY, 0, 0.0};
    }
    
    return Order{
        tick_index,
        tick.timestamp,
        OrderType::MARKET,
        order_side_,
        order_size_,
        tick.price
    };
}

std::vector<Tick> SimulatorTest::create_test_ticks(int count) {
    std::vector<Tick> ticks;
    for (int i = 0; i < count; ++i) {
        ticks.push_back(Tick{
            "2025-05-28T09:30:" + std::to_string(i) + ".000Z",
            100.0 + i,  // price increases by 1 each tick
            10,         // constant size
            true       // all aggressive buys
        });
    }
    return ticks;
}

void SimulatorTest::assert_condition(bool condition, const std::string& message) {
    if (condition) {
        std::cout << "✓ " << message << "\n";
        tests_passed_++;
    } else {
        std::cout << "✗ " << message << "\n";
        tests_failed_++;
    }
}

void SimulatorTest::test_single_tick_processing() {
    std::cout << "\nTesting single tick processing...\n";
    
    auto strategy = std::make_unique<MockStrategy>();
    strategy->set_order_size(100);
    SimulatorEngine sim(std::move(strategy));
    
    auto ticks = create_test_ticks(1);
    sim.process_tick(ticks[0]);
    
    // Export results
    sim.export_results("test_single_tick_summary.json", "test_single_tick_trades.json");
    
    // Read and verify results
    std::ifstream trades_file("test_single_tick_trades.json");
    json trades_json = json::parse(trades_file);
    
    assert_condition(trades_json.size() == 1, "Single trade should be recorded");
    assert_condition(trades_json[0]["size"] == 100, "Trade size should match order size");
    assert_condition(trades_json[0]["price"] == 100.0, "Trade price should match tick price");
}

void SimulatorTest::test_no_orders_generated() {
    std::cout << "\nTesting no orders generated...\n";
    
    auto strategy = std::make_unique<MockStrategy>();
    strategy->set_should_generate_order(false);
    SimulatorEngine sim(std::move(strategy));
    
    auto ticks = create_test_ticks(5);
    for (const auto& tick : ticks) {
        sim.process_tick(tick);
    }
    
    sim.export_results("test_no_orders_summary.json", "test_no_orders_trades.json");
    
    std::ifstream trades_file("test_no_orders_trades.json");
    json trades_json = json::parse(trades_file);
    
    assert_condition(trades_json.size() == 0, "No trades should be recorded");
}

void SimulatorTest::test_multiple_ticks_processing() {
    std::cout << "\nTesting multiple ticks processing...\n";
    
    auto strategy = std::make_unique<MockStrategy>();
    SimulatorEngine sim(std::move(strategy));
    
    auto ticks = create_test_ticks(5);
    for (const auto& tick : ticks) {
        sim.process_tick(tick);
    }
    
    sim.export_results("test_multiple_summary.json", "test_multiple_trades.json");
    
    std::ifstream trades_file("test_multiple_trades.json");
    json trades_json = json::parse(trades_file);
    
    assert_condition(trades_json.size() == 5, "Should have 5 trades");
    assert_condition(trades_json[4]["price"] == 104.0, "Last trade price should be correct");
}

void SimulatorTest::test_pnl_calculation() {
    std::cout << "\nTesting PnL calculation...\n";
    
    auto strategy = std::make_unique<MockStrategy>();
    strategy->set_order_size(100);
    SimulatorEngine sim(std::move(strategy));
    
    // Create a sequence of alternating buys and sells
    auto ticks = create_test_ticks(2);
    
    strategy->set_order_side(OrderSide::BUY);
    sim.process_tick(ticks[0]); // Buy at 100
    
    strategy->set_order_side(OrderSide::SELL);
    sim.process_tick(ticks[1]); // Sell at 101
    
    sim.export_results("test_pnl_summary.json", "test_pnl_trades.json");
    
    std::ifstream summary_file("test_pnl_summary.json");
    json summary = json::parse(summary_file);
    
    // Should have made 100 * (101 - 100) = 100 in profit
    assert_condition(summary["realized_pnl"] == 100.0, "PnL calculation should be correct");
}

void SimulatorTest::test_order_execution() {
    std::cout << "\nTesting order execution...\n";
    
    auto strategy = std::make_unique<MockStrategy>();
    strategy->set_order_size(50);
    SimulatorEngine sim(std::move(strategy));
    
    auto ticks = create_test_ticks(1);
    sim.process_tick(ticks[0]);
    
    sim.export_results("test_execution_summary.json", "test_execution_trades.json");
    
    std::ifstream trades_file("test_execution_trades.json");
    json trades_json = json::parse(trades_file);
    
    assert_condition(trades_json[0]["size"] == 50, "Order size should be executed exactly");
    assert_condition(trades_json[0]["side"] == "BUY", "Order side should be correct");
}

void SimulatorTest::run_all_tests() {
    test_single_tick_processing();
    test_no_orders_generated();
    test_multiple_ticks_processing();
    test_pnl_calculation();
    test_order_execution();
    
    report_results();
}

void SimulatorTest::report_results() const {
    std::cout << "\nTest Results:\n"
              << "✓ Passed: " << tests_passed_ << "\n"
              << "✗ Failed: " << tests_failed_ << "\n"
              << "Total: " << (tests_passed_ + tests_failed_) << "\n";
} 