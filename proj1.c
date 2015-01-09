#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define F_CPU 8000000UL
#include <util/delay.h>

#include "uart.h"
#include "spi.h"
#include "nrf24.h"

void delay_by_100ms(uint8_t);
void send_data(uint8_t); 

ISR(PCINT1_vect)       // interrupt service routine
{
   printf("Interrupt!\n");
   delay_by_100ms((uint8_t) 3);
   if(PINC & (1<<PINC5)){
     send_data((uint8_t) 1);
     printf("Send 1\n");
   }else{
     send_data((uint8_t) 0);
     printf("Send 0\n");
   }  
   return;
}


void SystemInit(void)
{
    PCMSK1 |= (1<<PCINT13); // pin change mask: listen to portb bit 2
    PCICR  |= (1<<PCIE1);
    sei();      // enable all interrupts
}

//void send_data(uint8_t* value) 
void send_data(uint8_t value) 
{    
    uint8_t temp;
    uint8_t data_array[8];
    
    data_array[0] = value; 
    data_array[1] = 0xAA;
    data_array[2] = 0x55;
    data_array[3] = 0xAA;
    data_array[4] = 0x55;
    data_array[5] = 0xAA;
    data_array[6] = 0x55;
    data_array[7] = 0xAA;
    /* Automatically goes to TX mode */
    nrf24_send(data_array);        

    /* Wait for transmission to end */
    while(nrf24_isSending());

    /* Make analysis on last tranmission attempt */
    temp = nrf24_lastMessageStatus();

    if(temp == NRF24_TRANSMISSON_OK)
    {                    
        printf("\n> Tranmission went OK\r\n");
    }
    else if(temp == NRF24_MESSAGE_LOST)
    {                    
        printf("> Message is lost ...\r\n");    
    }

    /* Retranmission count indicates the tranmission quality */
    temp = nrf24_retransmissionCount();
    printf("> Retranmission count: %d\r\n",temp);

    /* Optionally, go back to RX mode ... */
    //nrf24_powerUpRx();

    /* Or you might want to power down after TX */
     nrf24_powerDown();            
}



int main(void) {    
    int led      = PINB0; // PortB 0
    int rf24_csn = PIND7;
    int rf24_ce  = PIND6;

    uint8_t data_array[8];
    uint8_t tx_address[5] = {0xE1,0xF0,0xF0,0xF0,0xF0};
    uint8_t rx_address[5] = {0xE1,0xF0,0xF0,0xF0,0xF0};
    uint8_t temp;
    uint8_t q = 0;
    uint8_t stat;
   
    DDRB |= (1<<led);

    DDRC &= ~(1<<PINC5);
    PORTC |= (1<<PINC5); 
    
    SystemInit();


    uart_init();
    stdout = &uart_output;
    stdin  = &uart_input;
                              
    setup_spi(SPI_MODE_0, SPI_MSB, SPI_NO_INTERRUPT, SPI_MSTR_CLK16);
    
    /* init hardware pins */
    nrf24_init();

    /* Channel #2 , payload length: 4 */
    nrf24_config(10,8);
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);

    /* Set the device addresses */
    nrf24_tx_address(tx_address);
    nrf24_rx_address(rx_address);    

    nrf24_powerDown();            


    while(1) {
    //    send_data();
    //    putchar('.');
    
       // delay_by_100ms((uint8_t) 30);
    sleep_mode();

  }
                              
    return 0;
}

void delay_by_100ms(uint8_t cnt){
    uint8_t i;
    for(i=0;i<cnt;i++)
    {
       _delay_ms(10);
       _delay_ms(10);
       _delay_ms(10);
       _delay_ms(10);
       _delay_ms(10);
       _delay_ms(10);
       _delay_ms(10);
       _delay_ms(10);
       _delay_ms(10);
       _delay_ms(10);
    }
}    


