# controlHueWithArduino

This is a project that enables the control of the Philips Hue Lights via esp8266 and arduino with a help of an ultrasonic sensor. 
Basically, when something close is detected by the ultrasonic sensor, the arudino turns on the detection signal, which is received by esp8266. 
With this signal, the esp8266 sends the on/off to Hue lights using http request to change the status of the lights in the network. 
