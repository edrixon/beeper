//  Alerter for Met. Office codec connections
//    - When trigger input goes low (from codec gpio pin)
//      turns "on-air" light on and then beeps four times
//    - when trigger goes high again, turns off "on-air" light
//
//  For Arduino Trinket
//    don't use internal pull up on input, use stronger external one for noise immunity
//    on-air light is on same pin as on-board led (pin 1)
//    beeper is on pin 2
//    don't use gpio 3 or 4 because they are shared with usb programmer
//

// Trinket gpio pins
#define PIN_BEEP    0
#define PIN_LIGHT   1
#define PIN_TRIGGER 2
#define PIN_USB_1   3
#define PIN_USB_2   4

// Beep defines
#define BEEP_TIME   60   // on-time in milliseconds
#define QUIET_TIME  60   // off-time
#define BEEP_COUNT  4    // number of beeps when triggered

#define IDLE_TIME   50   // Delay in idle state

// State machine states
enum activeState_t
{
    ACTIVE_START,
    ACTIVE_BEEP_ON,
    ACTIVE_BEEP_OFF,
    ACTIVE_IDLE
};

//
//
//
activeState_t activeState;
int beepCount;
int waitTime;

void setup()
{
    int c;
  
    // Port setup
    pinMode(PIN_LIGHT, OUTPUT);
    pinMode(PIN_BEEP, OUTPUT);
    pinMode(PIN_TRIGGER, INPUT);
    pinMode(PIN_USB_1, OUTPUT);
    pinMode(PIN_USB_2, OUTPUT);

    digitalWrite(PIN_TRIGGER, HIGH);

    digitalWrite(PIN_BEEP, LOW);

    for(c = 1; c < 5; c++)
    {
        digitalWrite(PIN_LIGHT, HIGH);
        delay(50);
        digitalWrite(PIN_LIGHT, LOW);
        delay(50);
    }

    activeState = ACTIVE_START;

}

// Main loop
void loop()
{
    // See if triggered...
    if(digitalRead(PIN_TRIGGER) == LOW)
    {
        // studio has connected
        
        switch(activeState)
        {
            case ACTIVE_START:
                digitalWrite(PIN_LIGHT, HIGH);
                activeState = ACTIVE_BEEP_ON;
                beepCount = BEEP_COUNT - 1;
                waitTime = 0;
                break;
                
            case ACTIVE_BEEP_ON:
                digitalWrite(PIN_BEEP, HIGH);
                activeState = ACTIVE_BEEP_OFF;
                waitTime = BEEP_TIME;
                break;

            case ACTIVE_BEEP_OFF:
                digitalWrite(PIN_BEEP, LOW);
                waitTime = QUIET_TIME;
                // See if this is the last beep
                if(beepCount)
                {
                    // still more beeps
                    beepCount--;
                    activeState = ACTIVE_BEEP_ON;
                }
                else
                {
                    // finished beeping
                    activeState = ACTIVE_IDLE;
                    waitTime = IDLE_TIME;
                }
                break;

            case ACTIVE_IDLE:;
            default:;
                // Do nothing, finished beeping
                // just waiting for trigger to go away now...
                break;

        }

    }
    else
    {
        // studio has disconnected

        if(activeState != ACTIVE_START)
        {
            digitalWrite(PIN_BEEP, LOW);  // Make sure beeper is off
            digitalWrite(PIN_LIGHT, LOW); // Turn off on-air light
            activeState = ACTIVE_START;   // Reset state machine
            waitTime = IDLE_TIME;
        }
    }

    delay(waitTime);

}
