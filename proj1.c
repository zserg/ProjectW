#include <stdio.h>
#include <avr/io.h>

#define F_CPU 8000000UL
#include <util/delay.h>

#include "uart.h"
#include "spi.h"
#include "nrf24.h"

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

    uart_init();
    stdout = &uart_output;
    stdin  = &uart_input;
                              
    setup_spi(SPI_MODE_0, SPI_MSB, SPI_NO_INTERRUPT, SPI_MSTR_CLK16);
    
    /* init hardware pins */
    nrf24_init();

    /* Channel #2 , payload length: 4 */
    nrf24_config(10,8);

    /* Set the device addresses */
    nrf24_tx_address(tx_address);
    nrf24_rx_address(rx_address);    



    nrf24_readRegister(TX_ADDR,&stat,1);
    printf("## TX_ADDR=%x",stat);
    nrf24_readRegister(RX_ADDR_P0,&stat,1);
    printf("## RX_ADDR_P0=%x",stat);
    nrf24_readRegister(CONFIG,&stat,1);
    printf("## CONFIG=%x",stat);
    nrf24_readRegister(RF_CH,&stat,1);
    printf("## RF_CH=%x",stat);
    nrf24_readRegister(RF_SETUP,&stat,1);
    printf("## RF_SETUP=%x\n",stat);

    while(1) {
        puts("Hello, world!");
//        printf("You wrote %c\n", input);        
    /* Fill the data buffer */
    data_array[0] = 0x00;
    data_array[1] = 0xAA;
    data_array[2] = 0x55;
    data_array[3] = q++;                                    
    data_array[4] = q++;                                    
    data_array[5] = q++;                                    
    data_array[6] = q++;                                    
    data_array[7] = q++;                                    

    /* Automatically goes to TX mode */
    nrf24_send(data_array);        

    /* Wait for transmission to end */
    while(nrf24_isSending());

    /* Make analysis on last tranmission attempt */
    temp = nrf24_lastMessageStatus();

    if(temp == NRF24_TRANSMISSON_OK)
    {                    
        printf("> Tranmission went OK\r\n");
    }
    else if(temp == NRF24_MESSAGE_LOST)
    {                    
        printf("> Message is lost ...\r\n");    
    }

    /* Retranmission count indicates the tranmission quality */
    temp = nrf24_retransmissionCount();
    printf("> Retranmission count: %d\r\n",temp);

    /* Optionally, go back to RX mode ... */
    nrf24_powerUpRx();

    /* Or you might want to power down after TX */
    // nrf24_powerDown();            

    /* Wait a little ... */
    _delay_ms(30);
    _delay_ms(30);
    _delay_ms(30);
    _delay_ms(30);
    _delay_ms(30);
    _delay_ms(30);
    _delay_ms(30);
    _delay_ms(30);
    _delay_ms(30);
    _delay_ms(30);
    _delay_ms(30);


  }
                              
    return 0;
}
