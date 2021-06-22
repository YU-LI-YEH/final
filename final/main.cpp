#include"mbed.h"
#include "bbcar.h"

BufferedSerial pc(USBTX,USBRX);
BufferedSerial uart(D1,D0); 
BufferedSerial xbee(D10, D9);

DigitalIn encoder(D11);
DigitalInOut ping(D12);

BBCar car(pin5, pin6, servo_ticker);
PwmOut pin5(D5), pin6(D6);

Ticker servo_ticker;
Ticker encoder_ticker;

volatile int steps;
volatile int last;
float val;
Timer t;
Thread t1, t2, t3, t4;

char x;
int num[4] = {0, 0, 0, 0};
int i= 0, start = 0;
int c[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char buffer[10];
char pre;
int k;
int task = 0, pre_task = 0;

void encoder_control() {
   int value = encoder;
   if (!last && value) steps++;
   last = value;
}

void PING(){
    while(1){
        printf("4\n");
        ping.output();
        ping = 0; wait_us(200);
        ping = 1; wait_us(5);
        ping = 0; wait_us(5);

        ping.input();
        while(ping.read() == 0);
        t.start();
        while(ping.read() == 1);
        val = t.read()*17700.4f;
        sprintf(buffer, "%f", val);
        xbee.write(buffer, sizeof(buffer));
        printf("Ping = %lf\r\n", val);
        t.stop();
        t.reset(); 
    }
}

void task1(){
    while(1){
        printf("1\n");
        int xd = (c[0] + c[1])/2;
        if(task == 0){
            if(xd >= 70) 
               car.turn(300,0.1);
            else if(xd <= -50) 
               car.turn(300,-0.1);
            else
               car.goStraight(200);
        }
    }
}
void task2(){
    while(1){
        printf("2\n");
        if(task == 1){

            steps = 0;
            last = 0;
            car.turn(100,0.4);
            while(steps*6.5*3.14/32 < 90) {
                ThisThread::sleep_for(100ms);
            }
            car.stop();

            task = 1;
            xbee.write("task 2", 4);
            pre_task = 1;

        }
    }
}

void task3(){
    while(1){
        printf("3\n");
        if(task == 3){
           c[6] = -400;
           task = 0;
            steps = 0;
            last = 0;
            car.turn(40,-0.1);
            car.stop();
            
            xbee.write("task 4", 4);
            pre_task = 2;
            

        }
    }
}


int main(){
   
   t1.start(task1);
    t2.start(task2);
    t3.start(task3);
    t4.start(PING);
   
    encoder_ticker.attach(&encoder_control, 10ms);
    pc.set_baud(9600);
    uart.set_baud(9600);
    xbee.set_baud(9600);
    xbee.write("task 0",4);

    while(1){
        
        if(uart.readable()){
            char recv[1];
            uart.read(recv, sizeof(recv));
            pre = x;
            x = *recv;
            if(start == 1 && x!= '.'){
                num[i] = int(x)-48;
                i++;
            }
            if(x == ':'){
                xbee.write("+++",3);
                start = 1;
         
            }
            if(x == '.'){
                start = 0;
                if(num[0] != -3){
                    if(i == 1) 
                       c[k] = num[0];
                    else if(i == 2) 
                       c[k] = 10*num[0] + num[1];
                    else if(i == 3) 
                       c[k] = 100*num[0] + 10*num[1] + num[2];
            
                   num[0] = 0; 
                   num[1] = 0; 
                   num[2] = 0; 
                   num[3] = 0;
                   i = 0;
                }
                else{
                    if(i == 2) 
                       c[k] = num[1];
                    else if(i == 3) 
                       c[k] = 10*num[1] + num[2];
                    else if(i == 4) 
                       c[k] = 100*num[1] + 10*num[2] + num[3];
                    c[k] = c[k] *(-1);
                    printf("C%d: %d\n",k,c[k]);
                   if(c[6] > -5 && task == 0)
                     {task = 1; xbee.write("task 1",4);}
                   
                   num[0] = 0; 
                   num[1] = 0; 
                   num[2] = 0; 
                   num[3] = 0;
                   i = 0;
                } 
            }
  
        }
   }
}
