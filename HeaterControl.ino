#include <SHT1x.h>
#include <Servo.h>
#include <EEPROM.h>


Servo servo1;
const int servoPin = 9;
char inputChar = 0;
const int heaterServoOn = 160;
const int heaterServoOff = 90;

//SHT Data and Clock Pins;
const int shtDataPin = A5;
const int shtSCKPin = A4;
SHT1x sht(shtDataPin, shtSCKPin);


//Buttons
int  adc_key_val[5] ={30, 150, 360, 535, 760};
int NUM_KEYS = 5;
const int buttonPin = A7;
int buttonIn = 0;
int buttonNum = -1;
int oldButtonNum= -1;

bool heaterState = false;
const int heaterBGap = 1; //Gap in degree F before turning on 
const int heaterAGap = 2; //Gap in degree F before turning off

const int heatLED = 13;


//Counting check to save temp to EEPROM
const int saveTempInterval = 120000; //in milliseconds if value has changed
unsigned long saveOldTime = millis();

//Temp handling var
int setTemp = 70;
int currTemp = 0;
int savedTemp = 0;
const int eeAdd = 0; //EEPROM Address for saving Temp




void setup()
{
 Serial.begin(9600);
 delay(1000);
 currTemp = getTemp();
 Serial.println("Online");
 
 savedTemp = EEPROM.read(eeAdd);
 
 //Init first save of default temp;
 if (savedTemp == 255) EEPROM.write(eeAdd, setTemp);
 
 setTemp = savedTemp;
 Serial.print("Saved Set Temp: ");
 Serial.print(savedTemp);
 Serial.print("F, Current Temp: ");
 Serial.print(currTemp);
 Serial.println("F");
 
 pinMode(heatLED, OUTPUT);

}


void loop()
{
  //Refresh current temp 
  currTemp = getTemp();
   
  //Save setTemp if changed based on interval
  if (millis() > (saveOldTime+saveTempInterval)  ) {
    
     if (setTemp != savedTemp){
       
       EEPROM.write(eeAdd, setTemp);
       savedTemp = setTemp;
       saveOldTime = millis(); //Reset time
       Serial.println(savedTemp);
     
     }
    
  }
  
  if ((setTemp-heaterBGap >= currTemp) and (heaterState == false)){
    
    
    heaterOn(); 
    heaterState = true;  
    digitalWrite(heatLED, HIGH); 
    printTemp();
    
  }
  
  if ((setTemp+heaterAGap <= currTemp) and (heaterState == true)){
    
    
    heaterOff();
    heaterState = false;
    digitalWrite(heatLED, LOW);
    printTemp();

  }
  
  
  
  
  
  if (Serial.available()){
    
   inputChar = Serial.read();
  
  switch(inputChar){
   
   case 'o':
      
      heaterOn();
   
   break;
   
   
   case 'f':
   
       heaterOff();
   
   break;
   
   
   case 'u':
   
      //setServo(10);
      setTemp++;
      printTemp();
      
      
   
   break;
   
   
   case 'd':
   
    //setServo(-10);
    setTemp--;
    printTemp();
   
   break;
   
   case 't':
     
     printTemp();
     
  break;
   
   
  } 
  
   }
   
   
  buttonIn  = analogRead(buttonPin);
  
  buttonNum = get_key(buttonIn);
  
  if (buttonNum != oldButtonNum) {   // if keypress is detected
    delay(50);      // wait for debounce time
    buttonIn = analogRead(buttonPin);    // read the value from the sensor  
    buttonNum = get_key(buttonIn);    // convert into key press
    if (buttonNum != oldButtonNum) {         
      oldButtonNum = buttonNum;
      if (buttonNum >=0){
       // Serial.println(buttonNum);
        switch (buttonNum){
          
        case 0:
      
         //setServo(10);
         setTemp++;
         printTemp();
         
        break;
        
        case 1:
        
          heaterOff();
        
        break;
         
        case 2:
         
         //setServo(-10);
         setTemp--;
         printTemp();
           
        break;
      
        case 3:
        
          heaterOn();
        
        break;
       
          
          
          
        }

      }
    }
  }
  
  
  
}




void setServo(int degree)
{
  
  int currPos = servo1.read();
  int newPos = currPos+degree;
  int setPos = 0;
  
  setPos = constrain(newPos, 90, 160);
  servo1.attach(servoPin);
  
  servo1.write(setPos);
  delay(500);
  servo1.detach();   
  Serial.println(servo1.read());
 
  
}


int get_key(unsigned int input)
{   
  int k;
  for (k = 0; k < NUM_KEYS; k++)
  {
    if (input < adc_key_val[k])
    {  
      return k;  
    }
  }
  if (k >= NUM_KEYS)
    k = -1;     // No valid key pressed
  return k;
}


void printTemp()
{
  
 int temp;
 int humidity; 
 
 temp = sht.readTemperatureF();
 humidity = sht.readHumidity();
 
 Serial.print("Current Set Temp: ");
 Serial.print(setTemp, DEC);
 Serial.print("F, ");
 Serial.print("Current Temp: ");
 Serial.print(temp, DEC);
 Serial.print("F, Current Humidity: ");
 Serial.print(humidity, DEC);
 Serial.println("%");

}


int getTemp()
{
 int temp;
 temp = sht.readTemperatureF();
 
 return temp; 
 
}

void heaterOn(){
  
  
     Serial.println("Heater On");
     servo1.attach(servoPin);
     servo1.write(heaterServoOn);
     delay(500);
     servo1.detach();
  
}


void heaterOff(){
  
   Serial.println("Heater Off");
   servo1.attach(servoPin);
        
   servo1.write(heaterServoOff);
   delay(500);
   servo1.detach();   
  
}


