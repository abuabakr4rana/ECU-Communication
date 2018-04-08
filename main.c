#include "init.h"
/********************************************************************
*                          global variables                         *
********************************************************************/

/********************************************************************
*                    _____  ___  ___   ___                          *
*                   |_   _|/ _ \|   \ / _ \                         *
*                     | | | (_) | |) | (_) |                        *
*                     |_|  \___/|___/ \___/                         *
*                                                                   *
* If you need global variables define them here!                    *
********************************************************************/  


int temperature;
int high_beam = 0b00000110;
int low_beam = 0b00000101;
int parking_lights = 0b00000011;
int parking_high_lights = 0b00000010;
int speed;
int gear, gear_ratio;
int rpm;
int headlight_to_transmit;
int ECU0_alive_signal = 1;

/********************************************************************
 *                              main                                *
 ********************************************************************/
void main(void)
{
    /* board initialization */
    Init();

    /* turn off leds */
    LED0 = 1;
    LED1 = 1;
    LED2 = 1;
    LED3 = 1;
    LED4 = 1;
    LED5 = 1;
    LED6 = 1;
    LED7 = 1;

    //gear = 0b00000100;
    //rpm = 234;

    PIT_ConfigureTimer(0, 200 );
    PIT_ConfigureTimer(1, 100 );
    PIT_StartTimer(0);
    PIT_StartTimer(1);

    /********************************************************************
    *                    _____  ___  ___   ___                          *
    *                   |_   _|/ _ \|   \ / _ \                         *
    *                     | | | (_) | |) | (_) |                        *
    *                     |_|  \___/|___/ \___/                         *
    *                                                                   *
    * Some configuration/initialisation must                            *
    * be done outside of the for-loop....(!)                            *
    ********************************************************************/  

    /* forever */
    for(;;)
    {
        /********************************************************************
        *                    _____  ___  ___   ___                          *
        *                   |_   _|/ _ \|   \ / _ \                         *
        *                     | | | (_) | |) | (_) |                        *
        *                     |_|  \___/|___/ \___/                         *
        *                                                                   *
        * Write down your logic here.                                       *
        ********************************************************************/  
    }
}

/********************************************************************
 *                      Interrupt Functions                         *
 ********************************************************************/  

void PITCHANNEL0(void)
{
    /* your own code above! */ // 200ms timer
    temperature = ADC_0.CDR[4].B.CDATA;
    
    CAN_0.BUF[0].DATA.B[0] = temperature;
    CAN_0.BUF[0].DATA.B[1] = temperature >> 8;

    CAN_0.BUF[0].CS.B.CODE = 12;
    //LED0 = ~LED0;

    // headlights
    if(SW1 == 0 && SW4 == 1){
        headlight_to_transmit = high_beam;
    LED1=0;
    LED4=1;
    LED2=1;
    LED3=1;
    LED7=1;
    }
    if(SW3 == 0){
        headlight_to_transmit = low_beam;
    LED2=0;
    LED1=1;
    LED4=1;
    LED3=1;
    LED7=1;
    }
    if(SW1 == 1 && SW4 == 0){
        headlight_to_transmit = parking_lights;
        LED3=0;
    LED1=1;
    LED2=1;
    LED4=1;
    LED7=1;
    }
    if(SW1 == 0 && SW4 == 0){
        headlight_to_transmit = parking_high_lights;
        LED4=0;
    LED1=1;
    LED2=1;
    LED3=1;
    LED7=1;
    }
    if(SW1 == 1 && SW3 == 1 && SW4 == 1){
        headlight_to_transmit = 0b00000111;
        LED7=0;
        LED1=1;
    LED2=1;
    LED3=1;
    LED4=1;
    }


    
    

    CAN_0.BUF[2].DATA.B[0] = headlight_to_transmit;

    CAN_0.BUF[2].CS.B.CODE = 12;    
    
    
    //alive signal
    CAN_0.BUF[3].CS.B.CODE = 12;

    //error signal
    if(ECU0_alive_signal){
        CAN_0.BUF[4].CS.B.CODE = 12;
        LED7=~LED7;
    }

    PIT.CH[0].TFLG.B.TIF = 1;
}

void PITCHANNEL1(void)
{
    /* your own code above! */  // 100ms timer
    
    switch (gear){
        case 0b00000001:
            gear_ratio = 7842;
            break;
        case 0b00000010:
            gear_ratio = 0;
            break;
        case 0b00000100:
            gear_ratio = 7842;
            break;
        case 0b00001000:
            gear_ratio = 13112;
            break;
        case 0b00010000:
            gear_ratio = 19861;
            break;
        case 0b00100000:
            gear_ratio = 27038;
            break;
        case 0b01000000:
            gear_ratio = 33149;
            break;
        case 0b10000000:
            gear_ratio = 40035;
            break;
        default:
            break;
    }

    speed  = (gear_ratio * rpm) / 100000 ;
    
    CAN_0.BUF[1].DATA.B[0] = speed;
    CAN_0.BUF[1].DATA.B[1] = speed >> 8;

    CAN_0.BUF[1].CS.B.CODE = 12;
    


    PIT.CH[1].TFLG.B.TIF = 1;
}

void CANMB0003(void)
{
/* No modifications needed here */
/* Receive interrupts are being cleared here */
    CAN_0.IFRL.B.BUF00I = 1;
    CAN_0.IFRL.B.BUF01I = 1;
    CAN_0.IFRL.B.BUF02I = 1;
    CAN_0.IFRL.B.BUF03I = 1;
}

void CANMB0407(void)
{
    /********************************************************************
    *                    _____  ___  ___   ___                          *
    *                   |_   _|/ _ \|   \ / _ \                         *
    *                     | | | (_) | |) | (_) |                        *
    *                     |_|  \___/|___/ \___/                         *
    *                                                                   *
    * CAN reception is handled here                                     *
    * The following buffers are important:                              *
    * CAN_0.RXFIFO.ID.B.STD_ID: ID of received message                  *
    * CAN_0.RXFIFO.DATA.B[i]: value of data byte 'i'                    *
    * IMPORTANT: check for the flag in CAN_0.IFRL.B.BUF05I first!       *
    ********************************************************************/  
    if(CAN_0.IFRL.B.BUF05I){
        switch (CAN_0.RXFIFO.ID.B.STD_ID){
            case 0x105:
                ECU0_alive_signal = 0;
                break;
            case 0x201: // rpm
                rpm = (CAN_0.RXFIFO.DATA.B[1] << 8 ) | CAN_0.RXFIFO.DATA.B[0];
                break;
            case 0x202: // gears
                gear = CAN_0.RXFIFO.DATA.B[0];
                break;
            default:
                break;
        }
    }
    
    /* end of own code! */
    /* clear flags as last step here! */
    /* don't change anything below! */
    CAN_0.IFRL.B.BUF04I = 1;
    CAN_0.IFRL.B.BUF05I = 1;
    CAN_0.IFRL.B.BUF06I = 1;
    CAN_0.IFRL.B.BUF07I = 1;
}

void CANMB0811(void)
{
/* No modifications needed here */
/* transmit interrupts are being cleared here */

    CAN_0.IFRL.B.BUF08I = 1;
    CAN_0.IFRL.B.BUF09I = 1;
    CAN_0.IFRL.B.BUF10I = 1;
    CAN_0.IFRL.B.BUF11I = 1;
}

void CANMB1215(void)
{
/* No modifications needed here */
/* transmit interrupts are being cleared here */
    CAN_0.IFRL.B.BUF12I = 1;
    CAN_0.IFRL.B.BUF13I = 1;
    CAN_0.IFRL.B.BUF14I = 1;
    CAN_0.IFRL.B.BUF15I = 1;
}



/********************************************************************
 *                   Interrupt Vector Table                         *
 ********************************************************************/
#pragma interrupt Ext_Isr
#pragma section IrqSect RX address=0x040
#pragma use_section IrqSect Ext_Isr

void Ext_Isr() {
    switch(INTC.IACKR.B.INTVEC)
    {
        case 59:
            PITCHANNEL0();
            break;
        case 60:
            PITCHANNEL1();
        case 68:
            CANMB0003();
            break;
        case 69:
            CANMB0407();
            break;
        case 70:
            CANMB0811();
            break;
        case 71:
            CANMB1215();
            break;        
        default:
            break;
    }
    /* End of Interrupt Request */
    INTC.EOIR.R = 0x00000000;
}
