#include "debugmode.h"

// Set this to true to enable debug prints, false to disable
bool DEBUG_MODE = true;

/* * Pro-Tip for C3: 
 * If you are using the internal USB-Serial JTAG, 
 * add 'while(!Serial);' in your main.ino setup() 
 * to ensure you don't miss the first few boot messages.
 */