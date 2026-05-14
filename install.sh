#!/usr/bin/env bash
# SSD1306 display driver — install or update
#
# Usage (one-liner):
#   curl -fsSL https://raw.githubusercontent.com/egrif/U6143_ssd1306/master/install.sh | sudo bash
#
# Re-running installs the latest binary without touching an existing config.

set -euo pipefail

REPO_URL="https://github.com/egrif/U6143_ssd1306.git"
BINARY_NAME="ssd1306-display"
BINARY_DEST="/usr/local/bin/$BINARY_NAME"
CONFIG_DEST="/etc/ssd1306.conf"
SERVICE_NAME="$BINARY_NAME"
SERVICE_FILE="/etc/systemd/system/${SERVICE_NAME}.service"

# --------------------------------------------------------------------------- #

echo "=== SSD1306 Display Driver Installer ==="
echo ""

# Must run as root
if [ "$EUID" -ne 0 ]; then
    echo "ERROR: run this script with sudo."
    echo "  curl -fsSL https://raw.githubusercontent.com/egrif/U6143_ssd1306/master/install.sh | sudo bash"
    exit 1
fi

# Determine the real (non-root) user who invoked sudo
SERVICE_USER="${SUDO_USER:-}"
if [ -z "$SERVICE_USER" ]; then
    echo "WARNING: could not detect a non-root user; service will run as root."
    SERVICE_USER="root"
fi
echo "Service will run as: $SERVICE_USER"

# --------------------------------------------------------------------------- #
# 1. Dependencies

echo ""
echo "-- Installing build dependencies --"
apt-get update -qq
apt-get install -y -qq git gcc make i2c-tools

# --------------------------------------------------------------------------- #
# 2. Clone and build in a temp directory

BUILD_DIR="$(mktemp -d)"
trap 'rm -rf "$BUILD_DIR"' EXIT

echo ""
echo "-- Cloning repository --"
git clone --depth=1 "$REPO_URL" "$BUILD_DIR/src"

echo ""
echo "-- Building --"
make -C "$BUILD_DIR/src/C"

# --------------------------------------------------------------------------- #
# 3. Install binary

echo ""
echo "-- Installing binary to $BINARY_DEST --"
install -m 755 "$BUILD_DIR/src/C/display" "$BINARY_DEST"

# --------------------------------------------------------------------------- #
# 4. Install default config (never overwrite an existing one)

if [ ! -f "$CONFIG_DEST" ]; then
    echo "-- Installing default config to $CONFIG_DEST --"
    install -m 644 "$BUILD_DIR/src/C/ssd1306.conf.example" "$CONFIG_DEST"
else
    echo "-- Config already exists at $CONFIG_DEST — leaving it unchanged --"
fi

# --------------------------------------------------------------------------- #
# 5. Add user to i2c group so service can access /dev/i2c-* without root

if [ "$SERVICE_USER" != "root" ]; then
    if ! groups "$SERVICE_USER" 2>/dev/null | grep -qw i2c; then
        echo "-- Adding $SERVICE_USER to the i2c group --"
        usermod -aG i2c "$SERVICE_USER"
    else
        echo "-- $SERVICE_USER is already in the i2c group --"
    fi
fi

# --------------------------------------------------------------------------- #
# 6. Create systemd service

echo ""
echo "-- Writing service file $SERVICE_FILE --"
cat > "$SERVICE_FILE" <<EOF
[Unit]
Description=SSD1306 OLED Display Service
After=multi-user.target

[Service]
User=$SERVICE_USER
ExecStart=$BINARY_DEST
Restart=on-failure
RestartSec=5s

[Install]
WantedBy=multi-user.target
EOF

# --------------------------------------------------------------------------- #
# 7. Enable and start

systemctl daemon-reload
systemctl enable "${SERVICE_NAME}.service"
systemctl restart "${SERVICE_NAME}.service"

# --------------------------------------------------------------------------- #

echo ""
echo "=== Install complete ==="
echo ""
echo "  Binary:  $BINARY_DEST"
echo "  Config:  $CONFIG_DEST"
echo "  Service: $SERVICE_NAME (enabled, running)"
echo ""
echo "Edit $CONFIG_DEST to enable/disable screens, then:"
echo "  sudo systemctl restart $SERVICE_NAME"
echo ""
echo "Other useful commands:"
echo "  sudo systemctl status  $SERVICE_NAME"
echo "  sudo journalctl -u $SERVICE_NAME -f    # follow logs"
echo "  sudo systemctl disable $SERVICE_NAME   # stop autostart"
