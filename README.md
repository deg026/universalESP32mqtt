# universalESP32mqtt
<u><b>Universal ESP32 to MQTT client</b></u>

(<i>this project is being developed using Clion IDE + PlatformIO</i>)

it is maximum flexible and detailed customizable client for managing DIY IoT devices based on ESP 32 platforms
easy to configure and install!

<b>PCB Project</b><br>
in additional for this project was developed prototype board and it is available now in the public EasyEDA access:<br>
<a href='https://easyeda.com/deg026/esp32-120-85'>https://easyeda.com/deg026/esp32-120-85</a><br>
You can easily order its production through jlcpcb.com, just for about 5$

<b>MAIN FEATURES:</b>
 - stable work via WiFi connection
 - start configuration via WiFi AP and web browser
 - auto-switching to AP (access point) mode in the absence of unavailable requested ssid (repeating search every 5 minutes and automatic re-connect if it appears)
 - immediately ready to use, does not require any changes in the program code to start, everything is configured directly through the web interface
 - control and configuration via simple web interface (including relay switching commands, and read current sensors values)
 - access control by authorization using configurable user / password combinations (basic HTTP AUTH)
 - synchronization of all sensors values data with the MQTT server (for relays - in both way)
 - automatically detecting connection lost from MQTT broker and reconnection automatically
 - OTA update - easy update using firmware upload via web interface
 - support for up to 10 temperature sensors connected via 1-wire, automatic search for sensors - no need to manually write addresses
   (a modified version of the library is used, which solved the problems of simultaneous operation of 1wire and wifi on esp32 chips)
 - control up to 10 OUTPUT pins switches (for relays etc)
 - control states up to 10 INPUT pins (in the settings it is possible to switch PULLUP and PULLDOWN modes)
 - easy button configuration with bounce protection, allow to collect events: current status, single click, long click, double click
 - reading values of up to 5 analog signals from standard analog pins
 
 <b>IMPORTANT INFO:</b>
  - basic WiFi AP ssid: esp32mqtt (no password)
  - ESP IP in AP mode: 192.168.4.1
  - basic HTTP auth: admin / admin
  
