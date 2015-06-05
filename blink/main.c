#include <avr/io.h>			// Einbinden von Einstellungen/Definitionen/usw. für den Mikrocontroller
#define F_CPU 1000000
#include <util/delay.h>			// Einbinden der _delay_ms()-Funktion um Wartezeiten zu erzeugen

int main (void)				// Hauptprogramm, hier startet der Mikrocontroller
{
	unsigned char zwischenspeicher;		// Initialisierung
	DDRB |= (1<<PB2)|(1<<PB3);		
	PORTB = 0b00000100;
	while(1)			// Nie endende Hauptschleife (Endlosschleife)
	{
		zwischenspeicher = PORTB;				// Einlesen
		zwischenspeicher = zwischenspeicher ^ 0b00001100;	// Verarbeiten
		PORTB = zwischenspeicher;				// Ausgeben
		_delay_ms(500);						// Wartezeit von 500ms
	}				// Ende der Endlosschleife (Es wird wieder zu "while(1)" gesprungen)
	return 0;			// Wird nie erreicht, aber ohne schreibt der GCC eine Warnung
}					// Ende des Hauptprogramms
