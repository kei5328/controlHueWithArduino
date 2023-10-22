# controlHueWithArduino

<h3>Summary</h3>
This is a project that enables the control of the Philips Hue Lights via esp8266 and arduino with a help of an ultrasonic sensor. 
Basically, when something close is detected by the ultrasonic sensor, the arudino turns on the detection signal, which is received by esp8266. 
With this signal, the esp8266 sends the on/off to Hue lights using http request to change the status of the lights in the network. 


<h3>Motivation</h3>
I typically use Alexa to turn on/off the lights. However, my Alexa lives in my bedroom. So, when I go out of my house, I always had to yell "ALEXA!!!!! TURN OFF THE LIGHTS!!!!" so she can hear me. I thought this was very inconvenient and annoys my neighbours. However, I'm lazy so I don't want to bring up my phone to open up the Hue app. 
That's why I wanted to have some switch to be placed near the entrance to turn on/off all Hue Lights. 


![alt text](https://github.com/kei5328/controlHueWithArduino/blob/master/picture.jpg?raw=true)
