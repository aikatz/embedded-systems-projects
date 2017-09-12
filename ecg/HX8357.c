#include "fsl_device_registers.h"

#include "HX8357.h"
#include "spi.h"
#include "utils.h"

// From https://github.com/adafruit/Adafruit-GFX-Library/blob/master/Adafruit_GFX.cpp
#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

/*
 HX8357_init

 Initializes the DC pin, sends a ton of commands to the screen to initialize it

 The command sequences (most of the function) are copied from: 
 https://github.com/adafruit/Adafruit_HX8357_Library/blob/master/Adafruit_HX8357.cpp
*/

void HX8357_init(void)
{
	/* Configure the D/C pin */
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK; 	// Enable clock to PORTC
	PORTC->PCR[12] = (1 << 8);			// Configure D/C (PTC12) for GPIO
	PTC->PDOR = (1 << 12);         		// Turn off PTC12
	PTC->PDDR = (1 << 12);				// Enable PTC12 as output
	
	set_CS_high();
	
	/* These sequences of writes are basically just copied from the Arduino driver */
    writeCommand(HX8357_SWRESET);
    delay_ms(10);

    // setextc
    writeCommand(HX8357D_SETC);
    writeData(0xFF);
    writeData(0x83);
    writeData(0x57);
    delay_ms(300);

    // setRGB which also enables SDO
    writeCommand(HX8357_SETRGB); 
    writeData(0x80);  //enable SDO pin!
    writeData(0x0);
    writeData(0x06);
    writeData(0x06);

    writeCommand(HX8357D_SETCOM);
    writeData(0x25);  // -1.52V
    
    writeCommand(HX8357_SETOSC);
    writeData(0x68);  // Normal mode 70Hz, Idle mode 55 Hz
    
    writeCommand(HX8357_SETPANEL); //Set Panel
    writeData(0x05);  // BGR, Gate direction swapped
    
    writeCommand(HX8357_SETPWR1);
    writeData(0x00);  // Not deep standby
    writeData(0x15);  //BT
    writeData(0x1C);  //VSPR
    writeData(0x1C);  //VSNR
    writeData(0x83);  //AP
    writeData(0xAA);  //FS

    writeCommand(HX8357D_SETSTBA);  
    writeData(0x50);  //OPON normal
    writeData(0x50);  //OPON idle
    writeData(0x01);  //STBA
    writeData(0x3C);  //STBA
    writeData(0x1E);  //STBA
    writeData(0x08);  //GEN
    
    writeCommand(HX8357D_SETCYC);  
    writeData(0x02);  //NW 0x02
    writeData(0x40);  //RTN
    writeData(0x00);  //DIV
    writeData(0x2A);  //DUM
    writeData(0x2A);  //DUM
    writeData(0x0D);  //GDON
    writeData(0x78);  //GDOFF

    writeCommand(HX8357D_SETGAMMA); 
    writeData(0x02);
    writeData(0x0A);
    writeData(0x11);
    writeData(0x1d);
    writeData(0x23);
    writeData(0x35);
    writeData(0x41);
    writeData(0x4b);
    writeData(0x4b);
    writeData(0x42);
    writeData(0x3A);
    writeData(0x27);
    writeData(0x1B);
    writeData(0x08);
    writeData(0x09);
    writeData(0x03);
    writeData(0x02);
    writeData(0x0A);
    writeData(0x11);
    writeData(0x1d);
    writeData(0x23);
    writeData(0x35);
    writeData(0x41);
    writeData(0x4b);
    writeData(0x4b);
    writeData(0x42);
    writeData(0x3A);
    writeData(0x27);
    writeData(0x1B);
    writeData(0x08);
    writeData(0x09);
    writeData(0x03);
    writeData(0x00);
    writeData(0x01);
    
	// 16 bit
    writeCommand(HX8357_COLMOD);
    writeData(0x55);
    
    writeCommand(HX8357_MADCTL);  
    writeData(0xC0); 
    
	// TE off
    writeCommand(HX8357_TEON);
    writeData(0x00); 
    
	// tear line
    writeCommand(HX8357_TEARLINE);
    writeData(0x00); 
    writeData(0x02);

	//Exit Sleep
    writeCommand(HX8357_SLPOUT); 
    delay_ms(150);
    
	// display on
    writeCommand(HX8357_DISPON);
    delay_ms(50);
}

/*
 writeCommand

 Sends a command byte to the screen (sets DC to low)

 We wrote this ourselves but there is an extremely similar function
 in the Adafruit driver
*/
void writeCommand(uint8_t cmd)
{
	set_DC_low();
	set_CS_low();
	
	SPI_write(cmd);
	
	set_CS_high();
}

/*
 writeData

 Sends a data byte to the screen (sets DC to high)

 We wrote this ourselves but there is an extremely similar function
 in the Adafruit driver
*/
void writeData(uint8_t data)
{
	set_DC_high();
	set_CS_low();

	SPI_write(data);
	
	set_CS_high();
}

/*
 setAddrWindow

 Sets the addressing window on the screen

 Taken from https://github.com/adafruit/Adafruit_HX8357_Library/blob/master/Adafruit_HX8357.cpp

 We removed uncessary code and made small modifications
*/
void setAddrWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	writeCommand(HX8357_CASET); // Column addr set
	writeData(x1 >> 8);
	writeData(x1 & 0xFF);  		// XSTART 
	writeData(x2 >> 8);
	writeData(x2 & 0xFF);		// XEND

	writeCommand(HX8357_PASET); // Row addr set
	writeData(y1>>8);
	writeData(y1);     			// YSTART
	writeData(y2>>8);
	writeData(y2);     			// YEND

	writeCommand(HX8357_RAMWR); // write to RAM
}

/*
 drawPixel

 Draws a single pixel on the screen

 Taken from https://github.com/adafruit/Adafruit_HX8357_Library/blob/master/Adafruit_HX8357.cpp

 We removed uncessary code and made small modifications
*/
void drawPixel(uint16_t x, uint16_t y, uint16_t color)
{
	// don't need to check if < 0 since x and y are unsigned
	if ((x >= HX8357_TFTWIDTH) || (y >= HX8357_TFTHEIGHT))
	{
		return;
	}
	
	setAddrWindow(x, y, x, y);
	
	set_DC_high();
	set_CS_low();

	writeData(color >> 8);
	writeData(color);
	
	set_CS_high();
}

/*
 fillRect

 Fills a rectangular section of the screen

 Taken from https://github.com/adafruit/Adafruit_HX8357_Library/blob/master/Adafruit_HX8357.cpp

 We removed uncessary code and made small modifications
*/
void fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
	// rudimentary clipping
	if((x >= HX8357_TFTWIDTH) || (y >= HX8357_TFTHEIGHT)) return;
	if((x + w - 1) >= HX8357_TFTWIDTH)  w = HX8357_TFTWIDTH  - x;
	if((y + h - 1) >= HX8357_TFTHEIGHT) h = HX8357_TFTHEIGHT - y;

	setAddrWindow(x, y, x+w-1, y+h-1);

	uint8_t hi = color >> 8, lo = color;

	set_DC_high();
	set_CS_low();
	
	for(y=h; y>0; y--) {
		for(x=w; x>0; x--) {
			SPI_write(hi);
			SPI_write(lo);
		}
	}
}


/*
 fillScreen

 Fills the screen with one color, uses fillRect. 

 We wrote this ourselves, but there is an effectively idential function in 
 the adafruit Arduino driver. 
*/
void fillScreen(uint16_t color) 
{
	fillRect(0, 0, HX8357_TFTWIDTH, HX8357_TFTHEIGHT, color);
}

/*
 drawFastHLine

 Draws a fast horizontal line

 Taken from https://github.com/adafruit/Adafruit_HX8357_Library/blob/master/Adafruit_HX8357.cpp

 We removed uncessary code and made small modifications
*/
void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {

	// Rudimentary clipping
	if((x >= HX8357_TFTWIDTH) || (y >= HX8357_TFTHEIGHT)) return;
	if((x+w-1) >= HX8357_TFTWIDTH) w = HX8357_TFTWIDTH-x;
  
	setAddrWindow(x, y, x+w-1, y);

	uint8_t hi = color >> 8, lo = color;

	set_DC_high();
	set_CS_low();

	while (w--) {
		SPI_write(hi);
		SPI_write(lo);
	}
	
	set_CS_high();
}
	
/*
 drawFastVLine

 Draw a fast vertical line

 Taken from https://github.com/adafruit/Adafruit_HX8357_Library/blob/master/Adafruit_HX8357.cpp

 We removed uncessary code and made small modifications
*/
void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {

	// Rudimentary clipping
	if((x >= HX8357_TFTWIDTH) || (y >= HX8357_TFTHEIGHT)) return;
	if((y+h-1) >= HX8357_TFTHEIGHT) h = HX8357_TFTHEIGHT-y;

	setAddrWindow(x, y, x, y+h-1);

	// Split the two bytes into individual bytes
	uint8_t hi = color >> 8, lo = color;

	set_DC_high();
	set_CS_low();
	
	while (h--) {
		SPI_write(hi);
		SPI_write(lo);
	}

	set_CS_high();
}

/*
 drawLine

 Straightforward implementation of Bresenham's line-drawing algorithm. 
 Draws a line between two points: (x1, y1) and (x2, y2)

 Taken from https://github.com/adafruit/Adafruit-GFX-Library/blob/master/Adafruit_GFX.cpp
 The authors of the repo credit Wikipedia for this function

 We made no substantive modifications (only changed x0 -> x1, x1 -> x2)
*/
void drawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
    int16_t steep = abs(y2 - y1) > abs(x2 - x1);
    if (steep) {
        _swap_int16_t(x1, y1);
        _swap_int16_t(x2, y2);
    }

    if (x1 > x2) {
        _swap_int16_t(x1, x2);
        _swap_int16_t(y1, y2);
    }

    int16_t dx, dy;
    dx = x2 - x1;
    dy = abs(y2 - y1);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y1 < y2) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x1 <= x2; x1++) {
        if (steep) {
            drawPixel(y1, x1, color);
        } else {
            drawPixel(x1, y1, color);
        }
        err -= dy;
        if (err < 0) {
            y1 += ystep;
            err += dx;
        }
    }
}

