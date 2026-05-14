# U6143_ssd1306

SSD1306 128×32 I²C OLED display driver for Raspberry Pi rack enclosures. Cycles through configurable system-stat screens: temperature, memory, disk, CPU frequency, network throughput, uptime, clock, Docker, Wi-Fi, hostname, and IP address.

---

## Prerequisites

Enable I²C on your Raspberry Pi:

```bash
sudo raspi-config
```

Choose **Interface Options → I2C → Enable**, then reboot.

---

## Install

```bash
curl -fsSL https://raw.githubusercontent.com/egrif/U6143_ssd1306/master/install.sh | sudo bash
```

The installer will:
- Install `git`, `gcc`, `make`, and `i2c-tools` if missing
- Clone the repo, compile, and install the binary to `/usr/local/bin/ssd1306-display`
- Write a default config to `/etc/ssd1306.conf` (only if one doesn't exist)
- Add your user to the `i2c` group
- Create, enable, and start a systemd service (`ssd1306-display`)

Re-running the same command updates the binary without touching your config.

---

## Configuration

Edit `/etc/ssd1306.conf` to enable screens and set preferences:

```bash
sudo nano /etc/ssd1306.conf
sudo systemctl restart ssd1306-display
```

### Options

| Key | Default | Description |
|-----|---------|-------------|
| `show_temperature` | `1` | Temperature + CPU load screen |
| `show_memory` | `1` | RAM free / total screen |
| `show_disk` | `1` | Disk free / total screen |
| `show_clock` | `0` | Date and time screen |
| `show_uptime` | `0` | System uptime and process count |
| `show_cpu_freq` | `0` | CPU clock speed and throttle status |
| `show_gpu_temp` | `0` | CPU and GPU temperatures side-by-side |
| `show_network` | `0` | Network RX/TX throughput |
| `show_wifi` | `0` | Wi-Fi signal strength and link quality |
| `show_docker` | `0` | Running and stopped Docker container count |
| `show_hostname` | `0` | Hostname screen |
| `show_ip` | `0` | IP addresses of configured interfaces |
| `temp_unit` | `fahrenheit` | `fahrenheit` or `celsius` |
| `load_display` | `percent` | `percent` (e.g. `15%`) or `cores` (e.g. `0.15`) |
| `network_interfaces` | `eth0,wlan0` | Comma-separated interfaces for IP and network screens |
| `screen_time` | `3` | Seconds to show each screen |
| `top_line` | `ip` | Top line of every screen: `ip`, `hostname`, `custom`, or `none` |
| `custom_text` | `UCTRONICS` | Text shown when `top_line=custom` |

See `C/ssd1306.conf.example` for the full annotated config.

---

## Managing the service

```bash
sudo systemctl status ssd1306-display      # Check status and recent output
sudo systemctl stop ssd1306-display        # Stop the display
sudo systemctl start ssd1306-display       # Start it again
sudo systemctl disable ssd1306-display     # Prevent autostart on boot
sudo journalctl -u ssd1306-display -f      # Follow live logs
```

---

## Legacy Python demo (0.91″ display without MCU)

For the older display variant without an MCU controller:

```bash
sudo apt-get install python3-pip python3-pil
sudo pip3 install adafruit-circuitpython-ssd1306
cd ~/U6143_ssd1306/python
sudo python3 ssd1306_stats.py
```
