<?php
require_once __DIR__ . '/../api/trading_data.php';

$tradingData = new TradingData();
$results = $tradingData->getLatestResults();

$totalPnL = $results['totalPnL'] ?? 0.00;
$winRate = $results['winRate'] ?? 0.0;
$sharpeRatio = $results['sharpeRatio'] ?? 0.00;

// Get trades for the chart
$trades = $tradingData->getTrades();
$chartData = [];
$cumulativePnL = 0;

foreach ($trades as $trade) {
    $cumulativePnL += $trade['pnl'];
    $chartData[] = [
        'time' => date('H:i:s', strtotime($trade['timestamp'])),
        'pnl' => $cumulativePnL
    ];
}
?>

<div class="row">
    <div class="col-md-4">
        <div class="card">
            <div class="card-body">
                <h5 class="card-title">Total P&L</h5>
                <p class="card-text display-4 <?php echo $totalPnL >= 0 ? 'positive' : 'negative'; ?>">
                    $<?php echo number_format($totalPnL, 2); ?>
                </p>
            </div>
        </div>
    </div>
    <div class="col-md-4">
        <div class="card">
            <div class="card-body">
                <h5 class="card-title">Win Rate</h5>
                <p class="card-text display-4"><?php echo number_format($winRate, 1); ?>%</p>
                <small class="text-muted">Based on <?php echo count($trades); ?> trades</small>
            </div>
        </div>
    </div>
    <div class="col-md-4">
        <div class="card">
            <div class="card-body">
                <h5 class="card-title">Sharpe Ratio</h5>
                <p class="card-text display-4"><?php echo number_format($sharpeRatio, 2); ?></p>
                <small class="text-muted">Annualized</small>
            </div>
        </div>
    </div>
</div>

<div class="row mt-4">
    <div class="col-12">
        <div class="card">
            <div class="card-body">
                <h5 class="card-title">Performance Chart</h5>
                <canvas id="performanceChart"></canvas>
            </div>
        </div>
    </div>
</div>

<script>
document.addEventListener('DOMContentLoaded', function() {
    const ctx = document.getElementById('performanceChart').getContext('2d');
    new Chart(ctx, {
        type: 'line',
        data: {
            labels: <?php echo json_encode(array_column($chartData, 'time')); ?>,
            datasets: [{
                label: 'Cumulative P&L',
                data: <?php echo json_encode(array_column($chartData, 'pnl')); ?>,
                borderColor: 'rgb(75, 192, 192)',
                tension: 0.1
            }]
        },
        options: {
            responsive: true,
            scales: {
                y: {
                    beginAtZero: true,
                    ticks: {
                        callback: function(value) {
                            return '$' + value.toFixed(2);
                        }
                    }
                }
            },
            plugins: {
                tooltip: {
                    callbacks: {
                        label: function(context) {
                            return 'P&L: $' + context.raw.toFixed(2);
                        }
                    }
                }
            }
        }
    });
});</script> 