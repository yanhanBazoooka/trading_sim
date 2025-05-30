import requests
import csv
from datetime import datetime, timedelta
import time
import os

# ================== CONFIG ==================
API_KEY = "your_api_key"
API_SECRET = "your_secret_key"

HEADERS = {
    "APCA-API-KEY-ID": API_KEY,
    "APCA-API-SECRET-KEY": API_SECRET
}

SYMBOL = "AAPL"
DAY = "2025-05-28"                 # YYYY-MM-DD
MAX_TRADES = 5000   # for testing, stop early 
START_HOUR_ET = 9
START_MINUTE_ET = 30
END_HOUR_ET = 9
END_MINUTE_ET = 31
CHUNK_SIZE_MINUTES = 5
SAVE_DIR = "data"
# ============================================

def fetch_ticks(symbol, start_utc, end_utc):
    url = f"https://data.alpaca.markets/v2/stocks/{symbol}/trades"
    params = {
        "start": start_utc.isoformat() + "Z",
        "end": end_utc.isoformat() + "Z",
        "limit": 10000
    }

    all_trades = []
    page = 1

    while True:
        resp = requests.get(url, headers=HEADERS, params=params)
        if resp.status_code != 200:
            print(f"error {resp.status_code}: {resp.text}")
            break

        data = resp.json()
        trades = data.get("trades", [])
        all_trades.extend(trades)

        if trades:
            first_ts = trades[0]["t"]
            last_ts = trades[-1]["t"]
        else:
            first_ts = last_ts = "N/A"

        print(f"page {page:>2}: +{len(trades):>5} trades  |  first: {first_ts}  last: {last_ts}  |  total: {len(all_trades):>6}")

        page += 1

        if "next_page_token" not in data:
            break

        if MAX_TRADES and len(all_trades) >= MAX_TRADES:
            print(f"🛑 reached max trade cap: {MAX_TRADES}")
            break

    params["page_token"] = data["next_page_token"]
    page += 1
    time.sleep(0.25)


    return all_trades

def apply_tick_rule(trades):
    enriched = []
    last_price = None
    last_side = "B"

    for t in trades:
        ts, price, size = t["t"], t["p"], t["s"]

        if last_price is None:
            side = last_side
        elif price > last_price:
            side = "B"
        elif price < last_price:
            side = "S"
        else:
            side = last_side

        enriched.append((ts, price, size, side))
        last_price = price
        last_side = side

    return enriched

def save_to_csv(trades, filename):
    os.makedirs(SAVE_DIR, exist_ok=True)
    with open(os.path.join(SAVE_DIR, filename), "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(["timestamp", "price", "size", "side"])
        for t in trades:
            writer.writerow(t)

def run():
    date_parts = [int(p) for p in DAY.split("-")]
    base_time = datetime(*date_parts, 0, 0)

    start_min = START_HOUR_ET * 60 + START_MINUTE_ET
    end_min = END_HOUR_ET * 60 + END_MINUTE_ET

    for offset in range(start_min, end_min, CHUNK_SIZE_MINUTES):
        start_utc = base_time + timedelta(minutes=offset)
        end_utc = start_utc + timedelta(minutes=CHUNK_SIZE_MINUTES)

        print(f"\nfetching {SYMBOL} from {start_utc.isoformat()} to {end_utc.isoformat()}")
        raw_trades = fetch_ticks(SYMBOL, start_utc, end_utc)
        enriched = apply_tick_rule(raw_trades)

        suffix = f"{SYMBOL.lower()}_{start_utc.strftime('%H%M')}_{end_utc.strftime('%H%M')}.csv"
        save_to_csv(enriched, suffix)
        print(f"✅ saved {len(enriched)} trades to {suffix}\n")

if __name__ == "__main__":
    run()