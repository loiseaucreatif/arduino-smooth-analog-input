/*  
 ** 
     Fast Smoothing of Analog Input Data
     A simple base experiment on Arduino.
     Code with comments :-)
     
     Loiseau créatif - https://loiseaucreatif.com - 2020
     https://github.com/loiseaucreatif/arduino-smooth-analog-input

     The goal is to have stable, smooth and precise readings from a potentiometer connected on an analog pin of an Arduino.

     I'm not an expert, there's certainly improvements to make.
     This experiment can be a source of inspiration and information for Makers.
     Feel free to interact on GitHub ;-)

     At start I watched the YouYube Live Bar Arduino #8
     https://www.youtube.com/watch?v=0zNmt_IKwRg
     Ubi de Feo talked about Fast Smoothing of Analog Input, and showed his code in action.  
     This inspired me, so I took up and analyzed his code, and researched more about this topic.  
    
     Enjoy!

     ----

     Methods used:
     - Running Average algorythm with a FIFO data structure (First In First Out)
     - Hardware RC Low Pass Filter (see schematic)
     - Mapping filtered values

     The running average algorythm seems like a good alternative to a simple average to give a smoother output and let the Arduino work on other things.
     But it has one big down side: memory use.
     
     The hardare RC Low Pass Filter on the input analog pin stabilizes and smoothes the signal.
     You can use these components and test other values: 100nF(104) Ceramic Condensator + 10KOhm resistor.
     Online Frequency Calculator: http://www.learningaboutelectronics.com/Articles/Low-pass-filter-calculator.php

     ----

     You can find additional informations here:
     - 3 methods to filter noisy Arduino measurements: https://www.megunolink.com/articles/coding/3-methods-filter-noisy-arduino-measurements/
     
     - Arduino Reference: https://www.arduino.cc/reference/en/language/functions/analog-io/analogread/

     - Arduino Smoothing Tutorial: https://www.arduino.cc/en/tutorial/smoothing

     An additional method called Hysteresis can help you to define actions depending on range of readed values:
     https://forum.arduino.cc/index.php?topic=526806.0
**
*/


// Initialize variables for the Running Average algorythm
// You can adjust the SAMPLES_COUNT but be careful, more samples use more memory!
const int SAMPLES_COUNT = 32;
int samplesBuffer[SAMPLES_COUNT] = {0}; 
int sampleIndex = 0;
int lastSensorValue;
int lastMappedValue;
unsigned long samplesSum = 0;

// Adjust with your connected pin sensor
const int knobInputPin = A0;


void setup() {
  Serial.begin(57600);
  delay(5000); // Delay to initialize Serial connection
  Serial.println("Let's go!");
}


void loop() {
  readMyKnob();
}


// Function to lauch an analog reading and take actions with the result
// Here we map the value and print it on the Serial interface
// But you can perform what actions you need, like an Hysteresis method...
void readMyKnob() {
  int knobReadOut = filterAnalogValue(knobInputPin);
  
  // You can map here the resulting value
  // Mapping is the final touch to have more stable readings and try to kill last bits of noise
  // But some old potentiometers can stay very noisy and hard to control 
  int mappedValue = map(knobReadOut, 0, 1023, 0, 127);

  // If a new value is detected, print it on the Serial interface
  if (lastMappedValue != mappedValue) {    
    Serial.println(mappedValue);
  }
  
  lastMappedValue = mappedValue;
}


// The core algorythm
int filterAnalogValue(int _inputPin) {
  samplesSum = 0;
  int analogValue = analogRead(_inputPin);

  if (sampleIndex < SAMPLES_COUNT) {
    // Initial loop, fill the buffer with SAMPLE_COUNTS values
    samplesBuffer[sampleIndex] = analogValue;
    sampleIndex++;
    } else {
    // Shifts the values in the stack one position to the left
    for (int samplePosition = 0; samplePosition < sampleIndex - 1; samplePosition++) {
      samplesBuffer[samplePosition] = samplesBuffer[samplePosition + 1];
    }
    // The last value in the stack is now the last analog reading value
    samplesBuffer[SAMPLES_COUNT - 1] = analogValue;
  }

  // Add all the values in the stack
  for (int samplePosition = 0; samplePosition < sampleIndex; samplePosition++) {
    samplesSum += samplesBuffer[samplePosition];
  }

  // Return the filtered value (the sum divided by number of samples)
  if (samplesSum == 0) {
    return 0;
  } else {
    unsigned int filteredValue = samplesSum / sampleIndex;
    return filteredValue;
  }
}
