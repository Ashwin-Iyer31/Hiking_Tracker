#include <math.h>                                                           // Math Library
#include <Wire.h>                                                           // Wire library - used for I2C communication
#include <Adafruit_GFX.h>                                                   // Library for the OLED
#include <Adafruit_SSD1306.h>                                               // Library for the OLED     
#include <Adafruit_BMP280.h>                                                // Library for BMP280
#include <MechaQMC5883.h>                                                   // Library for HMC5883
#include <Adafruit_Sensor.h>                                                // Library for ADXL345
#include <Adafruit_ADXL345_U.h>                                             // Library for ADXL345

#define SCREEN_WIDTH 128                                                    // OLED display width, in pixels
#define SCREEN_HEIGHT 32                                                    // OLED display height, in pixels
#define OLED_RESET    -1                                                    // Reset Pin Hash

MechaQMC5883 qmc;                                                           // Declaring HMC5883
int x, y, z, az;                                                            // Variables for HMC5883 and ADXL345

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified();                // Declaring ADXL345

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);   // Declaring OLED

Adafruit_BMP280 bmp;                                                        // Declaring BMP280
float T,P,A;                                                                // Variables for BMP280

void setup() {  
  bmp.begin();                                                              // Start the bmp                  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);                                // Start the OLED display
  display.clearDisplay();                                                   // Clearing Display
  display.setTextSize(1);                                                   // Setting Text Size
  display.setTextColor(WHITE);                                              // Setting Text Colour
  display.display();                                                        // Commit the Changes
  Serial.begin(9600);
  if(!accel.begin()){                                                       // Check ADXL345 
      Serial.println("No ADXL345 sensor detected.");
      while(1);
  }
  Wire.begin();
  qmc.init();                                                               // Start HMC
}

void loop() {
    display.clearDisplay();                                                 // Clearing Display
    display.drawLine(38, 0, 38, 32, WHITE);                                 // Drawing Lines
    display.drawLine(38, 24, 128, 24, WHITE);                               
    T = bmp.readTemperature();                                              // Read temperature in C
    P = bmp.readPressure()/1000;                                            // Read Pressure in Pa and conversion to kPa
    A = bmp.readAltitude(1019.66);                                          // Calculating the Altitude, the "1019.66" is the
                                                                            // pressure in (hPa) at sea level at day in the current region
    sensors_event_t event;                                                  // Creating Sensor Event
    accel.getEvent(&event);                                                 // Passing the Event for ADXL345
    x = event.acceleration.x;                                               // Getting ADXL345 Sensor Data
    y = event.acceleration.y;
    z = event.acceleration.z;
    float G = (sqrt(x*x+y*y+z*z)-0.3)/9.83;                                   // Subrtracting Offset and Changing to g Units                                                       
    Serial.print(G);
    Serial.println("  g.uts.");
    
    qmc.read(&x, &y, &z, &az);                                              // Getting HMC5883 Sensor Data
    float heading = atan2(y, x);                                            // Calculate Heading
    if(heading < 0)                                                         // Readings Corrections
      heading += 2*PI;
    if(heading > 2*PI)
      heading -= 2*PI;
    float H = heading*180/M_PI;                                             // Converting Radians to Degrees
    display.drawCircle(12,20,11,WHITE);                                     // Drawing Circle
    display.drawLine(12,20,9*cos(az)+12,abs(9*sin(az)-20), WHITE);          // Drawing the Compass Needle

    display.setCursor(40,25);                                               // Displaying Data on OLED
    display.print("HEAD:");
    display.setCursor(74,25);
    display.print(H);

    display.setCursor(0,0);
    display.print("AZMTH:");
    
    display.setCursor(40,0);
    display.print("TEMP:");
    display.setCursor(74,0);
    display.print(T,1);
    display.setCursor(110,0);
    display.print("C");
    
    display.setCursor(40,8);
    display.print("PRES:");
    display.setCursor(74,8);
    display.print(P);
    display.setCursor(110,8);
    display.print("kPa");

    display.setCursor(40,16);
    display.print("ALT :");
    display.setCursor(74,16);
    display.print(A,0);
    display.setCursor(110,16);
    display.print("m");
    
    display.display();
    delay(5000);                                                             // Update Every 5 Seconds
}
