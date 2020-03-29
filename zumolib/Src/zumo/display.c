/*
  ___          _____     __  __
   _/    _  _ (_  | |\/|  _)  _)
  /__|_||||(_)__) | |  | __) /__

 Fachhochschule Südwestfalen
 Mechatronik/Mikrocomputer
 Prof. Dr.-Ing. Tobias Ellermeyer

 Datei: display.c
 Pfad:  zumolib/src/zumo

 Erstellt:    30.06.2019
 Autor:       Tobias Ellermeyer
*/

/**
 @file display.c
 @code #include "zumo/display.h" @endcode

 @brief High-Level Ansteuerung des Grafikdisplays über u8g2-Bibliothek

 Das Display arbeitet mit einem Framebuffer welcher im RAM des STM32 liegt.
 Damit Änderungen auf dem Display sichtbar werden, muss der Befehl
 display_update() aufgerufen werden.

 D.h. es kann immer nur der gesamte Framebuffer vom RAM zum Display übertragen werden.

 @author Tobias Ellermeyer
 */

#include <u8g2.h>
#include "main.h"
#include <zumo/logo128x64.h>

#include <zumo/display.h>

static u8g2_t display_u8g2;			// Display Handle
extern SPI_HandleTypeDef hspi1;		// SPI Handle


static uint8_t u8x8_stm32_gpio_and_delay (U8X8_UNUSED u8x8_t*,U8X8_UNUSED uint8_t, U8X8_UNUSED uint8_t,U8X8_UNUSED void*);
static uint8_t u8x8_byte_4wire_hw_spi (u8x8_t*, uint8_t, uint8_t , void*);

static uint8_t line_no;		// to which line will the next text be printed
// High Level API

volatile uint8_t DMAcompleted=1;

/**
 * @brief Initialisierung des Displays und Anzeige des uC-Mechatronik-Logos
 *
 * **Muss** einmal aufgerufen werden, bevor andere Funktionen benutzt werden
 *
 */
void display_init()
{
	u8g2_Setup_ssd1306_128x64_noname_f(&display_u8g2, U8G2_R0,	u8x8_byte_4wire_hw_spi,u8x8_stm32_gpio_and_delay);
	u8g2_InitDisplay(&display_u8g2);
	u8g2_SetPowerSave(&display_u8g2, 0);
	u8g2_ClearDisplay(&display_u8g2);

	u8g2_DrawXBM(&display_u8g2, 0, 0, logo128x64_width, logo128x64_height, logo128x64_bits);

	u8g2_UpdateDisplay(&display_u8g2);

	u8g2_SetFont(&display_u8g2, u8g2_font_tom_thumb_4x6_mf   ); //u8g2_font_4x6_mf   );  // Select Font

	line_no=0;
}


/**
 * @brief Das Display löschen; ein display_update() wird durchgeführt
 *
 */
void display_clear()
{
	u8g2_ClearDisplay(&display_u8g2);
	u8g2_UpdateDisplay(&display_u8g2);
	line_no=0;
}

/**
 * @brief Das Display löschen **ohne** display_update() durchzuführen
 *
 * D.h. Inhalt bleibt zunächst angezeigt, bis
 * display_update() aufgerufen wird.
 * Neues Bild kann aber im "Hintergrund" schon aufgebaut werden
 */
void display_clear_f()
{
	u8g2_ClearDisplay(&display_u8g2);
	line_no=0;
}

/**
 * @brief Einen String auf das Display an der aktuellen Cursorposition schreiben
 *
 * @param msg String (char*) mit dem Text
 *
 * Am Ende des Textes wird automatisch eine Zeile weiter nach unten an den Beginn der
 * Zeile gesprungen. Ggf. wird der Display-Inhalt auch um eine Zeile **nach oben gescrollt**
 *
 * Newline wird z.Zt nicht unterstützt!
 *
 */
void display_println(char* msg)
{
	char *ptr_s, *ptr_p;
	char tmp;
	// Separate NewLine \n
	ptr_s = msg;
	ptr_p = ptr_s;
/*	do
	{
		if ( ( (*ptr_s) == '\n') || ( (*ptr_s) == '\0')) 	// NewLine found
		{
			// temporary replace with \0
			tmp = (*ptr_s);
			(*ptr_s) = '\0';
			// Draw this part of string to screen, first check if scroll is necessary
			if (line_no==7)  {display_scroll(); }
			  else {line_no++;}

			u8g2_DrawStr(&display_u8g2,0,8*line_no,ptr_p);
			// Revert to NewLine
			(*ptr_s) = tmp;
			// Set Print Pointer to Char after \n
			ptr_p = ptr_s+1;
		}
		// next char
		ptr_s++;
	}
	while ( (*ptr_s) != '\0');
*/
	if (line_no>7)  {display_scroll(); }	// Bugfix last line (29.03.2020 TE)
	  else {line_no++;}

	u8g2_DrawStr(&display_u8g2,0,8*line_no,msg);

	u8g2_UpdateDisplay(&display_u8g2);
}


/**
 * @brief Springe an Text-Position x/y
 *
 * **Wichtig: Im Moment nur y-Position (=Zeile) unterstützt**
 *
 * @param x  z.Zt. nicht unterstützt
 * @param y  Zeile 0...7 des Displays
 *
 */
void display_gotoxy(int x, int y)
{
	if ((y>=0) && (y<8))
	{
		line_no = y;
	}
}

/**
 * @brief Das Display um eine Zeile nach oben scrollen
 */
void display_scroll()
{
	// The Display memory is organized as follows:
	// The 8 bits of a Byte form one column.
	// The first 128 Byte are the upper 128x8 pixel
	uint8_t *src, *dst;
	uint16_t i;

	dst=u8g2_GetBufferPtr(&display_u8g2);
	src = dst + 128;	// Next line


	// Copy 7 lines to one line up
	i=128*7;
	do
	{
		*(dst++) = *(src++);
	}
	while ((--i) > 0);

	// Fill last line with zeros
	i=128;
	do
	{
		*(dst++) = 0x00;
	}
	while ((--i) > 0);

}


/**
 * @brief Eine Linie zeichnen
 *
 * **Wichtig**: Es wird kein display_update() ausgeführt
 *
 * @param x1,y1  Startpunkt (x=0...127; y=0...63)
 * @param x2,y2  Endpunkt
 *
 */
void display_line_f(int x1, int y1, int x2, int y2)
{
	u8g2_DrawLine(&display_u8g2, x1, y1, x2, y2);
}

/**
 * @brief Framebuffer auf das Display übertragen
 *
 * Hierbei wird der gesamte Framebuffer, d.h. 128x64 Pixel = 1024 Bytes
 * übertragen, was im Millisekunden-Bereich dauert
 *
 */
void display_update()
{
	u8g2_UpdateDisplay(&display_u8g2);
}

static uint8_t u8x8_stm32_gpio_and_delay (U8X8_UNUSED u8x8_t *u8x8,U8X8_UNUSED	uint8_t msg, U8X8_UNUSED uint8_t arg_int,U8X8_UNUSED void *arg_ptr)
{
	switch (msg)
	{
		case U8X8_MSG_GPIO_AND_DELAY_INIT:
				HAL_Delay(1);
				break;
		case U8X8_MSG_DELAY_MILLI:
				HAL_Delay(arg_int);
				break;
		case U8X8_MSG_GPIO_DC:
				HAL_GPIO_WritePin(DISP_DC_GPIO_Port, DISP_DC_Pin, arg_int);
				break;
		case U8X8_MSG_GPIO_RESET:
			HAL_GPIO_WritePin(DISP_RST_GPIO_Port, DISP_RST_Pin, arg_int);
			HAL_GPIO_WritePin(DISP_CS_GPIO_Port, DISP_CS_Pin, 1);
			break;
	}
	return 1;
}

static uint8_t u8x8_byte_4wire_hw_spi (u8x8_t *u8x8, uint8_t msg, uint8_t arg_int,	void *arg_ptr)
{
	switch (msg)
	{
		case U8X8_MSG_BYTE_SEND:
				//while (!DMAcompleted);	// Wait for DMA to complete transmission
				DMAcompleted = 0;		// lock for new transmission
				HAL_SPI_Transmit(&hspi1, (uint8_t *) arg_ptr, arg_int,100);
				break;
		case U8X8_MSG_BYTE_INIT:
				break;
		case U8X8_MSG_BYTE_SET_DC:
				//while (!DMAcompleted);	// Wait for DMA to complete transmission
				HAL_GPIO_WritePin(DISP_DC_GPIO_Port, DISP_DC_Pin, arg_int);
				break;
		case U8X8_MSG_BYTE_START_TRANSFER:
				//while (!DMAcompleted);	// Wait for DMA to complete transmission
				HAL_GPIO_WritePin(DISP_CS_GPIO_Port, DISP_CS_Pin, 0);
				break;
		case U8X8_MSG_BYTE_END_TRANSFER:
				//while (!DMAcompleted);	// Wait for DMA to complete transmission
				HAL_GPIO_WritePin(DISP_CS_GPIO_Port, DISP_CS_Pin, 1);
				break;
		default:
			return 0;
	}
	return 1;
}

void display_free_dma()
{
	DMAcompleted = 1;
	HAL_GPIO_TogglePin(LED_USR0_GPIO_Port, LED_USR0_Pin);
}


