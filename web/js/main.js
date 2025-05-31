// Fetch latest trading results
async function fetchLatestResults() {
    try {
        const response = await fetch('api/trading_data.php?action=latest_results');
        const data = await response.json();
        if (data.error) {
            console.error('Error fetching results:', data.error);
            return;
        }
        updateDashboard(data);
    } catch (error) {
        console.error('Failed to fetch results:', error);
    }
}

// Update dashboard with new data
function updateDashboard(data) {
    if (data.totalPnL !== undefined) {
        const pnlElement = document.querySelector('.card-text.display-4');
        if (pnlElement) {
            const formattedPnL = Number(data.totalPnL).toLocaleString(undefined, {
                minimumFractionDigits: 2,
                maximumFractionDigits: 2
            });
            pnlElement.textContent = `$${formattedPnL}`;
            pnlElement.className = `card-text display-4 ${data.totalPnL >= 0 ? 'positive' : 'negative'}`;
        }
    }
    
    if (data.winRate !== undefined) {
        const elements = document.querySelectorAll('.card-text.display-4');
        const winRateElement = Array.from(elements).find(el => el.textContent.includes('%'));
        if (winRateElement) {
            winRateElement.textContent = `${Number(data.winRate).toLocaleString(undefined, {
                minimumFractionDigits: 1,
                maximumFractionDigits: 1
            })}%`;
        }
    }
    
    if (data.sharpeRatio !== undefined) {
        const elements = document.querySelectorAll('.card-text.display-4');
        const sharpeElement = Array.from(elements).find(el => !el.textContent.includes('$') && !el.textContent.includes('%'));
        if (sharpeElement) {
            sharpeElement.textContent = Number(data.sharpeRatio).toLocaleString(undefined, {
                minimumFractionDigits: 2,
                maximumFractionDigits: 2
            });
        }
    }
}

// Initial load
document.addEventListener('DOMContentLoaded', fetchLatestResults);

// Auto-refresh dashboard every 30 seconds
setInterval(fetchLatestResults, 30000); 