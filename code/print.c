/*
 * von http://www.eckart-winkler.de/computer/c/varargs.htm
 * 
 * Formatzeichen:
 * %c = Char
 * %s = String
 * %d = Integer
 * char *txt
 * 
 * *txt ist der Text in dem Formatzeichen eingebettet sein können,
 * deren Parameter hinter *txt, durch Kommatas separiert, angegeben werden.
 * Beispiel:
 * out("test %c, %s, %d", 'c', "String", 123);
 */

#include <stdint.h>
#include "print.h"
#include "uart_attiny.h"

void out(char *txt, ...) {
  va_list args;
  char *pt;
  char ch;
  char *str;
  int n;
  char back[10];
	char *ph = back;	//ph bekommt die Startadresse von back
	int h;

  va_start(args, txt);  //Parameterabfrage initialisieren
  for (pt = txt; *pt; pt++) {
    if (*pt != '%') {		//Alle Zeichen ausser % ausgeben 
			sendDat(*pt);
			continue;
		}
    switch (*(++pt)) {	//Formatkennzeichner abarbeiten
      case 'c':
        ch = (char)va_arg(args, int);
        sendDat(ch); 
        break;
      
      case 's':
        str = (char*)va_arg(args, int*);
        while(*str) {
					sendDat(*str);
					str++;
				} 
        break;
      
      case 'd':
        n = (int)va_arg(args, int);
				if(!n) {					//Gleich null?
					sendDat('0'); 
					return; 
				}
				if(n < 0) {				//Vorzeichen?
					sendDat('-'); 
					n = -n; 				//Vorzeichen ist geschrieben, jetzt mit dem Betrag weiter arbeiten
				}
				while(n) { 								 	//Ziffern rueckwaerts nach back schreiben
					h = n % 10; 					 		//letzte Ziffer ermitteln, in h speichern
					n = n / 10; 							//letzte Ziffer entfernen
					*ph++ = (char)('0' + h); 	//an char ph übergeben (Ascii 0 + int h = Ascii von Ziffer!!!!). Die Ziffern sind in umgekehrter Reihenfolge in ph! 
				}
				while(ph > back) 	//Nun Ziffern ausgeben 
					sendDat(*(--ph));
        break;
      case 0:
        return;
      default:
        sendDat(*pt); 
        break;
    }
  }
  va_end(args);						//Parameterabfrage beenden und Schluss
  return;
}
