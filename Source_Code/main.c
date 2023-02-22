/*******************************************************
This program was created by the
CodeWizardAVR V3.12 Advanced
Automatic Program Generator
© Copyright 1998-2014 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : Digital Clock
Version : V1
Date    : 2/6/2023
Author  : phamhiep027081
Company : HUST
Comments:


Chip type               : ATmega16
Program type            : Application
AVR Core Clock frequency: 1.000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 256
*******************************************************/

#include <mega16.h>

// I2C Bus functions
#include <i2c.h>

// DS1307 Real Time Clock functions
#include <ds1307.h>

// Alphanumeric LCD functions
#include <alcd.h>

// Declare your global variables here
#include <delay.h>
#include <stdint.h>
#include <stdio.h>

#define DOWN PINB.0
#define MODE PINB.1
#define UP PINB.2
uint8_t str[16];
uint8_t index;
uint8_t* days[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
uint8_t maxDay[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

void BUTTON_HANDLE();
void CLOCK_HANDLE();

void RTC_Display();
void RTC_Blink();
void Up_Value(uint8_t *value, uint8_t max, uint8_t min);
void Down_Value(uint8_t *value, uint8_t max, uint8_t min);
uint8_t Max_Day(void);
uint16_t Get_dayofweek(uint16_t d, uint16_t m, uint16_t y);

typedef struct{
    uint8_t second;
    uint8_t min;
    uint8_t hour;
    uint8_t day_of_week;
    uint8_t date;
    uint8_t month;
    uint8_t year;
}DateTime;
typedef enum{
    NORMAL,
    SETTING_HOUR,
    SETTING_MIN,
    SETTING_DATE,
    SETTING_MON,
    SETTING_YEAR,
}State;
DateTime date_time;
State clock_state = NORMAL;
void BUTTON_HANDLE(){
    delay_ms(1);
    if(index == 0){
        clock_state = NORMAL;
    }
    if(MODE == 0){
        index++;
        switch(index){
        case 1:
            clock_state = SETTING_HOUR;
            break;
        case 2:
            clock_state = SETTING_MIN;
            break;
        case 3:
            clock_state = SETTING_DATE;
            break;
        case 4:
            clock_state = SETTING_MON;
            break;
        case 5:
            clock_state = SETTING_YEAR;
            break;
        default:
            break;
        }
        if(index > 5){
            index = 0;
        }
        while(MODE == 0);
    }
}

void Up_Value(uint8_t *value, uint8_t max, uint8_t min){
    (*value)++;
    if((*value) >= max){
        (*value) = min;
    }
}
void Down_Value(uint8_t *value, uint8_t max, uint8_t min){
    (*value)--;
    if((*value) <= min){
        (*value) = max - 1;
    }
}
uint16_t Get_dayofweek(uint16_t d, uint16_t m, uint16_t y){
    static uint16_t t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    y -= m < 3;
    return ( y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;
}
void RTC_Display(){
    lcd_gotoxy(4, 0);
    sprintf(str, "%02d:%02d:%02d", date_time.hour, date_time.min, date_time.second);
    lcd_puts(str);

    lcd_gotoxy(1, 1);
    sprintf(str, "%3s %02d/%02d/20%02d", days[date_time.day_of_week - 1], date_time.date, date_time.month, date_time.year);
    lcd_puts(str);
}
void CLOCK_HANDLE(){
    switch(clock_state){
        case NORMAL:
            RTC_Display();
            break;
        case SETTING_HOUR:
        case SETTING_MIN:
        case SETTING_DATE:
        case SETTING_MON:
        case SETTING_YEAR:
            RTC_Blink();
            break;
        default:
            break;
    }
}
void RTC_Blink(){
//    lcd_gotoxy(1, 1);
//    sprintf(str, "%3s", days[date_time.day_of_week - 1]);
//    lcd_puts(str);
    uint8_t max_day;
    switch(clock_state){
        case SETTING_HOUR:
            lcd_gotoxy(4, 0);
            lcd_puts("  ");
            delay_ms(10);
            if(UP == 0){
                Up_Value(&date_time.hour, 24, 0);
                rtc_set_time(date_time.hour, date_time.min, date_time.second);
                while(UP == 0);
            }else if(DOWN == 0){
                Down_Value(&date_time.hour, 24, 0);
                rtc_set_time(date_time.hour, date_time.min, date_time.second);
                while(DOWN == 0);
            }
            break;
        case SETTING_MIN:
            lcd_gotoxy(7, 0);
            lcd_puts("  ");
            delay_ms(10);
            if(UP == 0){
                Up_Value(&date_time.min, 60, 0);
                rtc_set_time(date_time.hour, date_time.min, date_time.second);
                while(UP == 0);
            }else if(DOWN == 0){
                Down_Value(&date_time.min, 60, 0);
                rtc_set_time(date_time.hour, date_time.min, date_time.second);
                while(DOWN == 0);
            }
            break;
        case SETTING_DATE:
            lcd_gotoxy(5, 1);
            lcd_puts("  ");
            delay_ms(10);
            max_day = Max_Day();
            if(UP == 0){
                Up_Value(&date_time.date, max_day + 1, 1);
                date_time.day_of_week = Get_dayofweek(date_time.date, date_time.month, date_time.year + 2000) + 1;
                rtc_set_date(date_time.day_of_week, date_time.date, date_time.month, date_time.year);
                while(UP == 0);
            }else if(DOWN == 0){
                Down_Value(&date_time.date, max_day + 1, 1);
                date_time.day_of_week = Get_dayofweek(date_time.date, date_time.month, date_time.year + 2000) + 1;
                rtc_set_date(date_time.day_of_week, date_time.date, date_time.month, date_time.year);
                while(DOWN == 0);
            }
            break;
        case SETTING_MON:
            lcd_gotoxy(8, 1);
            lcd_puts("  ");
            delay_ms(10);
            if(UP == 0){
                Up_Value(&date_time.month, 13, 1);
                date_time.day_of_week = Get_dayofweek(date_time.date, date_time.month, date_time.year + 2000) + 1;
                rtc_set_date(date_time.day_of_week, date_time.date, date_time.month, date_time.year);
                while(UP == 0);
            }else if(DOWN == 0){
                Down_Value(&date_time.month, 13, 1);
                date_time.day_of_week = Get_dayofweek(date_time.date, date_time.month, date_time.year + 2000) + 1;
                rtc_set_date(date_time.day_of_week, date_time.date, date_time.month, date_time.year);
                while(DOWN == 0);
            }
            break;
        case SETTING_YEAR:
            lcd_gotoxy(13, 1);
            lcd_puts("  ");
            delay_ms(10);
            if(UP == 0){
                Up_Value(&date_time.year, 99, 1);
                date_time.day_of_week = Get_dayofweek(date_time.date, date_time.month, date_time.year + 2000) + 1;
                rtc_set_date(date_time.day_of_week, date_time.date, date_time.month, date_time.year);
                while(UP == 0);
            }else if(DOWN == 0){
                Down_Value(&date_time.year, 99, 1);
                date_time.day_of_week = Get_dayofweek(date_time.date, date_time.month, date_time.year + 2000) + 1;
                rtc_set_date(date_time.day_of_week, date_time.date, date_time.month, date_time.year);
                while(DOWN == 0);
            }
            break;
        default:
            break;
    }
}
uint8_t Max_Day(void){
	uint8_t MAX_DAY;
	if((date_time.year + 2000) % 4 == 0){
		MAX_DAY = 29;
	}else{
		MAX_DAY = maxDay[date_time.month - 1];
	}
	return MAX_DAY;
}
void main(void)
{
// Declare your local variables here

// Input/Output Ports initialization
// Port A initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In
DDRA=(0<<DDA7) | (0<<DDA6) | (0<<DDA5) | (0<<DDA4) | (0<<DDA3) | (0<<DDA2) | (0<<DDA1) | (0<<DDA0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T
PORTA=(0<<PORTA7) | (0<<PORTA6) | (0<<PORTA5) | (0<<PORTA4) | (0<<PORTA3) | (0<<PORTA2) | (0<<PORTA1) | (0<<PORTA0);

// Port B initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In
DDRB=(0<<DDB7) | (0<<DDB6) | (0<<DDB5) | (0<<DDB4) | (0<<DDB3) | (0<<DDB2) | (0<<DDB1) | (0<<DDB0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=P Bit1=P Bit0=P
PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (1<<PORTB2) | (1<<PORTB1) | (1<<PORTB0);

// Port C initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In
DDRC=(0<<DDC7) | (0<<DDC6) | (0<<DDC5) | (0<<DDC4) | (0<<DDC3) | (0<<DDC2) | (0<<DDC1) | (0<<DDC0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T
PORTC=(0<<PORTC7) | (0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (0<<PORTC1) | (0<<PORTC0);

// Port D initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In
DDRD=(0<<DDD7) | (0<<DDD6) | (0<<DDD5) | (0<<DDD4) | (0<<DDD3) | (0<<DDD2) | (0<<DDD1) | (0<<DDD0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T
PORTD=(0<<PORTD7) | (0<<PORTD6) | (0<<PORTD5) | (0<<PORTD4) | (0<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: Timer 0 Stopped
// Mode: Normal top=0xFF
// OC0 output: Disconnected
TCCR0=(0<<WGM00) | (0<<COM01) | (0<<COM00) | (0<<WGM01) | (0<<CS02) | (0<<CS01) | (0<<CS00);
TCNT0=0x00;
OCR0=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: Timer1 Stopped
// Mode: Normal top=0xFFFF
// OC1A output: Disconnected
// OC1B output: Disconnected
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
TCCR1A=(0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<WGM11) | (0<<WGM10);
TCCR1B=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (0<<WGM12) | (0<<CS12) | (0<<CS11) | (0<<CS10);
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;

// Timer/Counter 2 initialization
// Clock source: System Clock
// Clock value: Timer2 Stopped
// Mode: Normal top=0xFF
// OC2 output: Disconnected
ASSR=0<<AS2;
TCCR2=(0<<PWM2) | (0<<COM21) | (0<<COM20) | (0<<CTC2) | (0<<CS22) | (0<<CS21) | (0<<CS20);
TCNT2=0x00;
OCR2=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=(0<<OCIE2) | (0<<TOIE2) | (0<<TICIE1) | (0<<OCIE1A) | (0<<OCIE1B) | (0<<TOIE1) | (0<<OCIE0) | (0<<TOIE0);

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
// INT2: Off
MCUCR=(0<<ISC11) | (0<<ISC10) | (0<<ISC01) | (0<<ISC00);
MCUCSR=(0<<ISC2);

// USART initialization
// USART disabled
UCSRB=(0<<RXCIE) | (0<<TXCIE) | (0<<UDRIE) | (0<<RXEN) | (0<<TXEN) | (0<<UCSZ2) | (0<<RXB8) | (0<<TXB8);

// Analog Comparator initialization
// Analog Comparator: Off
// The Analog Comparator's positive input is
// connected to the AIN0 pin
// The Analog Comparator's negative input is
// connected to the AIN1 pin
ACSR=(1<<ACD) | (0<<ACBG) | (0<<ACO) | (0<<ACI) | (0<<ACIE) | (0<<ACIC) | (0<<ACIS1) | (0<<ACIS0);
SFIOR=(0<<ACME);

// ADC initialization
// ADC disabled
ADCSRA=(0<<ADEN) | (0<<ADSC) | (0<<ADATE) | (0<<ADIF) | (0<<ADIE) | (0<<ADPS2) | (0<<ADPS1) | (0<<ADPS0);

// SPI initialization
// SPI disabled
SPCR=(0<<SPIE) | (0<<SPE) | (0<<DORD) | (0<<MSTR) | (0<<CPOL) | (0<<CPHA) | (0<<SPR1) | (0<<SPR0);

// TWI initialization
// TWI disabled
TWCR=(0<<TWEA) | (0<<TWSTA) | (0<<TWSTO) | (0<<TWEN) | (0<<TWIE);

// Bit-Banged I2C Bus initialization
// I2C Port: PORTC
// I2C SDA bit: 1
// I2C SCL bit: 0
// Bit Rate: 100 kHz
// Note: I2C settings are specified in the
// Project|Configure|C Compiler|Libraries|I2C menu.
i2c_init();

// DS1307 Real Time Clock initialization
// Square wave output on pin SQW/OUT: On
// Square wave frequency: 1Hz
rtc_init(0,1,0);

// Alphanumeric LCD initialization
// Connections are specified in the
// Project|Configure|C Compiler|Libraries|Alphanumeric LCD menu:
// RS - PORTD Bit 6
// RD - PORTD Bit 5
// EN - PORTD Bit 7
// D4 - PORTC Bit 4
// D5 - PORTC Bit 5
// D6 - PORTC Bit 6
// D7 - PORTC Bit 7
// Characters/line: 16
lcd_init(16);
lcd_gotoxy(2, 0);
lcd_puts("DIGITAL CLOCK");
delay_ms(1000);
lcd_clear();
//rtc_set_time(14, 2, 30);
//rtc_set_date(2, 6, 2, 23);
while (1)
      {
      // Place your code here
          RTC_Display();
          rtc_get_time(&date_time.hour, &date_time.min, &date_time.second);
          rtc_get_date(&date_time.day_of_week, &date_time.date, &date_time.month, &date_time.year);
          CLOCK_HANDLE();
          BUTTON_HANDLE();
      }
}

