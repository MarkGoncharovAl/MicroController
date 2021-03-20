#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 8000000 //Clock Speed
#define BAUD 9600
#include <util/delay.h>

#define PA0 0
#define PA1 1
const uint8_t MYUBRR = F_CPU / 8 / BAUD - 1;

//USART communication
////////////////////////////////////
void USART_Init (void);
int serial_available (); //returns unread data in receive buffer
int serial_read (); //read byte
void serial_write (uint8_t data);
void serial_write_array (char* buffer , uint8_t size);
void WRITE_HEX_8 (const uint8_t data); //printing in ASCII code
void WRITE_DEC (const uint16_t num); //ASCII
void receive (uint8_t* buf , int len); // printing to buf
////////////////////////////////////

void Timer_Init ();
void Button_Init (); //setting DDRB, PORTB

uint8_t TimerOverflow = 0;
ISR (TIMER1_COMPA_vect)
{
    TimerOverflow = 1;
    TIFR &= ~(1 << ICF1);
}

//trigger pin = bit in DDRA port (PA0, PA1, ...)
//in order to get more sensor -> rewrite DDRA (now 0b11)
uint16_t MeasureDistance (uint8_t trigger_pin);


int main ()
{
    cli ();
    USART_Init ();
    Timer_Init ();
    sei ();

    Button_Init ();
    _delay_us (10);

    while (1)
    {
            
        PORTB ^= 1 << PINB0; // to toggle only pin0 on Port B
        if (bit_is_clear (PINB , 1)) //if button is toggled
        {
            _delay_ms (500);
            uint16_t dist0= MeasureDistance (PA0);
            uint16_t dist1 = MeasureDistance (PA1);


            serial_write_array ("Distance PA0: " , 13);
            WRITE_DEC (dist0);
            serial_write_array ("\nDistance PA1: " , 14);
            WRITE_DEC (dist1);

            serial_write_array ("\n" , 1);
        }
        else
            _delay_ms (100);

    }

}


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//USART Initialization
void USART_Init (void)
{
    // start writing to the UBRRH
    UBRRH = (uint8_t)(MYUBRR >> 8);
    UBRRL = (uint8_t)MYUBRR;

    // enable receive and transmit
    UCSRB = (1 << RXEN) | (1 << TXEN);

    // disable all interruption
    UCSRB &= ~(1 << RXCIE) & ~(1 << TXCIE) & ~(1 << UDRIE);

    // asynchronous mode
    UCSRC &= ~(1 << UMSEL) | (1 << URSEL);

    // no parite checker;
    UCSRC &= (~(1 << UPM0) & ~(1 << UPM1)) | (1 << URSEL);

    // one stop bit
    UCSRC &= ~(1 << USBS) | (1 << URSEL);

    // double speed mode
    UCSRA |= (1 << U2X);

    // 8 bit character size
    UCSRC = (1 << UCSZ0) | (1 << UCSZ1) | (1 << URSEL);
    UCSRB &= ~(1 << UCSZ2);
}

int serial_available ()
{
    // waiting for unread data in receive buffer
    return (UCSRA & (1 << RXC));
};
int serial_read ()
{
    // read buffer!
    return UDR;
};

void serial_write (uint8_t data)


{
    // waitinng for transmit buffer
    // should wait permission to transmit
    while (!(UCSRA & (1 << UDRE)))
        ;

    // send data!
    UDR = data;
};

void serial_write_array (char* buffer , uint8_t size)
{
    while (size--)
        serial_write (*buffer++);
}

void WRITE_HEX_8 (const uint8_t data)
{
    //creates 4 bites in array
    uint8_t bites[2] = { (data & 0x0f), ((data & 0xf0) >> 4) };
    //in order to output hexes
    for (uint8_t i = 0; i < 2; ++i)
    {
        if (bites[i] < 10)
            bites[i] += '0';
        else
            bites[i] += 'A' - 10;
        serial_write (bites[i]);
    }
}

void WRITE_DEC (const uint16_t num)
{
    if (num == 0)
    {
        serial_write ('0');
        return;
    }
    //case num > 0

    uint16_t out = num;
    int i = 0;
    uint8_t nums[10] = { 0 };
    while (out != 0)
    {
        nums[i] = (out % 10) + '0';
        out /= 10;
        i++;
    }

    for (i = i - 1; i >= 0; --i)
        serial_write (nums[i]);
}

void receive (uint8_t* buf , int len)
{
    for (int read_bytes = 0; read_bytes < len; ++read_bytes)
    {
        uint8_t ret = 0;
        while ((ret = serial_read ()) < 0)
            ;

        buf[read_bytes] = ret;
    }
}
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

void Button_Init ()
{
    DDRB |= 1 << PINB0; // PB0 is output
    DDRB &= ~(1 << PINB1); //PB1 is input
    PORTB |= 1 << PINB1; //set PINB1 to high reading
}

void Timer_Init ()
{
    //triger pin as output
    DDRA = 0b11; //PA0 and PA1 are ready

    // turn on pullup
    PORTD = 0xFF;

    // CTC mode
    TCCR1B |= (1 << WGM12);

    // enable interrupt on compare
    TIMSK = (1 << OCIE1A);
    OCR1AL = 250;
    OCR1AH = 100;
}

uint16_t MeasureDistance (uint8_t trigger_pin)
{
    // Give 10us trigger pulse on trig. pin to HC-SR04
    PORTA |= (1 << trigger_pin);
    _delay_us (10);
    PORTA &= (~(1 << trigger_pin));
    // Clear Timer counter
    TCNT1 = 0;

    // Start timer without prescaler
    TCCR1B |= (1 << CS10);
    // Capture on rising edge
    TCCR1B |= (1 << ICES1);
    // Clear ICP flag (Input Capture flag)
    TIFR |= (1 << ICF1);
    // Clear Timer Overflow flag
    TIFR |= (1 << OCF1A);

    // Calculate width of Echo by Input Capture (ICP)
    while ((TIFR & (1 << ICF1)) == 0);

    // Clear Timer counter
    TCNT1 = 0;

    // Capture on falling edge
    TCCR1B &= ~(1 << ICES1);
    // Clear ICP flag (Input Capture flag)
    TIFR |= (1 << ICF1);
    // Clear Timer Overflow flag
    TIFR |= (1 << OCF1A);
    // Clear Timer overflow count
    TimerOverflow = 0;

    uint16_t Count_Tact = 0;
    // Wait for falling edge
    while ((TIFR & (1 << ICF1)) == 0)
        Count_Tact += 7;

    if (TimerOverflow)
        return 0;

    return (Count_Tact / (F_CPU / 1000000)) / 6; // distance in ms
}

// void delay_us (uint32_t nopes)
// {
//     uint32_t count = nopes / (FOSC / 1000000);
//     count /= 2;
//     for (uint32_t i = 0; i < count; ++i)
//         ;
// }
