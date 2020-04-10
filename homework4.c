#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "homework4.h"

int main(void)
{
    char rChar;
    char *response = "\n\n\r2534 is the best course in the curriculum!\r\n\n";

    // TODO: Declare the variables that main uses to interact with your state machine. Done.

    int itIsDone = 0;

    // Stops the Watchdog timer.
    initBoard();
    // TODO: Declare a UART config struct as defined in uart.h. Done.
    //       To begin, configure the UART for 9600 baud, 8-bit payload (LSB first), no parity, 1 stop bit.

    eUSCI_UART_ConfigV1 UARTConfig =
    {
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,         //The clock source uses a 3MHz setting.
        19,                                    //N = 3M/9600 = 312.5, clockPrescalar = int(312/16) = int(19.5) = 19
        8,                                      //firstModReg = 16 * frac(312/16) = 16 * 0.5 = 8, UCBRSx = 0x88
        0x55,                                   //secondModReg = frac(N) = frac(312.5) = 0.5, drops to 0.4738, UCBRSx = 0x55
        EUSCI_A_UART_NO_PARITY,                  //Parity given above.
        EUSCI_A_UART_LSB_FIRST,                 //Payload given above.
        EUSCI_A_UART_ONE_STOP_BIT,              //Stop bit given above.
        EUSCI_A_UART_MODE,                            //Taken from class video.
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,                   //Taken from class video.
        EUSCI_A_UART_8_BIT_LEN,                             //Taken from class video.
    };


    // TODO: Make sure Tx AND Rx pins of EUSCI_A0 work for UART and not as regular GPIO pins. Done.

    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION); //Transmit pin Tx.

    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION); //Receive pin Rx.

    // TODO: Initialize EUSCI_A0. Done.
    UART_initModule(EUSCI_A0_BASE, &UARTConfig);

    // TODO: Enable EUSCI_A0. Done.

    UART_enableModule(EUSCI_A0_BASE);

    //initUART(EUSCI_A0_BASE, &UARTConfig);

    while(1)
    {
        // TODO: Check the receive interrupt flag to see if a received character is available.
        //       Return 0xFF if no character is available. Done.

        if(UART_getInterruptStatus(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG) == EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG) //Use the UART_getInterruptStatus to get the receive interrupt flag.
        {
            rChar = UART_receiveData(EUSCI_A0_BASE);
        }
        else
        {
            rChar = 0xFF;
        }


        // TODO: If an actual character was received, echo the character to the terminal AND use it to update the FSM.
        //       Check the transmit interrupt flag prior to transmitting the character.

        if(rChar != 0xFF)
        {
            if (UART_getInterruptStatus(EUSCI_A0_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG) == EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG) //Use the UART_getInterruptStatus to get the transmit interrupt flag.
            {
                charFSM(rChar);
                if(charFSM(rChar))
                {
                    itIsDone = 1;
                }
                else
                {
                    itIsDone = 0;
                }
            }
        }


        // TODO: If the FSM indicates a successful string entry, transmit the response string.
        //       Check the transmit interrupt flag prior to transmitting each character and moving on to the next one.
        //       Make sure to reset the success variable after transmission.

            if(itIsDone == 1)
            {
                if (UART_getInterruptStatus(EUSCI_A0_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG) == EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG) //Use the UART_getInterruptStatus to get the transmit interrupt flag.
                {
                    UART_transmitData(EUSCI_A0_BASE, *response);
                }

                itIsDone = 0;
            }
    }
}

void initBoard()
{
    WDT_A_hold(WDT_A_BASE);
}

// TODO: FSM for detecting character sequence. Done.
bool charFSM(char rChar)
{
    bool finished = false;

    typedef enum {SX, S2, S25, S253} message_state;

    static message_state comboKey = SX;

    switch (comboKey)
    {
        case SX:
        if (rChar == '2')
            {
                comboKey = S2;
            }
            else
            {
                comboKey = SX;
            }

            break;

        case S2:
            if (rChar == '5')
            {
                comboKey = S25;
            }
            else if (rChar == '2')
            {
                comboKey = S2;
            }
            else
            {
                comboKey = SX;
            }

            break;

        case S25:
            if (rChar == '3')
            {
                comboKey = S253;
            }
            else if (rChar == '2')
            {
                comboKey = S2;
            }
            else
            {
                comboKey = SX;
            }

            break;

        case S253:
            if (rChar == '4')
            {
                comboKey = SX;
                finished = true;
            }
            else if (rChar == '2')
            {
                comboKey = S2;
            }
            else
            {
                comboKey = SX;
            }

            break;


    }

    return finished;
}

bool UART_hasChar(uint32_t EUSCI_A0_BASE)
{
    bool dataAvailable;

    if(UART_getInterruptStatus(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG) == EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
    {
        dataAvailable = true;
    }
    else
    {
        dataAvailable = false;
    }

    return dataAvailable;
}

uint8_t UARTGetChar(uint32_t EUSCI_A0_BASE)
{
    char dataReceived;
    if(UART_hasChar(EUSCI_A0_BASE))
    {
        UART_receiveData(EUSCI_A0_BASE);
        dataReceived = UART_receiveData(EUSCI_A0_BASE);
    }
    else
    {
        dataReceived = 0xFF;
    }

    return dataRecevied;
}

void initUART(uint32_t moduleInstance, const eUSCI_UART_ConfigV1 *config)
{
    UART_initModule(moduleInstance, config);
    UART_enableModule(moduleInstance);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION); //Transmit pin Tx.
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION); //Receive pin Rx.
}
