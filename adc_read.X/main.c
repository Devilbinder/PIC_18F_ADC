#include <xc.h>
#include <p18f4520.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "conbits.h"
#include "uart_layer.h"

#define ADC_RES (5.0/1023.0)

uint8_t data = 0;
bool got_data_bool = false;
uint8_t print_buffer[256] = {0};

uint16_t adc_to_led = 0;

float adc_volt = 0.0;

void interrupt high_isr(void);
void interrupt low_priority low_isr(void);

void main(void) {

    OSCCONbits.IRCF = 0x07;
    OSCCONbits.SCS = 0x03;
    while(OSCCONbits.IOFS!=1);
    
    TRISB=0;
    LATB=0;
    uart_init(51,0,1,0);//baud 9600
    
    TRISAbits.RA0 = 1;
    
    ADCON1bits.VCFG0 = 0;
    ADCON1bits.VCFG1 = 0;
    ADCON1bits.PCFG = 0x0E;
    
    ADCON2bits.ADFM = 1;
    ADCON2bits.ACQT = 0x04;
    ADCON2bits.ADCS = 0x04;
    
    ADCON0bits.CHS = 0;
    ADCON0bits.ADON = 1;
    
    RCONbits.IPEN = 1;
    INTCONbits.GIEH = 1; 
    INTCONbits.GIEL = 1;
    
    while(1){
        ADCON0bits.GODONE = 1;
        while(ADCON0bits.GODONE);
        sprintf(print_buffer,"\rADC Read: 0x%02x%02x\t",ADRESH,ADRESL);
        uart_send_string(print_buffer);
        
        adc_to_led = (ADRESH << 6) | (ADRESL >> 2);
        LATB = adc_to_led;
       
        adc_to_led = (ADRESH << 8) | (ADRESL);
        
        adc_volt = adc_to_led * ADC_RES;
        sprintf(print_buffer,"ADC Volt: %.2f\t",adc_volt);
        uart_send_string(print_buffer);
       
    } 
}

void interrupt high_isr(void){
    INTCONbits.GIEH = 0;
    if(PIR1bits.RCIF){
        uart_receiver(&data,&got_data_bool);
       PIR1bits.RCIF=0;
    }
    
    INTCONbits.GIEH = 1;
}

void interrupt low_priority low_isr(void){
    INTCONbits.GIEH = 0;
    
    INTCONbits.GIEH = 1;
}



