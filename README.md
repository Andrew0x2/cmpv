# CMPV

CMPV is an application designed to monitor the status of MPV and integrate seamlessly with i3blocks. Written in C, it provides a simple and efficient way to display whether MPV is running directly on your i3 status bar.

---

## Features
- Real-time monitoring of MPV's status.
- Integration with [i3blocks](https://github.com/vivien/i3blocks) for displaying the status on the i3 bar.
- Lightweight and optimized for minimalist environments.

---

## Requirements
- **MPV**: Must be installed and configured to use IPC sockets.
- **i3blocks**: To integrate CMPV into the i3 status bar.
- **C Compiler**: Required to compile the source code.

---

## Installation

1. Clone the repository:
git clone git@github.com:Andrew0x2/cmpv.git
cd cmpv
text

2. Compile the application:
gcc -o mpv_monitor mpv_monitor.c -ljson-c
text

3. Configure i3blocks:
Edit your i3blocks configuration file (usually located at `~/.config/i3blocks/config`) and add a new entry:
[mpv]
command=/absolute/path/to/mpv_monitor
interval=5
text

4. Reload i3 configuration:
i3-msg reload
text

---

## Usage

Run the application directly from the terminal:
./mpv_monitor
text

Or let i3blocks execute it automatically.

### Output
- 🚫 - *Mpv is not executed*: MPV is not running.
- ✅ - *Mpv is running*: MPV is active.

---

## Screenshots

### CMPV Integrated with i3blocks
![CMPV Running](images/cmpv_running.png)

### CMPV with MPV Not Running
![CMPV Not Running](images/cmpv_not_running.png)

---

## Demo Video

Watch CMPV in action:

[![CMPV Demo](images/demo_thumbnail.png)](videos/cmpv_demo.mp4)

---

## Contributions

Contributions are welcome! Feel free to open issues or pull requests to improve CMPV.

---

## License

This project is licensed under the MIT License. See the `LICENSE` file for more details.
