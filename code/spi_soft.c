/* noch nicht für 4-Draht-Protokoll ausgelegt
 * d.h.: TFTs haben noch einen Pin für die Command/Data-Unterscheidungserkennung,
 * der immer vor dem CS_PIN eingestellt wird.
 */

#include "spi_soft.h"

void initSPI() {
	setOutput(SCK_PIN);
	clrPin(SCK_PIN);
	setOutput(DO_PIN);
	setOutput(CS_PIN);
	clrPin(CS_PIN);
}

uint8_t sendDat(uint8_t dat) {
    uint8_t maske = 0b10000000;
    uint8_t erg = 0;

    
    if (dat & maske)
        setPin(DO_PIN);
    else
        clrPin(DO_PIN);
    setPin(SCK_PIN); //hier: CLK negative Logic (clrPin();)
    clrPin(SCK_PIN); //hier: CLK negative Logic (setPin();)
    maske >>= 1;

    
    if (dat & maske)
        setPin(DO_PIN);
    else
        clrPin(DO_PIN);
    setPin(SCK_PIN); //hier: CLK negative Logic (clrPin();)
    clrPin(SCK_PIN); //hier: CLK negative Logic (setPin();)
    maske >>= 1;


    if (dat & maske)
        setPin(DO_PIN);
    else
        clrPin(DO_PIN);
    setPin(SCK_PIN); //hier: CLK negative Logic (clrPin();)
    clrPin(SCK_PIN); //hier: CLK negative Logic (setPin();)
    maske >>= 1;

    
    if (dat & maske)
        setPin(DO_PIN);
    else
        clrPin(DO_PIN);
    setPin(SCK_PIN); //hier: CLK negative Logic (clrPin();)
    clrPin(SCK_PIN); //hier: CLK negative Logic (setPin();)
    maske >>= 1;

    
    if (dat & maske)
        setPin(DO_PIN);
    else
        clrPin(DO_PIN);
    setPin(SCK_PIN); //hier: CLK negative Logic (clrPin();)
    clrPin(SCK_PIN); //hier: CLK negative Logic (setPin();)
    maske >>= 1;

    
    if (dat & maske)
        setPin(DO_PIN);
    else
        clrPin(DO_PIN);
    setPin(SCK_PIN); //hier: CLK negative Logic (clrPin();)
    clrPin(SCK_PIN); //hier: CLK negative Logic (setPin();)
    maske >>= 1;

    
    if (dat & maske)
        setPin(DO_PIN);
    else
        clrPin(DO_PIN);
    setPin(SCK_PIN); //hier: CLK negative Logic (clrPin();)
    clrPin(SCK_PIN); //hier: CLK negative Logic (setPin();)
    maske >>= 1;

    
    if (dat & maske)
        setPin(DO_PIN);
    else
        clrPin(DO_PIN);
    setPin(SCK_PIN); //hier: CLK negative Logic (clrPin();)
    clrPin(SCK_PIN); //hier: CLK negative Logic (setPin();)
    maske >>= 1;

    return erg;
}

uint8_t sendData8(uint8_t dat) {
    clrPin(CS_PIN);
    dat = sendDat(dat);
    setPin(CS_PIN);
    return dat;
}

uint16_t sendData16(uint16_t dat) {
    clrPin(CS_PIN);
    uint8_t msByte = dat >> 8;
    uint8_t lsByte = dat & 0xFF;
    msByte = sendDat(msByte);
    lsByte = sendDat(lsByte);
    setPin(CS_PIN);
    return (msByte << 8) | lsByte;
}
