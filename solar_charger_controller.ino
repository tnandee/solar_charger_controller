/*
  Solar charger controller, or monitor or whatever
 */

#include <SoftwareSerial.h>

void meas(void);
void sendMeas(float a, float b, float c);

float VREF = 1.1;
float ADSTEP = VREF / 1024.0;

int GEL_CURR_PIN = 7;
int GEL_VOLT_PIN = 6;
int USB_CURR_PIN = 0;

float GEL_CURR_MUL = 1.0 * ADSTEP * 1000.0; // [mA]
float gelC;
float GEL_VOLT_MUL = 15.7 * ADSTEP; // 1k - 14.7k [V]
float gelV;
float USB_CURR_MUL = 2.0 * ADSTEP * 1000.0; // 4k7 - 4k7 [mA]
float usbC;

#define SAMPLE_PERIOD_MS  100
#define DEF_SAMPLE_NO 16
long lastMsg = 0;
int sampleLeft = DEF_SAMPLE_NO; 
float avgGelC = 0.0;
float avgGelV = 0.0;
float avgUsbC = 0.0;

SoftwareSerial BTserial(11, 12);

void setup() {
  analogReference(INTERNAL);
  BTserial.begin(9600);
}

void loop() {
  while (BTserial.available() > 0) {
    if (BTserial.read() == '\n') {
      meas();
      sendMeas(gelV, gelC, usbC);
    }
  }
  avgMeas();  
}

void avgMeas(){
  long now = millis();
  if (now - lastMsg > SAMPLE_PERIOD_MS) {
    lastMsg = now;

    meas();

    avgGelV += gelV;
    avgGelC += gelC;
    avgUsbC += usbC;

    sampleLeft -= 1;    
  }
  if (sampleLeft <= 0){
     avgGelV = avgGelV / DEF_SAMPLE_NO;    
     avgGelC = avgGelC / DEF_SAMPLE_NO;
     avgUsbC = avgUsbC / DEF_SAMPLE_NO;

     sendMeas(avgGelV, avgGelC, avgUsbC);
     
     sampleLeft = DEF_SAMPLE_NO;
     avgGelV = avgGelC = avgUsbC = 0.0;
  }
}

void meas(){
    gelV = analogRead(GEL_VOLT_PIN) * GEL_VOLT_MUL;
    gelC = analogRead(GEL_CURR_PIN) * GEL_CURR_MUL;
    usbC = analogRead(USB_CURR_PIN) * USB_CURR_MUL;
}

void sendMeas(float a, float b, float c){
    BTserial.print(a, 2);
    BTserial.print(",");
    BTserial.print(b, 0);
    BTserial.print(",");
    BTserial.println(c, 0);  
}

