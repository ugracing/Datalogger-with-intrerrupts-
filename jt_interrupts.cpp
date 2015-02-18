/*
jon trinder
jont@ninelocks.com 2015
 
Megasquirt test and debugging code for GU
 
 
*/
#include "mbed.h"
#include "jtlcd.h"
#include "Counter.h"
 
 
float coolant;
int int_coolant = 0;
void callback_reed(void);
void siphon_data();
 
DigitalOut myled(LED1);
DigitalOut myled2(LED2);
DigitalOut myled3(LED3);
DigitalOut myled4(LED4);
 
 
DigitalIn reed(p9); //would default to in but we wanna play with its mode
Counter counter_reed(p9, callback_reed ); //setup the counter to use pin 9
 
void sioRXHandler(void);
#define combuffsize  256  
volatile char comBuff[combuffsize];   /* space for commands yeh its small make bigger if you need to */
volatile int comBuffIndex;           /* buffer index       */
 
char lineBuffer[32]; //for saving data for the lcd display
 
Serial pc(USBTX, USBRX); // tx, rx
 
/*=====================================================================================*/
//
// callback funtions mostly for the counters
//
/*=====================================================================================*/
/*=====================================================================================*/
// callback for the reed switch input
// puts logentry into the ringbuffer
/*=====================================================================================*/
void callback_reed()
{
    time_t seconds = time(NULL);    
    //LPC_RTC->GPREG3 = counter_reed.read();
    __disable_irq();    // Disable Interrupts
    comBuffIndex = 0;
    __enable_irq();
    pc.putc('A');
 
}
 
void serialInit(void)
{  
    pc.attach(&sioRXHandler);
}
 
 
/*======================================================================*/
/* sioRXHandler               */
/*======================================================================*/
/*
This is inefficient and long handed but makes explaining it to someone
easier.
 
Which is the point, to be understandable at this stage :-)
 
 
If more data received than buffer length we throw it away!
*/
void sioRXHandler(void)
{
char theChar = 0x00;
myled2 = 1;
myled3 = 1;
    while (pc.readable()){
        theChar = pc.getc();
        pc.putc(theChar); //echo it back 
        if (comBuffIndex >= combuffsize - 1)
        {
            comBuffIndex = combuffsize - 1;
        }
        comBuff[comBuffIndex] = theChar;
        comBuffIndex++;
  }
  myled2 = 0;
}
 
/*======================================================================*/
/* Main            */
/*======================================================================*/
int main() {
     setbuf(stdout, NULL);
    // no buffering for this filehandle, this stop the weird
    //behaviour with things not being dfiplaye untill a character was received.
    serialInit();
    reed.mode(PullUp);   
    
    LcdInit();
    LcdHomeTop();
    LcdWriteTextLine("Squirt Tester");
    LcdHomeBottom();
    LcdWriteTextLine("Hello World");
    while(1) {
         siphon_data();
        myled = 1;
        wait(0.2);
        myled = 0;
        wait(0.2);
        LcdHomeBottom();
        sprintf(lineBuffer,"%.3f %04X %3d", coolant, int_coolant, counter_reed.read());
        LcdWriteTextLine(lineBuffer);
    }
}
 
void siphon_data(){
__disable_irq();    // Disable Interrupts
 
int_coolant = 0;
    if(comBuffIndex > 24){     
       int_coolant = comBuff[23];
       int_coolant <<= 8;
       int_coolant |= comBuff[22];
       coolant = (int_coolant-320) * 0.05555;
    }
__enable_irq();   
     
}
