#include <SoftwareSerial.h>

SoftwareSerial ss(3,4);


int Thermistorpin = 0;
int Vo;
float R1 = 10000;
float logR2, R2, T ,Tc;
float c1 = 0.0009100954221345331, c2 = 0.00021484415312397265, c3 = 0.00000010654853567651721;


String nmeasentez;
float latitude, longitude;


void setup() {
  Serial.begin(9600);
  ss.begin(9600);
}

void loop() {
   
   Vo = analogRead(Thermistorpin);
   R2 = R1 * (1023.0 / (float)Vo - 1.0);
   logR2 = log(R2);
   T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
   Tc = T - 273.15;

 
  while (ss.available() > 0) {
    char c = ss.read();
    nmeasentez += c;
  
    if (c == '\n') {
      
      if (nmeasentez.startsWith("$GPGGA")) {
        int commacounter = 0;
        int startindex = 0;

  
        for (int i = 0; i < nmeasentez.length(); i++) {
          if (nmeasentez[i] == ',') {
            commacounter++;
            if (commacounter == 2) {
              
              startindex = i + 1;
             } else if (commacounter == 4) {
              
              int latituteindex = startindex;
              int longituteindex = i;

              latitude = parsenmeacoordinate(nmeasentez.substring(latituteindex, longituteindex)); 
              longitude = parsenmeacoordinate(nmeasentez.substring(longituteindex + 1));

              Serial.print("#"); 
              Serial.print(Tc); 
              Serial.print(",");
              Serial.print(latitude, 2);   
              Serial.print(",");
              Serial.print(longitude, 2);   
              Serial.println("*");
              


              break;  
            }
          }
        }
      }

      nmeasentez = ""; 
    }
  }
}



    
   float parsenmeacoordinate(String coordinate) {
  

   int dotindex = coordinate.indexOf('.');  // indexof kordinatta '.'yı arar
   float minutes = coordinate.substring(dotindex - 2, dotindex + 5).toFloat(); //substring verileri bölerek kaçıncı veriden sonra değer alacağını seçer.
   int degree = coordinate.substring(0, dotindex - 2).toInt(); //toint verileri istenilen bit değerini dönüştürür 

   float floatdegree = degree + (minutes / 60.0);
   return floatdegree;
}