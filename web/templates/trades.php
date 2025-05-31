<?php
require_once __DIR__ . '/../api/trading_data.php';

$tradingData = new TradingData();

// Get date range from query parameters, default to VWAP strategy dates
$defaultStart = '2025-05-28';  // Match the dates in main.cpp
$defaultEnd = '2025-05-28';    // Same day since we're only trading 5 minutes

$startDate = $_GET['start_date'] ?? $defaultStart;
$endDate = $_GET['end_date'] ?? $defaultEnd;

// For debugging
$debug = true;  // Set to false in production
if ($debug) {
    ini_set('display_errors', 1);
    ini_set('display_startup_errors', 1);
    error_reporting(E_ALL);
}

$trades = $tradingData->getTrades($startDate, $endDate);

// Calculate summary statistics
$totalPnL = 0;
$winningTrades = 0;
foreach ($trades as $trade) {
    $totalPnL += $trade['pnl'];
    if ($trade['pnl'] > 0) $winningTrades++;
}
$winRate = count($trades) > 0 ? ($winningTrades / count($trades)) * 100 : 0;
?>

<div class="card">
    <div class="card-body">
        <h5 class="card-title">Trade History</h5>
        
        <?php if ($debug): ?>
        <div class="alert alert-info">
            <strong>Debug Info:</strong><br>
            Start Date: <?php echo $startDate; ?><br>
            End Date: <?php echo $endDate; ?><br>
            Trades Found: <?php echo count($trades); ?><br>
            Current Directory: <?php echo __DIR__; ?><br>
            Data Directory: <?php echo dirname(dirname(__DIR__)) . '/output/'; ?><br>
            Total P&L: $<?php echo number_format($totalPnL, 2); ?><br>
            Win Rate: <?php echo number_format($winRate, 1); ?>%
        </div>
        <?php endif; ?>
        
        <!-- Date Filter Form -->
        <form id="dateFilterForm" class="row g-3 mb-4">
            <div class="col-md-3">
                <label for="start_date" class="form-label">Start Date</label>
                <input type="date" class="form-control" id="start_date" name="start_date" 
                       value="<?php echo $startDate; ?>">
            </div>
            <div class="col-md-3">
                <label for="end_date" class="form-label">End Date</label>
                <input type="date" class="form-control" id="end_date" name="end_date" 
                       value="<?php echo $endDate; ?>">
            </div>
            <div class="col-md-6 d-flex align-items-end gap-2">
                <button type="submit" class="btn btn-primary">Filter</button>
                <button type="button" class="btn btn-secondary" id="resetDates">Reset to Strategy Date</button>
            </div>
        </form>

        <!-- Summary Stats -->
        <div class="row mb-4">
            <div class="col-md-4">
                <div class="card bg-light">
                    <div class="card-body">
                        <h6 class="card-subtitle mb-2 text-muted">Total P&L</h6>
                        <p class="card-text h4 <?php echo $totalPnL >= 0 ? 'positive' : 'negative'; ?>">
                            $<?php echo number_format($totalPnL, 2); ?>
                        </p>
                    </div>
                </div>
            </div>
            <div class="col-md-4">
                <div class="card bg-light">
                    <div class="card-body">
                        <h6 class="card-subtitle mb-2 text-muted">Number of Trades</h6>
                        <p class="card-text h4"><?php echo count($trades); ?></p>
                    </div>
                </div>
            </div>
            <div class="col-md-4">
                <div class="card bg-light">
                    <div class="card-body">
                        <h6 class="card-subtitle mb-2 text-muted">Win Rate</h6>
                        <p class="card-text h4"><?php echo number_format($winRate, 1); ?>%</p>
                    </div>
                </div>
            </div>
        </div>

        <!-- Trades Table -->
        <div class="table-responsive">
            <table class="table table-striped table-hover">
                <thead>
                    <tr>
                        <th>Time</th>
                        <th>Symbol</th>
                        <th>Side</th>
                        <th>Quantity</th>
                        <th>Price</th>
                        <th>P&L</th>
                    </tr>
                </thead>
                <tbody>
                    <?php if (empty($trades)): ?>
                    <tr>
                        <td colspan="6" class="text-center">
                            No trades found for the selected period<br>
                            <?php if ($debug): ?>
                            <small class="text-muted">
                                Make sure you've run the C++ simulator and check the output/ directory
                            </small>
                            <?php endif; ?>
                        </td>
                    </tr>
                    <?php else: ?>
                        <?php foreach ($trades as $trade): ?>
                        <tr>
                            <td><?php echo date('H:i:s', strtotime($trade['timestamp'])); ?></td>
                            <td><?php echo htmlspecialchars($trade['symbol'] ?? 'N/A'); ?></td>
                            <td><?php echo htmlspecialchars($trade['side'] ?? 'N/A'); ?></td>
                            <td><?php echo number_format($trade['quantity'] ?? 0); ?></td>
                            <td>$<?php echo number_format($trade['price'] ?? 0, 2); ?></td>
                            <td class="<?php echo ($trade['pnl'] ?? 0) >= 0 ? 'positive' : 'negative'; ?>">
                                $<?php echo number_format($trade['pnl'] ?? 0, 2); ?>
                            </td>
                        </tr>
                        <?php endforeach; ?>
                    <?php endif; ?>
                </tbody>
            </table>
        </div>
    </div>
</div>

<script>
document.addEventListener('DOMContentLoaded', function() {
    // Initialize date inputs with current values
    const startDateInput = document.getElementById('start_date');
    const endDateInput = document.getElementById('end_date');
    
    // Add form submit handler
    document.getElementById('dateFilterForm').addEventListener('submit', function(e) {
        e.preventDefault();
        const startDate = startDateInput.value;
        const endDate = endDateInput.value;
        
        // Redirect with new date parameters
        window.location.href = `index.php?page=trades&start_date=${startDate}&end_date=${endDate}`;
    });

    // Add reset button handler
    document.getElementById('resetDates').addEventListener('click', function() {
        window.location.href = 'index.php?page=trades&start_date=<?php echo $defaultStart; ?>&end_date=<?php echo $defaultEnd; ?>';
    });
});</script> 