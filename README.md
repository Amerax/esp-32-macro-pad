# esp-32-macro-pad

MacroPad Project
Ok so I have took a very long break for this project vbecase of school and other stuff and i compltly forgot about it. But now I am back and was able to do the actual build because most of the code was done and also I drew the schmeatics already so it was already there for me. I just had to revise and look back at the stuff I did and wrote and made because I forgot how it worked. So anyways i went back and rechecked it and now it looks good and I was able to understand it and finish the code.

I had to build it and the hardest part was figuring out why it did not work when I booted, the lights did not turn on the esp 32. That was because there is a weird quirk or something with pin 2 which is used for something else apparntly so I swapped the pin to another pin in my code and it started working. I guess I was testring it out before. So anyways just don't put anything on pin 2. 

Parts:
ESP32 
Oled screen
4x buttons
Pentiometer

Libary:
Adafruit_SSD1306
BleKeyboard

The Wiring Stuff
ESP32 Dev 
OLED Screen via I2C (SDA/SCL pins)   
Button 1: Pin 18 (Sends Ctrl+C)   
Button 2: Pin 4 (Sends Ctrl+V)   
Button 3: Pin 5 (Sends Ctrl+Z)   
Button 4: Pin 19 (Sends Ctrl+A)   
Ponteialometer: Pin 34 (Analog input)   

This uses multiple libaries like bluetooth libary and keyboard libary of adrafruit so it can use the bluetooh feature on the motherboard computer and also send input. 

Image:

<img width="1323" height="978" alt="image" src="https://github.com/user-attachments/assets/cf3e0e6d-3756-4a2f-803c-175dc5d14b34" />

Demo Video Link on Youtube:

https://youtu.be/6L0EWMCvtHA
