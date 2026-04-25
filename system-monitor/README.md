# Linux System Resource Monitor

## Project Description and Core Concepts

The Linux System Resource Monitor is a lightweight, real-time observability tool designed specifically for Linux environments. Unlike traditional monitoring agents that rely on high-level APIs, this project interacts directly with the kernel via the `/proc` virtual filesystem. Key concepts include:

- **Virtual Filesystems:** Understanding how the Linux kernel exposes hardware and process state through `/proc/stat`, `/proc/meminfo`, and `/proc/loadavg`.
- **Kernel State Extraction:** Calculating metrics like CPU usage by measuring deltas in jiffies (kernel time units) over a specific interval.
- **Data Serialization:** Transforming unstructured system data into structured JSON for modern web consumption.
- **Client-Server Separation:** Decoupling data collection (Bash) from visualization (HTML/JS) for modularity.

## Features

- **Direct Kernel Polling:** Reads raw data directly from the OS without heavy external dependencies.
- **Real-time Visualization:** Modern, dark-themed dashboard using `Chart.js` to graph CPU, memory, and disk trends.
- **Automated Logging:** Maintains a persistent `history.log` in CSV format for long-term system analysis.
- **Asynchronous Updates:** Uses `fetch` and cache-busting to update the dashboard without page reloads.

## Compilation/Build Instructions

This project consists of a Bash collection script and a web-based dashboard, so no binary compilation is required. However, proper environment setup is necessary:

1. **Make the collector executable:**
   ```bash
   chmod +x collector.sh
   ```

2. **Verify Dependencies:** Ensure your Linux system has `awk`, `grep`, and `df` installed (standard on most distributions).

## Usage Examples

### 1. Manual Collection
Run the script once to generate a snapshot in `data.json`:
```bash
./collector.sh
```

### 2. Continuous Monitoring (Crontab)
To monitor in the background, add a `cron` job to run the collector every minute. Run `crontab -e` and add:
```bash
* * * * * /path/to/OS-Projects-main/system-monitor/collector.sh
```

### 3. Launching the Dashboard
Start a lightweight web server in the `system-monitor` directory to view the live dashboard:
```bash
python -m http.server 8000
```
Then, visit `http://localhost:8000/dashboard.html` in your browser.

---
*Part of the Operating Systems Projects collection.*
