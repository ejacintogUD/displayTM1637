/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"

//Alias

//defines
#define escritura       0x40
#define poner_brillo    0x88
#define dir_display     0xC0

// prototipos
void condicion_start(void);
void condicion_stop (void);
void send_byte (char data);
void send_data (int number);
void counter_int(void); // Tarea del hilo 


//hilos
Thread T_counter(osPriorityNormal, 4096, NULL, NULL);

//variables globales 
char digit[4]={0,0,0,0};
int c = 0;

//pines 
DigitalOut  sclk (D2);
DigitalInOut dio (D3);
DigitalOut led(LED1);

const char digitToSegment[10] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D,0x07,0x7F,0x6F};
                       
int main()
{
    // Inicializar variables 
    c=0;

    //arrancar los hilos 
    T_counter.start(counter_int);
    printf("Arranque del programa\n\r");
    send_data(0);
    while (true) {
        led = !led;
        ThisThread::sleep_for(1s);
    }
}

void condicion_start(void)
{
 sclk = 1;
 dio.output();
 dio = 1;
 wait_us(1);
 dio = 0;
 sclk = 0;
}

void condicion_stop (void)
{
    sclk = 0;
    dio.output();
    dio = 0;
    wait_us(1);
    sclk = 1;
    dio = 1;
}

void send_byte(char data)
{
    dio.output();
    for (int i=0; i<8; i++)
    {
          sclk = 0;
          dio = (data&0x01) ? 1:0;
          data >>= 1;   
          sclk = 1;
    }
    // Esperar el ack
    dio.input();
    sclk = 0;
    wait_us(1);
    // Espera la señal de ACK
  

        if (dio==0) 
        {
            sclk=1;
            sclk=0;
        }
}

void send_data(int number)
{
    
    condicion_start();
    send_byte(escritura);
    condicion_stop();
    condicion_start();
    send_byte(dir_display);
    
    // Descomponer el número en dígitos y enviar al display
    
    for (int i = 0; i < 4; i++)
    {
        digit[i] = number % 10;
        number /= 10;
    }
    //Enviar los digitos desde el mas significativo al menos 
   for(int j=3; j> -1 ; j--)
    {
         send_byte(digitToSegment[digit[j]]); 
    }


    condicion_stop();
    condicion_start();
    send_byte(poner_brillo + 5);
    condicion_stop();

}

void counter_int(void)
{   
    while(true)
    {
    
     if (c == 9999) c=0;
     printf("%04u\n\r", c);
     send_data(c);
     c++;
     ThisThread::sleep_for(10ms);
    }
}
