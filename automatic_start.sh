#!/bin/bash

# Script to automatically configure the C display program to compile and run on boot via /etc/rc.local
# **Now dynamically detects the user's home directory**

RC_LOCAL="/etc/rc.local"             # Path to the rc.local file
START_MARKER="# --- Start UCTRONICS C Display ---" # Start marker to detect if already added
END_MARKER="# --- End UCTRONICS C Display ---"     # End marker to detect if already added
REPO_NAME="U6143_ssd1306"            # Name of the repository directory

echo "--- UCTRONICS C Display Automatic Startup Configuration Script ---"

# 1. Check if running as root
if [ "$EUID" -ne 0 ]; then
  echo "Error: This script must be run with sudo."
  echo "Please run as: sudo $0"
  exit 1
fi

# --- Dynamic Path Configuration ---
# 2. Determine the username and home directory of the user invoking sudo
if [ -z "$SUDO_USER" ]; then
  echo "Error: Cannot determine the original username (SUDO_USER environment variable not set)."
  echo "This usually happens when logged in directly as root instead of using 'sudo'."
  echo "Please ensure you are logged in as a regular user and run this script using 'sudo $0'."
  exit 1
fi

# Get the user's home directory using getent, which is more reliable than assuming /home/
USER_HOME=$(getent passwd "$SUDO_USER" | cut -d: -f6)

if [ -z "$USER_HOME" ] || [ ! -d "$USER_HOME" ]; then
   echo "Error: Could not find the home directory for user '$SUDO_USER'."
   # Attempting a fallback method (may be less reliable)
   if [ -d "/home/$SUDO_USER" ]; then
       echo "Warning: Failed to find home directory via getent, but '/home/$SUDO_USER' exists. Attempting to use this path."
       USER_HOME="/home/$SUDO_USER"
   else
       echo "Could not automatically determine the home directory for '$SUDO_USER'. Please check system configuration."
       exit 1
   fi
fi

echo "Detected user invoking sudo as '$SUDO_USER', with home directory: $USER_HOME"

# 3. Define paths based on the detected home directory
# Assuming the repository is cloned directly into the user's home directory
REPO_BASE_DIR="$USER_HOME/$REPO_NAME"
C_DIR="$REPO_BASE_DIR/C"          # Directory containing C source code and Makefile
MAKEFILE_PATH="$C_DIR/Makefile"   # Full path to the Makefile

echo "Assuming C program path: $C_DIR"
echo "Assuming Makefile path: $MAKEFILE_PATH"
# --- Dynamic Path Configuration End ---


# 4. Check if the C directory and Makefile exist
if [ ! -d "$C_DIR" ]; then
  echo "Error: C program directory not found: $C_DIR"
  echo "Please ensure the '$REPO_NAME' repository is cloned into your home directory '$USER_HOME'."
  exit 1
fi
if [ ! -f "$MAKEFILE_PATH" ]; then
  echo "Error: Makefile not found in directory: $C_DIR."
  echo "The compilation step may fail. Please ensure the Makefile exists."
  exit 1
fi

# 5. Check if rc.local file exists
if [ ! -f "$RC_LOCAL" ]; then
  echo "Error: $RC_LOCAL file not found. Cannot configure automatic startup using this method."
  exit 1
fi

# 6. Check if the startup marker already exists in rc.local
if grep -qF "$START_MARKER" "$RC_LOCAL"; then
  echo "Startup command block seems to already exist in $RC_LOCAL (found start marker)."
  echo "To avoid duplication, the script will not make any changes."
  echo "If you need to update the commands, please manually edit $RC_LOCAL and remove the block from '$START_MARKER' to '$END_MARKER'."
  exit 0
fi

# 7. Check if 'exit 0' line exists in rc.local
if ! grep -q "^[[:space:]]*exit[[:space:]]\+0" "$RC_LOCAL"; then
  # If 'exit 0' is not found, attempt to append to the end of the file, though this might not be standard rc.local behavior
  echo "Warning: Could not find a valid 'exit 0' line in $RC_LOCAL."
  echo "Will attempt to append the command block to the end of the file, but this may not be optimal."
  APPEND_MODE=true
else
  APPEND_MODE=false
fi

# 8. Prepare the command block for insertion or appending
# Using the dynamically determined C_DIR path
# Note: sudo removed from original commands as rc.local usually runs as root
COMMAND_BLOCK=$(printf "%s\ncd %s\n%s\n%s\n%s\n%s" \
    "$START_MARKER" \
    "$C_DIR" \
    "make clean" \
    "make" \
    "./display &" \
    "$END_MARKER")

# 9. Insert or append the command block
if [ "$APPEND_MODE" = true ]; then
    echo "Appending startup command block to the end of $RC_LOCAL..."
    # Add a newline before appending, in case the file doesn't end with one
    echo "" >> "$RC_LOCAL"
    echo "$COMMAND_BLOCK" >> "$RC_LOCAL"
    if [ $? -ne 0 ]; then
        echo "Error: Failed to append command block to the end of $RC_LOCAL."
        exit 1
    fi
else
    echo "Inserting startup command block before 'exit 0' in $RC_LOCAL..."
    # Prepare command block for sed: escape special chars (&, /) and handle newlines
    SED_COMMAND_BLOCK=$(echo "$COMMAND_BLOCK" | sed -e 's/[\/&]/\\&/g' -e 's/$/\\/' -e '$s/\\$//')

    # Use sed to insert the command block before the line matching 'exit 0'
    if ! sed -i "/^[[:space:]]*exit[[:space:]]\+0/i $SED_COMMAND_BLOCK" "$RC_LOCAL"; then
        echo "Error: Failed to insert command block into $RC_LOCAL using sed."
        echo "Please check the $RC_LOCAL file manually."
        exit 1
    fi
fi

# 10. Verify the operation succeeded (check for start marker)
# Add a short delay to help ensure filesystem changes are written
sleep 1
if grep -qF "$START_MARKER" "$RC_LOCAL"; then
  echo "Successfully added the startup command block to $RC_LOCAL."
  echo "Note: The system will run 'make clean' and 'make' in '$C_DIR' on every boot."
  echo "-----------------------------------------------------"
  echo "IMPORTANT: Please reboot your device for the changes to take effect."
  echo "You can reboot now by running: sudo reboot"
  echo "-----------------------------------------------------"
  exit 0
else
  echo "Error: After attempting to add, failed to verify that the command block was added to $RC_LOCAL."
  echo "Please check the $RC_LOCAL file manually."
  exit 1
fi
