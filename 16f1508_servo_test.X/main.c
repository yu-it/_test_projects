/* 
 * File:   main.c
 * Author: yuusuke.ito
 *
 * Created on 2017/04/23, 11:49
 * 1  --VDD         |  20 VSS
 * 2  --RA5 SWT     |  19 RA0
 * 3  --RA4 SWT     |  18 RA1
 * 4  --RA3         |  17 RA2 srv3  
 * 5  --RC5 mor1_1  |  16 RC0 srv2
 * 6  --RC4 srv1    |  15 RC1 devug
 * 7  --RC3 pwm3    |  14 RC2 
 * 8  --RC6         |  13 RB4 
 * 9  --RC7         |  12 RB5 SWT
 * 10 --RB7 PWM Con |  11 RB6 SWT
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <pic16F1508.h>
/***** ?????????????? *********/
/*
__CONFIG(FOSC_INTOSC & WDTE_OFF & PWRTE_ON & MCLRE_ON & CP_OFF
	& BOREN_ON & CLKOUTEN_OFF & IESO_OFF & FCMEN_OFF);
__CONFIG(WRT_OFF & STVREN_OFF & LVP_OFF);
*/
// ???????????????
#pragma config FOSC = INTOSC    // Oscillator Selection Bits (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable (PWRT enabled)
#pragma config MCLRE = OFF       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF       // Internal/External Switchover Mode (Internal/External Switchover Mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config STVREN = OFF     // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will not cause a Reset) aa
#pragma config BORV = HI        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)aaa
#pragma config LVP = OFF        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)
#pragma config LPBOR = OFF      // Low-Power Brown Out Reset (Low-Power BOR is disabled) zzz

/** ??????????? **/
unsigned int HWidth, VWidth, DFlag, Mode, i;
unsigned int Interval, AutoFlag, AutoInt, Level, NewLvl;
const unsigned int MAX = 1000;
const unsigned int MIN = 250;
//#define  _XTAL_FREQ     8000000      // ???????500kHz
unsigned int current_1, current_4;
void CLCInit(void);
/*
 * 
 */

void Init() {
    OSCCON = 0x73;                      // ??8MHz
    /** ????????? ***/
    ANSELA = 0x00;
    ANSELB = 0x00;
    
    TRISA = 0;                       // ?????
    TRISB = 0;                       // ???????
    ANSELC = 0x00;
    TRISC = 0;                       // ?????

    TRISAbits.TRISA4 = 1;
    TRISAbits.TRISA5 = 1;    
    TRISBbits.TRISB5 = 1;
    TRISBbits.TRISB6 = 1;    
    WPUA = 0b00110000;
    WPUB = 0b01100000;


    /* ???0????20.04msec?? */
    OPTION_REG = 0x07;                  // Int. 1/256, ????????
    TMR0 = 99;                          // 20msec         
    /* PWM1,4????? 2.048msec??*/
    /* Duty?????  450 to 1023 (0.9ms to 2.05ms width) */
    
    PWM1CON = 0x80;                     // PWM1?? Output??
    PWM4CON = 0x80;                     // PWM4?? Output??
    PWM3CON = 0x80;                     // PWM3?? OutputOn
    PWM3CONbits.PWM3OE = 1;
    //PWM4CONbits.PWM4OE = 1;
    
    /* ??????? */
    T2CON = 0x06;                       // 1/16  2MHz/16/256 = 2.048msec??
    PR2 = 0xFF;                         // 10bit???
    /** CLC?????  **/
    CLCInit();
     /* ?????? */
    /* ?????? */
    TMR0IE = 1;                         // ???0??????
    //TMR2IE = 1;                         // ???0??????
    PEIE = 1;                           // ????
    GIE = 1;                            // ???????
}
int main(int argc, char** argv) {

    Init();
    int pwm1 = 500;
    int pwm3 = 500;
    int pwm4 = 500;
    int delta = 50;
    LATCbits.LATC2 = 1;
    int flg = 0;
    while(1) {
        if (PORTAbits.RA4 + PORTAbits.RA5 + PORTBbits.RB5 + PORTBbits.RB6 == 4) {
            flg = 0;
        }
        if (flg) {
            LATCbits.LATC2 = 0;
            
            //continue;
        }
        if (PORTAbits.RA4 == 0) {
            pwm1 += delta;
            pwm3 += delta;
            pwm4 += delta;
            //LATCbits.LATC2 = 0;
            
        }else if (PORTAbits.RA5 == 0) {
            pwm1 -= delta;
            pwm3 -= delta;
            pwm4 -= delta;
            
        }else if (PORTBbits.RB5 == 0) {
            pwm1 = 350;
            pwm3 = 350;
            pwm4 = 350;
            
        }else if (PORTBbits.RB6 == 0) {
            pwm1 = 1000;
            pwm3 = 1000;
            pwm4 = 1000;
            
        };

        LATCbits.LATC2 = 1;
        PWM4DCH = pwm4 >> 2;
        PWM4DCL = pwm4 << 6;
        PWM1DCH = pwm1 >> 2;
        PWM1DCL = pwm1 << 6;

        PWM3DCH = pwm3 >> 2;
        PWM3DCL = pwm3 << 6;
        flg = 1;
    }
}

/*************************************
* CLC??????
*************************************/
void CLCInit(void){
   /* Timer0?20msec???PWM3?PWM4????1?????? */
    /* CLC1 ???? */
    CLC1GLS0 = 0x08;
    CLC1GLS1 = 0x00;                    // RS????????
    CLC1GLS2 = 0x02;                    // Timer0????
    CLC1GLS3 = 0x00;                    // Timer2?????
    CLC1SEL0 = 0x17;
    CLC1SEL1 = 0x60;
    CLC1POL  = 0x00;
    //CLC1CON  = 0xC3;
    CLC1CON  = 0x83;
    CLC3GLS0 = 0x02;
    CLC3GLS1 = 0x08;                    // D???????????
    CLC3GLS2 = 0x00;                    // CLC1????D??
    CLC3GLS3 = 0x00;                    // Timer2?????
    CLC3SEL0 = 0x47;
    CLC3SEL1 = 0x50;
    CLC3POL  = 0x00;
    CLC3CON  = 0xC4;
    
    CLC2GLS0 = 0x08;
    CLC2GLS1 = 0x20;                    // AND??
    CLC2GLS2 = 0x00;                    // CLC3???PWM1???AND
    CLC2GLS3 = 0x00;
    CLC2SEL0 = 0x62;
    CLC2SEL1 = 0x66;
    CLC2POL  = 0x0C;
    CLC2CON  = 0xC2;

    CLC4GLS0 = 0x08;
    CLC4GLS1 = 0x80;                    // AND??
    CLC4GLS2 = 0x00;                    // CLC3???PWM4???AND
    CLC4GLS3 = 0x00;
    CLC4SEL0 = 0x62;
    CLC4SEL1 = 0x37;
    CLC4POL  = 0x0C;
    CLC4CON  = 0xC2;
}

void interrupt isr(void){
    if(TMR0IF){                             // ???0??????
        TMR0IF = 0;                         // ??????
        TMR0 = 96;                          // ?????
    }
    
}
