#include "led.h"

// Pattern states
enum LedMode {
    LED_IDLE,
    LED_WIFI,
    LED_SERVER,
    LED_SAMPLE,
    LED_UPLOAD
};

static LedMode currentMode = LED_IDLE;

static unsigned long lastToggle = 0;
static uint8_t step = 0;
static bool ledState = false;

// --------------------
// Init
// --------------------
void initLED() {
    pinMode(STATUS_LED, OUTPUT);
    digitalWrite(STATUS_LED, LOW);
}

// --------------------
// Pattern Triggers
// --------------------

void ledWifiConnect() {
    currentMode = LED_WIFI;
    step = 0;
}

void ledServerResponse() {
    currentMode = LED_SERVER;
    step = 0;
}

void ledSampleTaken() {
    currentMode = LED_SAMPLE;
    step = 0;
}

void ledUpload() {
    currentMode = LED_UPLOAD;
    step = 0;
}

// --------------------
// Update Engine
// Call frequently
// --------------------
void ledUpdate() {

    unsigned long now = millis();

    switch (currentMode) {

        // --------------------
        // WIFI CONNECT
        // Slow blink
        // Power efficient
        // --------------------
        case LED_WIFI:

            if (now - lastToggle > 700) {
                lastToggle = now;

                ledState = !ledState;
                digitalWrite(STATUS_LED, ledState);

                if (++step > 6) {
                    digitalWrite(STATUS_LED, LOW);
                    currentMode = LED_IDLE;
                }
            }
            break;


        // --------------------
        // SERVER RESPONSE
        // Double fast blink
        // --------------------
        case LED_SERVER:

            if (now - lastToggle > 120) {
                lastToggle = now;

                ledState = !ledState;
                digitalWrite(STATUS_LED, ledState);

                if (++step > 4) {
                    digitalWrite(STATUS_LED, LOW);
                    currentMode = LED_IDLE;
                }
            }
            break;


        // --------------------
        // SAMPLE TAKEN
        // Single short blink
        // Ultra low power
        // --------------------
        case LED_SAMPLE:

            if (step == 0) {
                digitalWrite(STATUS_LED, HIGH);
                lastToggle = now;
                step = 1;
            }

            if (step == 1 && now - lastToggle > 40) {
                digitalWrite(STATUS_LED, LOW);
                currentMode = LED_IDLE;
            }

            break;


        // --------------------
        // UPLOAD
        // Triple blink
        // --------------------
        case LED_UPLOAD:

            if (now - lastToggle > 150) {
                lastToggle = now;

                ledState = !ledState;
                digitalWrite(STATUS_LED, ledState);

                if (++step > 6) {
                    digitalWrite(STATUS_LED, LOW);
                    currentMode = LED_IDLE;
                }
            }

            break;


        default:
            break;
    }
}