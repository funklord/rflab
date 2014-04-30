#include "pin.h"


pin_state_t read_pin(pin_t pin)
{
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
	//NOP();
	return(READ_BIT(PIN2REG_PIN(pin), PIN2PIN(pin)));
#elif defined(__MSP430__)
	return(READ_BIT(PIN2REG_IN(pin), PIN2PIN(pin)));
#endif
}


void write_pin(pin_t pin, pin_state_t state)
{
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
	WRITE_BIT(PIN2REG_PORT(pin), PIN2PIN(pin), state);
#elif defined(__MSP430__)
	WRITE_BIT(PIN2REG_OUT(pin), PIN2PIN(pin), state);
#endif
}


void toggle_pin(pin_t pin)
{
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
	SET_BIT(PIN2REG_PIN(pin), PIN2PIN(pin));
#elif defined(__MSP430__)
	PIN2REG_OUT(pin) ^= PIN2PIN(pin);
#endif
}


#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
void pin_mode(pin_t pin, pin_mode_t mode)
{
	WRITE_BIT(PIN2REG_DDR(pin), PIN2PIN(pin), (mode == PIN_MODE_OUTPUT));
	if(mode != PIN_MODE_OUTPUT)
		WRITE_BIT(PIN2REG_PORT(pin), PIN2PIN(pin), (mode == PIN_MODE_INPUT_PULLUP));
}
#elif defined(__MSP430__)
void pin_mode(pin_t pin, pin_mode_t mode)
{
	WRITE_BIT(PIN2REG_DIR(pin), PIN2PIN(pin), (mode == PIN_MODE_OUTPUT));
	if(mode != PIN_MODE_OUTPUT) {
		WRITE_BIT(PIN2REG_REN(pin), PIN2PIN(pin), (mode != PIN_MODE_INPUT));
		if(mode != PIN_MODE_INPUT
			WRITE_BIT(PIN2REG_OUT(pin), PIN2PIN(pin), (mode == PIN_MODE_INPUT_PULLUP));
	}
}


void pin_interrupt_mode(pin_t pin, pin_interrupt_mode_t mode)
{
	WRITE_BIT(PIN2REG_IE(pin), PIN2PIN(pin), (mode != PIN_INTERRUPT_MODE_OFF));
	if(mode != PIN_INTERRUPT_MODE_OFF)
		WRITE_BIT(PIN2REG_IES(pin), PIN2PIN(pin), (mode == PIN_INTERRUPT_MODE_FALLING_EDGE));
}
#endif

/* Measures the length (in microseconds) of a pulse on the pin; state is HIGH
 * or LOW, the type of pulse to measure.  Works on pulses from 2-3 microseconds
 * to 3 minutes in length, but must be called at least a few dozen microseconds
 * before the start of the pulse. */

uint_fast16_t get_pulse_length(pin_t pin, pin_state_t state, uint_fast16_t timeout)
{
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
	// cache the port and bit of the pin in order to speed up the
	// pulse width measuring loop and achieve finer resolution.  calling
	// digitalRead() instead yields much coarser resolution.
	//uint8_t bit = digitalPinToBitMask(pin);
	//uint8_t port = digitalPinToPort(pin);
	//uint8_t stateMask = (state ? bit : 0);
	uint_fast16_t pulse_width = 0; // keep initialization out of time critical area

	// convert the timeout from microseconds to a number of times through
	// the initial loop; it takes 16 clock cycles per iteration.
	uint_fast16_t i = 0;
	uint_fast16_t i_max = ms_to_cycles(timeout) / 16;

	// wait for any previous pulse to end
	while(read_pin(pin_t pin) == state) {
		if(i++ > i_max)
			return(0);
	}

	// wait for the pulse to start
	while(read_pin(pin_t pin) != state) {
		if(i++ > i_max)
			return(0);
	}

	// wait for the pulse to stop
	while(read_pin(pin_t pin) == state) {
		if(i++ > i_max)
			return(0);
		pulse_width++;
	}

	// convert the reading to microseconds. The loop has been determined
	// to be 20 clock cycles long and have about 16 clocks between the edge
	// and the start of the loop. There will be some error introduced by
	// the interrupt handlers.
	return(cycles_to_ms(pulsewidth * 21 + 16));
#endif
}