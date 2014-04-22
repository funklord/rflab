#ifndef PIN_H
#define PIN_H


//#if !defined(__AVR__) && !defined(__GCC_AVR32__)
#if !defined(__AVR_ATmega328P__) || !defined(__AVR_ATmega328__)
#error unsupported platform
#endif

#if defined(__AVR__)
#define ei() sei() //!< enable interrupts
#define di() cli() //!< disable interrupts
#endif

#ifdef __ARDUINO__
#define F_CPU 16000000L
#endif

#define cycles_per_ms() ( F_CPU / 1000000L )
#define cycles_to_us(a) ( ((a) * 1000L) / (F_CPU / 1000L) )
#define ms_to_cycles(a) ( ((a) * (F_CPU / 1000L)) / 1000L )

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
#define NOP() __no_operation()
#define PIN2PIN(p) ((p) & 0x7)
#define PIN2REG_PIN(p) (((p)>>4)?(PIND):(((p)>>3)?(PINC):(PINB)))
#define PIN2REG_PORT(p) (((p)>>4)?(PORTD):(((p)>>3)?(PORTC):(PORTB)))
#define PIN2REG_DDR(p) (((p)>>4)?(DDRDD):(((p)>>3)?(DDRC):(DDRB)))
#endif


typedef enum {
#if defined(__AVR__) || (__GCC_AVR32__)
	PIN_STATE_LOW  = 0,
	PIN_STATE_HIGH = 1
#endif
} pin_state_t;


#if defined(__AVR__) || (__GCC_AVR32__)
typedef enum {
	PIN_MODE_INPUT,
	PIN_MODE_INPUT_PULLUP,
	PIN_MODE_OUTPUT
} pin_mode_t;
#endif







typedef enum {
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
	PIN_B0 = 0,
	PIN_B1 = 1,
	PIN_B2 = 2,
	PIN_B3 = 3,
	PIN_B4 = 4,
	PIN_B5 = 5,
	PIN_B6 = 6,
	PIN_B7 = 7,
	PIN_C0 = 8,
	PIN_C1 = 9,
	PIN_C2 = 10,
	PIN_C3 = 11,
	PIN_C4 = 12,
	PIN_C5 = 13,
	PIN_C6 = 14,

	PIN_D0 = 16,
	PIN_D1 = 17,
	PIN_D2 = 18,
	PIN_D3 = 19,
	PIN_D4 = 20,
	PIN_D5 = 21,
	PIN_D6 = 22,
	PIN_D7 = 23,

	PIN_ADC0 = PIN_C0,
	PIN_ADC1 = PIN_C1,
	PIN_ADC2 = PIN_C2,
	PIN_ADC3 = PIN_C3,
	PIN_ADC4 = PIN_C4,
	PIN_ADC5 = PIN_C5,

	PIN_PWM0 = PIN_D3,
	PIN_PWM1 = PIN_D5,
	PIN_PWM2 = PIN_D6,
	PIN_PWM3 = PIN_B1,
	PIN_PWM4 = PIN_B2,
	PIN_PWM5 = PIN_B3,
#if defined(__AVR_ATmega328P__)
	PIN_1  = PIN_C6,
	PIN_2  = PIN_D0,
	PIN_3  = PIN_D1,
	PIN_4  = PIN_D2,
	PIN_5  = PIN_D3,
	PIN_6  = PIN_D4,
	PIN_9  = PIN_B6,
	PIN_10 = PIN_B7,
	PIN_11 = PIN_D5,
	PIN_12 = PIN_D6,
	PIN_13 = PIN_D7,
	PIN_14 = PIN_B0,
	PIN_15 = PIN_B1,
	PIN_16 = PIN_B2,
	PIN_17 = PIN_B3,
	PIN_18 = PIN_B4,
	PIN_19 = PIN_B5,
	PIN_23 = PIN_C0,
	PIN_24 = PIN_C1,
	PIN_25 = PIN_C2,
	PIN_26 = PIN_C3,
	PIN_27 = PIN_C4,
	PIN_28 = PIN_C5,
#endif
#if defined(__ARDUINO__)
	PIN_ARDUINO_0  = PIN_D0,
	PIN_ARDUINO_1  = PIN_D1,
	PIN_ARDUINO_2  = PIN_D2,
	PIN_ARDUINO_3  = PIN_D3,
	PIN_ARDUINO_4  = PIN_D4,
	PIN_ARDUINO_5  = PIN_D5,
	PIN_ARDUINO_6  = PIN_D6,
	PIN_ARDUINO_7  = PIN_D7,
	PIN_ARDUINO_8  = PIN_B0,
	PIN_ARDUINO_9  = PIN_B1,
	PIN_ARDUINO_10 = PIN_B2,
	PIN_ARDUINO_11 = PIN_B3,
	PIN_ARDUINO_12 = PIN_B4,
	PIN_ARDUINO_13 = PIN_B5,
	PIN_ARDUINO_A0 = PIN_C0,
	PIN_ARDUINO_A1 = PIN_C1,
	PIN_ARDUINO_A2 = PIN_C2,
	PIN_ARDUINO_A3 = PIN_C3,
	PIN_ARDUINO_A4 = PIN_C4,
	PIN_ARDUINO_A5 = PIN_C5,
#endif
#endif
	PIN_AMOUNT
} pin_t;

pin_state_t read_pin(pin_t pin);
void write_pin(pin_t pin, pin_state_t state);
void toggle_pin(pin_t pin);
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
void pin_mode(pin_t pin, pin_mode_t mode);
#endif
uint_fast16_t get_pulse_length(pin_t pin, pin_state_t state, uint_fast16_t timeout);

#endif