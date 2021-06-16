#include "mbed.h"
#include "bbcar.h"
BufferedSerial pc(USBTX, USBRX);

Ticker servo_ticker;
Ticker encoder_ticker;
PwmOut pin5(D5), pin6(D6);
DigitalIn encoder(D11);
volatile int steps;
volatile int last;

BBCar car(pin5, pin6, servo_ticker);
void encoder_control() {
   int value = encoder;
   if (!last && value) steps++;
   last = value;
}

int main() {
   pc.set_baud(9600);
   encoder_ticker.attach(&encoder_control, 10ms);
   steps = 0;
   last = 0;
   car.goStraight(-200);
   while(steps*6.5*3.14/32 < 50) {
      // printf("encoder = %d\r\n", steps);
      ThisThread::sleep_for(100ms);
   }
   car.stop();
   ThisThread::sleep_for(2s);
   steps = 0;
   last = 0;
   car.turn(-200,-0.1);
   while(steps*6.5*3.14/32 < 30) {
      // printf("encoder = %d\r\n", steps);
      ThisThread::sleep_for(100ms);
   }
   car.stop();
}