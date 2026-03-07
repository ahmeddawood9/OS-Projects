# Linux System Resource Monitor

![Dashboard Preview](assets/dashboard-preview.png)

A lightweight system monitoring tool built specifically for Linux environments. This project circumvents heavy monitoring agents by reading raw hardware states directly from the kernel and visualizing them through a modern, local web interface.

## 🧠 Core OS Concepts Demonstrated

- **Virtual Filesystems:** Reads directly from `/proc/stat`, `/proc/meminfo`, and `/proc/loadavg` to calculate hardware usage without relying on top-level abstractions.
- **Data Serialization:** Parses unstructured OS output using `awk` and `grep`, formatting it into strict JSON.
- **Asynchronous Polling:** The frontend fetches the `.json` state asynchronously, using cache-busting techniques to ensure real-time data updates without page reloads.

## 📂 Project Architecture

- `collector.sh`: The core engine. It calculates a 1-second CPU delta and extracts memory, load average, and root disk usage.
- `dashboard.html`: The UI layer. A responsive, dark-themed dashboard using `Chart.js` to graph CPU trends and dynamically color-code hardware strain.
- `data.json`: The volatile state file containing the latest hardware snapshot.
- `history.log`: A persistent CSV-formatted log tracking system metrics over time.

## 🚀 Quick Start

### 1. Collect the Data
Make the script executable and run it to generate the initial `data.json` and `history.log` files.
```bash
chmod +x collector.sh
./collector.sh
