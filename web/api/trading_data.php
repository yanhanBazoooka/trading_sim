<?php

class TradingData {
    private $outputDir;
    
    public function __construct() {
        // Look in the output directory where the C++ simulator writes its files
        $this->outputDir = dirname(dirname(__DIR__)) . '/output/';
    }
    
    /**
     * Calculate trading statistics from a list of trades
     */
    private function calculateStats($trades) {
        if (empty($trades)) {
            return [
                'totalPnL' => 0.00,
                'winRate' => 0.0,
                'sharpeRatio' => 0.00
            ];
        }

        // Calculate total PnL and winning trades
        $totalPnL = 0;
        $winningTrades = 0;
        $pnlArray = [];
        
        foreach ($trades as $trade) {
            $pnl = $trade['pnl'];
            $totalPnL += $pnl;
            if ($pnl > 0) $winningTrades++;
            $pnlArray[] = $pnl;
        }
        
        // Calculate win rate
        $winRate = (count($trades) > 0) ? ($winningTrades / count($trades)) * 100 : 0;
        
        // Calculate Sharpe Ratio
        // Using daily scale factor of sqrt(252) for annualization
        $mean = array_sum($pnlArray) / count($pnlArray);
        $variance = 0;
        foreach ($pnlArray as $pnl) {
            $variance += pow($pnl - $mean, 2);
        }
        $stdDev = sqrt($variance / count($pnlArray));
        $sharpeRatio = $stdDev != 0 ? ($mean / $stdDev) * sqrt(252) : 0;
        
        return [
            'totalPnL' => $totalPnL,
            'winRate' => $winRate,
            'sharpeRatio' => $sharpeRatio
        ];
    }
    
    /**
     * Get the latest trading results
     * @return array Trading statistics including PnL, win rate, etc.
     */
    public function getLatestResults() {
        $tradesFile = $this->outputDir . 'trades.json';
        if (!file_exists($tradesFile)) {
            error_log("Trades file not found: " . $tradesFile);
            return [
                'totalPnL' => 0.00,
                'winRate' => 0.0,
                'sharpeRatio' => 0.00
            ];
        }
        
        $trades = json_decode(file_get_contents($tradesFile), true);
        if (json_last_error() !== JSON_ERROR_NONE) {
            error_log("JSON decode error: " . json_last_error_msg());
            return [
                'totalPnL' => 0.00,
                'winRate' => 0.0,
                'sharpeRatio' => 0.00
            ];
        }
        
        // Convert C++ trade format if needed
        $trades = array_map(function($trade) {
            return [
                'timestamp' => $trade['timestamp'],
                'symbol' => 'AAPL',
                'side' => $trade['side'],
                'quantity' => $trade['size'],
                'price' => $trade['price'],
                'pnl' => $trade['pnl']
            ];
        }, $trades);
        
        return $this->calculateStats($trades);
    }
    
    /**
     * Get all trades for a specific date range
     * @param string $startDate Start date in YYYY-MM-DD format
     * @param string $endDate End date in YYYY-MM-DD format
     * @return array List of trades
     */
    public function getTrades($startDate = null, $endDate = null) {
        $tradesFile = $this->outputDir . 'trades.json';
        error_log("Looking for trades file at: " . $tradesFile);
        
        if (!file_exists($tradesFile)) {
            error_log("Trades file not found!");
            return [];
        }
        
        $trades = json_decode(file_get_contents($tradesFile), true);
        if (json_last_error() !== JSON_ERROR_NONE) {
            error_log("JSON decode error: " . json_last_error_msg());
            return [];
        }
        
        error_log("Found " . count($trades) . " total trades");
        error_log("Date range: " . $startDate . " to " . $endDate);
        
        // Convert C++ trade format to web format
        $trades = array_map(function($trade) {
            return [
                'timestamp' => $trade['timestamp'],
                'symbol' => 'AAPL', // Hardcoded since we're only trading AAPL
                'side' => $trade['side'],
                'quantity' => $trade['size'],
                'price' => $trade['price'],
                'pnl' => $trade['pnl']
            ];
        }, $trades);
        
        if ($startDate && $endDate) {
            $startTimestamp = strtotime($startDate . ' 00:00:00');
            $endTimestamp = strtotime($endDate . ' 23:59:59');
            
            $trades = array_filter($trades, function($trade) use ($startTimestamp, $endTimestamp) {
                $tradeTimestamp = strtotime($trade['timestamp']);
                $result = $tradeTimestamp >= $startTimestamp && $tradeTimestamp <= $endTimestamp;
                if (!$result) {
                    error_log("Filtered out trade from " . $trade['timestamp']);
                }
                return $result;
            });
            
            error_log("After filtering: " . count($trades) . " trades");
        }
        
        return array_values($trades);
    }
    
    /**
     * Update strategy configuration
     * @param array $config New configuration parameters
     * @return bool Success status
     */
    public function updateConfig($config) {
        $configFile = $this->outputDir . 'strategy_config.json';
        return file_put_contents($configFile, json_encode($config, JSON_PRETTY_PRINT)) !== false;
    }
}

// Only handle API requests if this file is accessed directly
if (basename($_SERVER['SCRIPT_NAME']) === basename(__FILE__)) {
    header('Content-Type: application/json');
    $action = $_GET['action'] ?? '';
    $tradingData = new TradingData();
    
    switch ($action) {
        case 'latest_results':
            echo json_encode($tradingData->getLatestResults());
            break;
        case 'trades':
            $startDate = $_GET['start_date'] ?? null;
            $endDate = $_GET['end_date'] ?? null;
            echo json_encode($tradingData->getTrades($startDate, $endDate));
            break;
        case 'update_config':
            $config = json_decode(file_get_contents('php://input'), true);
            echo json_encode(['success' => $tradingData->updateConfig($config)]);
            break;
        default:
            http_response_code(400);
            echo json_encode(['error' => 'Invalid action']);
    }
    exit;
} 