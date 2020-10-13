# battery_monitor_to_web & using WemosD1 & voltage divider resistors
Arduino code for monitoring a car battery volt reading and posting reading onto a webserver using the WemosD1

This project reads the voltage of a (car) battery or any other battery and posts the result volts reading to a webserver with a timestamp from an NTP server using WemosD1. 

1.Set webserver on port 80  
2.Sets fixed IP address to the WemosD1  
3.Read the voltage input from battery on analog pin 0 on a scale and converts to human understanding of volts using calculations including voltage divider resistors used.  
4.Connects to an NTP time server and sets refresh rate  
5.Sets functions for resetting the wemos, doing timecheck and formatting, handleroot.   
6.Void setup - Allcocates fixed IP, Starts webserver, serial prints information.  
7.Void loop - checks time, checks A0 pin volts, check for web requests  
  
function resetting the wemos is webpage /reset that will 'reboot' the wemos manually    
function timecheck checks time and if midnight, 'reboots' the wemos   
funtion handleroot prints text, time and volts to webpage   
resistors used r1 was around 10m and r2 1m    

  
