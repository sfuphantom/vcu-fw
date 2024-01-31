# VCU Testing Framework Setup Guide

## Software Setup: 

 **Requirements**

1. **Python 3.7 or later**
2. **Git**
3. **Physical VCU**
4. **Wi-Fi (Recommended)**

**Setup Instructions:**

1. **Clone the Repository:**
   - Use `git clone` to clone the VCU Framework repository:
     ```
     git clone https://github.com/sfuphantom/vcu-fw
     ```

2. **Navigate to the Repository Directory:**
   - Change to the repository directory.

3. **Install Required Python Packages:**
   - Run `pip install -r requirements.txt`.

4. **Drive Authentication**
  - Download JSON off of Team Phantom Drive. *DO NOT UPLOAD TO GITHUB*
  - Place JSON into command-line\configs\


## Hardware Setup

To use the script effectively, it's crucial to establish a connection to the Vehicle Control Unit (VCU) via UART communication. This connection is established by plugging a USB cable from your computer's USB port to the VCU.

![VCU Image](vcu_image.jpg)

Before proceeding, you'll need to identify the specific USB port your VCU is connected to. The process of finding this port may vary depending on your operating system, whether you're using Windows or Linux.

**For Windows:**

1. Click the "Start" button on your Windows computer.
2. In the search bar, type "Device Manager" and press Enter.
3. Scroll down in the Device Manager window to locate the "USB ports" section.
4. Within the USB ports section, you'll find a list of connected devices. Identify the USB port to which your VCU is connected, and make a note of the port name, e.g., "PORT3."

**For Linux:**

1. Open a terminal window.
2. Use the following command to list all the available serial ports:
   ```
   ls /dev/ttyUSB*
   ```
   This command will display a list of connected USB devices, including your VCU. Note the name of the USB port, e.g., "/dev/ttyUSB0" or similar.

**Configuring the Port:**

Once you've identified the USB port on either Windows or Linux, you can configure it in the repository. Here's how to do it:

1. Open the repository where the script and related files are located.

2. Locate the "command-line/configs/device_config.json" file within the repository.

3. In the "device_config.json" file, under the "VCU" section, you'll find a parameter that specifies the port. It should look something like this:
   ```
   "VCU": {
       "port": "COM3"  // Replace "COM3" with the port name you identified earlier
   ```

4. Update the "port" value to match the USB port you identified. For example, if you found the port to be "/dev/ttyUSB0" on Linux or "COM3" on Windows, modify the "port" entry accordingly.

**Loading VCU HAL:**

@Raf