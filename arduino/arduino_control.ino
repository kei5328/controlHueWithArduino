

// define the pins for outputs and ultrasonic pins.
const int OUTPUT_PIN = 5;  // out pin for close range detected.
const int ULTRA_ECHO_PIN = 2;
const int ULTRA_TRIG_PIN = 3;

const int ULTRA_THRESHOLD = 10; // distance threshold for close object

uint8_t consistCnt = 0; // counter for consistently detection close objects.
bool is_close = false; // indicates if there is something close consistently.


// trigeres the ultrasonic sensor to find the proximity to the object.
int getUltraDist()
{
    // define normal vars.
    long ultraDuration;
    int ultraDist;

    digitalWrite(ULTRA_TRIG_PIN, LOW);
    delayMicroseconds(2);
    // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
    digitalWrite(ULTRA_TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(ULTRA_TRIG_PIN, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    ultraDuration = pulseIn(ULTRA_ECHO_PIN, HIGH);
    // Calculating the distance
    ultraDistance = ultraDuration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
    return ultraDistance;
}

// setup funciton, to be run once at bootup.
void setup()
{
    pinMode(OUTPUT_PIN, OUTPUT);  // sets the digital pin 13 as output
    pinMode(ULTRA_TRIG_PIN, OUTPUT); // Sets the ULTRA_TRIG_PIN as an OUTPUT
    pinMode(ULTRA_ECHO_PIN, INPUT); // Sets the ULTRA_ECHO_PIN as an INPUT
}


// loop function, to be run repeatedly.
void loop() {
    // get the distance and detect the proximity.
    int ultraDistance = getUltraDist();
    if (ultraDistance<ULTRA_THRESHOLD)
    {// there is something within the threshold.
        if (consistCnt<10)
            consistCnt++;
    }
    else
    {
        if (consistCnt>0)
            consistCnt--;
    }

    if (is_close)
    {// currently the close object is detected.
        if (consistCnt<=5)
        {// turn the detection off.
            is_close = false;
            consistCnt = 0;
            digitalWrite(ledPin, LOW);  // sets the LED to the button's value
          }
    }
    else
    {// curerntly no object is close to the ultrasonic sensor
        if (consistCnt>=10)
        {// consistenly detected to be close
            is_close = true;
            digitalWrite(OUTPUT_PIN, HIGH);  // sets the LED to the button's value
        }
    }
}
