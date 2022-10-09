#include "ESP8266HTTPClient.h"
#include "ESP8266WiFi.h"
#include "network_info.h" // contains the network info.(SSID, PASSWORD, LOCAL_IP, and USER_NAME)

// input and output definition.
const int INPUT_PIN = 2;
const int OUTPUT_PIN = 0;

// json contents to turn on, off the hue lights
const char * ON_CMD = "{\"on\":true, \"bri\":254}";
const char * OFF_CMD = "{\"on\":false}";

// global vars
uint8_t consistCnt = 0; // counter to check the consistency in the input.
uint8_t hue_state = 0; // 0 is off; 1 is on; (we may have more states in the future. )
bool isOn = 0; // indicates the input signal from arudino with consistent check.
bool prevOn = 0; // indiates the previous signal from arduino with consistent check.
bool scan_complete = false; // indicates if the scan has been done at the beginning.
uint8_t num_lights = 0;     // # of lights. (gets it with get request. )

// this function "parses" jason just to find out the IDs of the lights in the network(returned by get request)
uint8_t countNumOfLights(String json)
{
    int len = json.length();
    int num_bracket = 0;
    uint8_t num_lights = 0;
    for (int ii = 0; ii<len; ii++)
    {
        if (json[ii] == '{')
        {
            num_bracket++;
            if (num_bracket==2)
                num_lights++;
        }
        else if (json[ii] == '}')
            num_bracket--;
    }
    Serial.print("# of lights=");
    Serial.println(num_lights);
    return num_lights;
}

// this funciton connects the esp8266 module to the network(wifi).
void connectToWifi()
{
    // normal vars
    uint8_t cnt = 0;
    // start of the process.
    WiFi.begin(SSID, PASSWORD); // start the conneciton
    while (WiFi.status() != WL_CONNECTED)
    {// the wifi is not connected yet.
        delay(1000);
        Serial.println("Connecting to WiFi..");
        // blink the output pin to let user know it's doing connection.
        if (cnt%2==0)
            digitalWrite(OUTPUT_PIN, HIGH);
        else
            digitalWrite(OUTPUT_PIN, LOW);

        cnt++;
        if (cnt>60) // after 60trials(60seconds), abort the proces.
            break;
    }
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Wifi connection failed...");
        uint8_t cnt = 0;
        while(cnt<250)
        {
            // let the user know with faster blinking(2Hz) that wifi connection failed.
            if (cnt%2==0)
                digitalWrite(OUTPUT_PIN, HIGH);
            else
                digitalWrite(OUTPUT_PIN, LOW);
            cnt++;
            delay(500);
        }
    }
    else
    {// with successful conneciton: stop blinking and turn it off.
        digitalWrite(OUTPUT_PIN, LOW);
        Serial.println("Wifi connected");
        Serial.println(WiFi.localIP());
    }
}

// sets the certain pins output/input modes. Should be called once at setup.
void setPins()
{
    pinMode(INPUT_PIN, INPUT); // signal input
    pinMode(OUTPUT_PIN, OUTPUT); // signaloutput
}


// send the http GET request to given url
/*
    input: url to send the get request.
    output: content obtained with get request.
*/
String httpGETRequest(const String url)
{
    WiFiClient client;
    HTTPClient http;

    http.begin(client, url);
    int httpResponseCode = http.GET();

    String response = "{}";
    if (httpResponseCode>0) {
        response = http.getString();
        Serial.println(httpResponseCode);
        Serial.println(response);
    }
    else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }
    http.end();
    return response;
}
// send the http PUT request to given url
/*
    input:
        url: url to send the get request.
        content: content of the put request
    output: content obtained with get request.
*/
String httpPUTRequest(const String url, const String content)
{
    WiFiClient client;
    HTTPClient http;
    // start the http;
    http.begin(client, url);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.PUT(content);
    String response = "{}";
    if(httpResponseCode>0){
        response = http.getString();
        Serial.println(httpResponseCode);
        Serial.println(response);
    }
    else
    {
        Serial.print("Error on sending PUT Request: ");
        Serial.println(httpResponseCode);
    }
    http.end();
    return response;
}

// setup funciton to be run once.
void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200); // set up the serial.
    setPins(); // sets the output/input modes for GPIO pins.
    connectToWifi(); // try to connect to wifi at bootup.
}

// loop functiion to run repeatedly.
void loop() {
    int val = digitalRead(INPUT_PIN);
    // check the connection to the wifi.
    if (WiFi.status()!=WL_CONNECTED)
    {
        scan_complete = false; // once disconnected, reset the light scanning status.
        connectToWifi();
        delay(1000);
    }

    // detect the input.
    if (val==HIGH)
        if (consistCnt<=100) consistCnt++;
    else
        if (consistCnt>0) consistCnt--;
    if (isOn)
    {
        if (consistCnt<=50)
        {
            isOn = false;
            consistCnt = 0;
            digitalWrite(OUTPUT_PIN, LOW);  // turn off the LED
        }
    }
    else
    {
        if (consistCnt>100)
        {
            isOn = true;
            digitalWrite(OUTPUT_PIN, HIGH);  // turn on the LED.
        }
    }

    if (isOn && !prevOn)
    {// when it's just turned on.
        if (WiFi.status()==WL_CONNECTED)
        {// as long as connected to wifi.
            if (!scan_complete)
            {
                String get_url = "http://" + local_ip + "/api/"+ USER_NAME + "/lights/";
                String hueStatus = httpGETRequest(get_url);
                num_lights = countNumOfLights(hueStatus);
                scan_complete = true; // scan is complete: we know the # of lights in the network.
            }

            for (int ii = 0; ii<num_lights; ii++)
            {// loop thru the lights to turn on/off.
                String light_id = String(ii+1); // light id starts from 1.
                String put_url = "http://192.168.0.170/api/" + USER_NAME + "/lights/" + light_id + "/state/"; // construct the put url.
                Serial.println(put_url);
                if (hue_state==0)
                {// the hue lights are considred to be off.
                    String response = httpPUTRequest(put_url, ON_CMD);
                }
                else if (hue_state==1)
                {// the hur lights are considered to be on.
                    String response = httpPUTRequest(put_url, OFF_CMD); // sends the off command.
                }
            }
            // update hue_state after the loop.
            if (hue_state==1)
                hue_state = 0;
            else if (hue_state==0)
                hue_state = 1;
        }
        else // wifi is not connected.
            connectToWifi();
        delay(1000);
    }
    delay(5); // run it with at most 200Hz.
    prevOn = isOn;
}
