#include <Conceptinetics.h>
#include <Servo.h>

///// Set DMX channel and quantity here
const int dmxStartChannel = 1;
const int numChannels = 3;

///// Set Servo pins to map to each DMX channel
const int servoOutputs[numChannels] = {9,10,11};

// Define arrays for DMX channel and servo pin mapping
int dmxChannels[numChannels];

DMX_Slave dmx_slave ( numChannels );
Servo servo[numChannels];  

void setup() {
  // Populate the DMX channel array with sequential channel numbers
  for (int i=0; i<numChannels; i++) { dmxChannels[i]=dmxStartChannel+i; }

  // Initialise the DMX receiver
  dmx_slave.enable ();  
  dmx_slave.setStartAddress (dmxChannels[0]);

  // Initialise the servos
  for (int i=0; i<numChannels; i++) {
    servo[i].attach(servoOutputs[i]);
  } 
}

void loop() {
  // Read each DMX channel and map the 0-255 value to a 0-180 value to the respective servo pin
  for (int i=0; i<numChannels; i++) {
    servo[i].write(map(dmx_slave.getChannelValue(dmxChannels[i]), 0, 255, 0, 180));
  }
    
  
  
}
