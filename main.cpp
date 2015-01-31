#include "mbed.h"
#include <stdio.h>
#include <string>

Serial pc(USBTX, USBRX); // tx, rx
Serial msquirt(p9, p10); // tx, rx

Timer t;

int byte_count;
string data;
bool fetch;
int count;

typedef struct car_data{
    float RPM;
    float map;
    float mat;
    float coolant;
    float throttle;
    float battery;
    float air_fuel_1;
    float air_fuel_2;
    bool engine_status[8];
    bool injectors_status[7];
    
} car_data;

void rx_data()
{   count++;
    car_data car_data;
    while (msquirt.readable())
    {
        data += msquirt.getc();
        byte_count++;
        //pc.printf("\r\n   %d" , byte_count);
        
        if (!msquirt.readable())
        {
            wait(0.001);
        }
    }
    wait(0.001);
    union
    {
        uint8_t b[2];
        int i;
    };
    
    b[1] = data[22]; b[0] = data[23];
    car_data.coolant = (i-320) * 0.05555;
    pc.printf("CLT = %f \r\n",car_data.coolant);
    
    pc.printf("got %d bytes\r\n", byte_count);
    
    data = "";
    byte_count = 0;
    pc.printf("set to zer mofofoker");
    fetch = true;
    pc.printf("fetch set true");
    //wait(0.2);
}

void log_data()
{
    //log to usb stick
}


void drive_dash()
{
    //drive leds etc
}


int main()
{
    msquirt.baud(38400);
    pc.baud(57600);
    msquirt.attach(&rx_data, Serial::RxIrq);
    
    fetch = true;
    t.start();
    while(1)
    {   wait(0.01);
        if(fetch)
        {
            printf("TIME IS: %f \r\n seconds", t.read());
            msquirt.printf("A");
            fetch = false;
        }
        else if (!fetch)
        {
            count++;
        }
    }
}