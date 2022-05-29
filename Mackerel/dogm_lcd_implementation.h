/**
 *dogm_lcd_implementation.h
 *
 *Graphics LCD implementation for 128x64 pixel LCDs by STB for ErikZalm/Marlin
 *Demonstrator: http://www.reprap.org/wiki/STB_Electronics
 *License: http://opensource.org/licenses/BSD-3-Clause
 *
 *With the use of:
 *u8glib by Oliver Kraus
 *http://code.google.com/p/u8glib/
 *License: http://opensource.org/licenses/BSD-3-Clause
 */

#ifndef ULTRA_LCD_IMPLEMENTATION_DOGM_H
#define ULTRA_LCD_IMPLEMENTATION_DOGM_H

/**
* Implementation of the LCD display routines for a DOGM128 graphic display. These are common LCD 128x64 pixel graphic displays.
**/

#ifdef ULTIPANEL
#define BLEN_A 0
#define BLEN_B 1
#define BLEN_C 2
#define EN_A (1 << BLEN_A)
#define EN_B (1 << BLEN_B)
#define EN_C (1 << BLEN_C)
#define encrot0 0
#define encrot1 2
#define encrot2 3
#define encrot3 1
#define LCD_CLICKED (buttons & EN_C)
#endif

#include <U8glib.h>
#include "DOGMbitmaps.h"
#include "dogm_font_data_marlin.h"
#include "ultralcd.h"
#include "ultralcd_st7920_u8glib_rrd.h"

/* Russian language not supported yet, needs custom font

#if LANGUAGE_CHOICE == 6
#include "LiquidCrystalRus.h"
#define LCD_CLASS LiquidCrystalRus
#else
#include <LiquidCrystal.h>
#define LCD_CLASS LiquidCrystal
#endif
*/

// DOGM parameters (size in pixels)
#define DOG_CHAR_WIDTH 6
#define DOG_CHAR_HEIGHT 12
#define DOG_CHAR_WIDTH_LARGE 9
#define DOG_CHAR_HEIGHT_LARGE 18

#define START_ROW 0

/* Custom characters defined in font font_6x10_marlin.c */
#define LCD_STR_BEDTEMP "\xFE"
#define LCD_STR_DEGREE "\xB0"
#define LCD_STR_THERMOMETER "\xFF"
#define LCD_STR_UPLEVEL "\xFB"
#define LCD_STR_REFRESH "\xF8"
#define LCD_STR_FOLDER "\xF9"
#define LCD_STR_FEEDRATE "\xFD"
#define LCD_STR_CLOCK "\xFC"
#define LCD_STR_ARROW_RIGHT "\xFA"

#define FONT_STATUSMENU u8g_font_6x9

int lcd_contrast;

// LCD selection
#ifdef U8GLIB_ST7920
//U8GLIB_ST7920_128X64_RRD u8g(0,0,0);
U8GLIB_ST7920_128X64_RRD u8g(0);
#elif defined(MAKRPANEL)
// The MaKrPanel display, ST7565 controller as well
U8GLIB_NHD_C12864 u8g(DOGLCD_CS, DOGLCD_A0);
#else
// for regular DOGM128 display with HW-SPI
U8GLIB_DOGM128 u8g(DOGLCD_CS, DOGLCD_A0); // HW-SPI Com: CS, A0
#endif

static void lcd_implementation_greet()
{
}

static void lcd_implementation_init()
{
#ifdef LCD_PIN_BL
	pinMode(LCD_PIN_BL, OUTPUT); // Enable LCD backlight
	digitalWrite(LCD_PIN_BL, HIGH);
#endif

	u8g.setContrast(lcd_contrast);
	//  Uncomment this if you have the first generation (V1.10) of STBs board
	//  pinMode(17, OUTPUT);	// Enable LCD backlight
	//  digitalWrite(17, HIGH);

	u8g.firstPage();
	do
	{
		u8g.setFont(u8g_font_6x10_marlin);
		u8g.setColorIndex(1);
		u8g.drawBox(0, 0, u8g.getWidth(), u8g.getHeight());
		u8g.setColorIndex(1);
	} while (u8g.nextPage());

#ifdef LCD_SCREEN_ROT_90
	u8g.setRot90(); // Rotate screen by 90Â°
#endif

#ifdef LCD_SCREEN_ROT_180
	u8g.setRot180(); // Rotate screen by 180Â°
#endif

#ifdef LCD_SCREEN_ROT_270
	u8g.setRot270(); // Rotate screen by 270Â°
#endif

	u8g.firstPage();
	do
	{
		u8g.setFont(u8g_font_5x8);
		u8g.drawStr(5, 20, SPLASH1);
		u8g.setFont(u8g_font_5x8);
		u8g.drawStr(5, 30, SPLASH2);
		u8g.setFont(u8g_font_5x8);
		u8g.drawStr(5, 40, SPLASH3);
	} while (u8g.nextPage());
}

static void lcd_implementation_clear()
{
	// NO NEED TO IMPLEMENT LIKE SO. Picture loop automatically clears the display.
	//
	// Check this article: http://arduino.cc/forum/index.php?topic=91395.25;wap2
	//
	//	u8g.firstPage();
	//	do {
	//			u8g.setColorIndex(0);
	//			u8g.drawBox (0, 0, u8g.getWidth(), u8g.getHeight());
	//			u8g.setColorIndex(1);
	//		} while( u8g.nextPage() );
}

/* Arduino < 1.0.0 is missing a function to print PROGMEM strings, so we need to implement our own */
static void lcd_printPGM(const char *str)
{
	char c;
	while ((c = pgm_read_byte(str++)) != '\0')
	{
		u8g.print(c);
	}
}

static void lcd_implementation_status_screen_nozzle_temperature(int row, int column)
{
	int tHotend = int(degHotend(0) + 0.5);
	int tTarget = int(degTargetHotend(0) + 0.5);

	u8g.setFont(FONT_STATUSMENU);
	u8g.setPrintPos(row, column);
	u8g.print(itostr3(tHotend));
	u8g.print('/');
	u8g.print(itostr3left(tTarget));
	lcd_printPGM(PSTR(LCD_STR_DEGREE " "));
}

static void lcd_implementation_status_screen_extruder_rpm(int row, int column)
{
	u8g.setPrintPos(row, column);
	if ((extrude_status & ES_SWITCH_SET) && (extrude_status & ES_HOT_SET)) //check if extruder motor switch is on
	{
		u8g.print(ftostr22(extruder_rpm)); //convert to rpm
		lcd_printPGM(PSTR(" rpm"));
	}
	else if (extrude_status & ES_HOT_SET)
	{
		lcd_printPGM(PSTR("OFF"));
	}
	else
	{
		lcd_printPGM(PSTR("COLD"));
	}
}

static void lcd_implementation_status_screen_puller_feedrate(int row, int column)
{
	u8g.setFont(u8g_font_6x10_marlin);
	u8g.setPrintPos(row + 2, column);
	u8g.print(LCD_STR_FEEDRATE[0]);
	u8g.setFont(FONT_STATUSMENU);
	u8g.setPrintPos(row + 9, column);
	u8g.print(ftostr22(puller_feedrate)); //give the feed rate in mm/sec
}

static void lcd_implementation_status_screen_puller_rpm(int row, int column)
{
	u8g.setPrintPos(row, column);
	u8g.print(ftostr22(puller_feedrate * (60.0 / PULLER_WHEEL_CIRC)));
	lcd_printPGM(PSTR(" rpm"));
}

static void lcd_implementation_status_screen_sensor_data(int row, int column)
{
	u8g.setPrintPos(row, column);
#ifdef FILAMENT_SENSOR
	u8g.print('d');
	u8g.print(ftostr12(current_filwidth));
#endif
#ifdef BLOB_SENSOR
	lcd_printPGM(PSTR(" b"));
	u8g.print(ftostr12(current_blobwidth));
#else
#ifdef FILAMENT_SENSOR
	if (alt_cnt < 5)
	{
		lcd_printPGM(PSTR(" Av: "));
		u8g.print(ftostr12(avg_measured_filament_width));
	};
	if (alt_cnt >= 5 && alt_cnt < 10)
	{
		lcd_printPGM(PSTR(" Mx: "));
		u8g.print(ftostr12(max_measured_filament_width));
	};
	if (alt_cnt >= 10 && alt_cnt < 15)
	{
		lcd_printPGM(PSTR(" Mn: "));
		u8g.print(ftostr12(min_measured_filament_width));
	};
	alt_cnt = alt_cnt + 1;
	if (alt_cnt == 15)
		alt_cnt = 0;
#else
	lcd_printPGM(PSTR("-"));
#endif //FILAMENT_SENSOR
#endif //BLOB_SENSOR
	lcd_printPGM(PSTR(" mm"));
}

static void lcd_implementation_status_screen_filament_status_bar(int leftColumnStart, int rightColumnStart, int row)
{
	uint16_t time;

	u8g.setColorIndex(1); // black on white
	u8g.drawBox(0, 42, 128, DOG_CHAR_HEIGHT + 1);
	
	//Time
	//=Elapsed time
	u8g.setColorIndex(0); // following text must be white on black
	u8g.setFont(u8g_font_6x10_marlin);
	u8g.setPrintPos(leftColumnStart-2 * DOG_CHAR_WIDTH + 3, row + 1);
	u8g.print(LCD_STR_CLOCK[0]);
	u8g.setFont(FONT_STATUSMENU);
	u8g.setPrintPos(leftColumnStart, row);
	time = duration / 60000;
	u8g.print(itostr2(time / 60));
	u8g.print(':');
	u8g.print(itostr2(time % 60));

	//Filament length
	//=Lenght icon
	const int iconX = rightColumnStart - 12;
	const int iconY = row-3;

	u8g.drawPixel(iconX, iconY);
	u8g.drawPixel(iconX, iconY + 2);
	u8g.drawPixel(iconX + 9, iconY);
	u8g.drawPixel(iconX + 9, iconY + 2);
	u8g.drawPixel(iconX + 1, iconY + 1);
	u8g.drawPixel(iconX + 2, iconY + 1);
	u8g.drawPixel(iconX + 3, iconY + 1);
	u8g.drawPixel(iconX + 4, iconY + 1);
	u8g.drawPixel(iconX + 5, iconY + 1);
	u8g.drawPixel(iconX + 6, iconY + 1);
	u8g.drawPixel(iconX + 7, iconY + 1);
	u8g.drawPixel(iconX + 8, iconY + 1);
	//=Lenght icon end

	u8g.setPrintPos(rightColumnStart, row);
	u8g.print(ftostr6(extrude_length));
	lcd_printPGM(PSTR(" mm"));

	u8g.setColorIndex(1);
}

static void lcd_implementation_status_screen()
{
	const int leftColumnStart = 12;
	const int rightColumnStart = 70;

	u8g.setColorIndex(1); // black on white

	u8g.drawBitmapP(1, 1, STATUS_SCREENBYTEWIDTH, STATUS_SCREENHEIGHT, status_screen0_bmp);

	//Extruder
	//=nozzle temperature
	lcd_implementation_status_screen_nozzle_temperature(leftColumnStart, 10);

	//=RPM
	lcd_implementation_status_screen_extruder_rpm(rightColumnStart, 10);

	//Puller
	lcd_implementation_status_screen_puller_feedrate(leftColumnStart, 25);

	lcd_implementation_status_screen_puller_rpm(rightColumnStart, 25);

	//Filament
	//=Sensor
	lcd_implementation_status_screen_sensor_data(leftColumnStart, 37);

	//Filament extrusion status
	lcd_implementation_status_screen_filament_status_bar(leftColumnStart, rightColumnStart, 51);

	// Status line
	u8g.setFont(FONT_STATUSMENU);
	u8g.setPrintPos(0, 63);
	u8g.print(lcd_status_message);
}

static void lcd_implementation_drawmenu_generic(uint8_t row, const char *pstr, char pre_char, char post_char)
{
	char c;

	uint8_t n = LCD_WIDTH - 1 - 2;

	if ((pre_char == '>') || (pre_char == LCD_STR_UPLEVEL[0]))
	{
		u8g.setColorIndex(1); // black on white
		u8g.drawBox(0, row * DOG_CHAR_HEIGHT + 3, 128, DOG_CHAR_HEIGHT);
		u8g.setColorIndex(0); // following text must be white on black
	}
	else
		u8g.setColorIndex(1); // unmarked text is black on white

	u8g.setPrintPos(0 * DOG_CHAR_WIDTH, (row + 1) * DOG_CHAR_HEIGHT);
	if (pre_char != '>')
		u8g.print(pre_char);
	else
		u8g.print(' '); // Row selector is obsolete

	while ((c = pgm_read_byte(pstr)) != '\0')
	{
		u8g.print(c);
		pstr++;
		n--;
	}
	while (n--)
	{
		u8g.print(' ');
	}

	u8g.print(post_char);
	u8g.print(' ');
	u8g.setColorIndex(1); // restore settings to black on white
}

static void lcd_implementation_drawmenu_setting_edit_generic(uint8_t row, const char *pstr, char pre_char, char *data)
{
	static unsigned int fkt_cnt = 0;
	char c;
	uint8_t n = LCD_WIDTH - 1 - 2 - strlen(data);

	u8g.setPrintPos(0 * DOG_CHAR_WIDTH, (row + 1) * DOG_CHAR_HEIGHT);
	u8g.print(pre_char);

	while ((c = pgm_read_byte(pstr)) != '\0')
	{
		u8g.print(c);

		pstr++;
		n--;
	}

	u8g.print(':');

	while (n--)
	{
		u8g.print(' ');
	}

	u8g.print(data);
}

static void lcd_implementation_drawmenu_setting_edit_generic_P(uint8_t row, const char *pstr, char pre_char, const char *data)
{
	char c;
	uint8_t n = LCD_WIDTH - 1 - 2 - strlen_P(data);

	u8g.setPrintPos(0 * DOG_CHAR_WIDTH, (row + 1) * DOG_CHAR_HEIGHT);
	u8g.print(pre_char);

	while ((c = pgm_read_byte(pstr)) != '\0')
	{
		u8g.print(c);

		pstr++;
		n--;
	}

	u8g.print(':');

	while (n--)
	{
		u8g.print(' ');
	}

	lcd_printPGM(data);
}

#define lcd_implementation_drawmenu_setting_edit_int3_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', itostr3(*(data)))
#define lcd_implementation_drawmenu_setting_edit_int3(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', itostr3(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float3_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr3(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float3(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr3(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float32_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr32(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float32(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr32(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float22_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr22(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float22(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr22(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float5_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr5(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float5(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr5(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float52_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr52(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float52(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr52(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float53_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr53(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float53(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr53(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float51_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr51(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float51(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr51(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float6_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr6(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float6(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr6(*(data)))
#define lcd_implementation_drawmenu_setting_edit_long5_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr5(*(data)))
#define lcd_implementation_drawmenu_setting_edit_long5(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr5(*(data)))
#define lcd_implementation_drawmenu_setting_edit_bool_selected(row, pstr, pstr2, data) lcd_implementation_drawmenu_setting_edit_generic_P(row, pstr, '>', (*(data)) ? PSTR(MSG_ON) : PSTR(MSG_OFF))
#define lcd_implementation_drawmenu_setting_edit_bool(row, pstr, pstr2, data) lcd_implementation_drawmenu_setting_edit_generic_P(row, pstr, ' ', (*(data)) ? PSTR(MSG_ON) : PSTR(MSG_OFF))

//Add version for callback functions
#define lcd_implementation_drawmenu_setting_edit_callback_int3_selected(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', itostr3(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_int3(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', itostr3(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float3_selected(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr3(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float3(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr3(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float32_selected(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr32(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float32(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr32(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float22_selected(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr22(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float22(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr22(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float5_selected(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr5(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float5(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr5(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float53_selected(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr53(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float53(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr53(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float52_selected(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr52(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float52(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr52(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float51_selected(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr51(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float51(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr51(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float6_selected(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr6(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float6(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr6(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_long5_selected(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr5(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_long5(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr5(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_bool_selected(row, pstr, pstr2, data, callback) lcd_implementation_drawmenu_setting_edit_generic_P(row, pstr, '>', (*(data)) ? PSTR(MSG_ON) : PSTR(MSG_OFF))
#define lcd_implementation_drawmenu_setting_edit_callback_bool(row, pstr, pstr2, data, callback) lcd_implementation_drawmenu_setting_edit_generic_P(row, pstr, ' ', (*(data)) ? PSTR(MSG_ON) : PSTR(MSG_OFF))

void lcd_implementation_drawedit(const char *pstr, char *value)
{
	u8g.setPrintPos(0 * DOG_CHAR_WIDTH_LARGE, (u8g.getHeight() - 1 - DOG_CHAR_HEIGHT_LARGE) - (1 * DOG_CHAR_HEIGHT_LARGE) - START_ROW);
	u8g.setFont(u8g_font_6x10_marlin);
	lcd_printPGM(pstr);
	u8g.print(':');
	u8g.setPrintPos((14 - strlen(value)) * DOG_CHAR_WIDTH_LARGE, (u8g.getHeight() - 1 - DOG_CHAR_HEIGHT_LARGE) - (1 * DOG_CHAR_HEIGHT_LARGE) - START_ROW);
	u8g.print(value);
}

static void lcd_implementation_drawmenu_sdfile_selected(uint8_t row, const char *pstr, const char *filename, char *longFilename)
{
	char c;
	uint8_t n = LCD_WIDTH - 1;

	if (longFilename[0] != '\0')
	{
		filename = longFilename;
		longFilename[LCD_WIDTH - 1] = '\0';
	}

	u8g.setColorIndex(1); // black on white
	u8g.drawBox(0, row * DOG_CHAR_HEIGHT + 3, 128, DOG_CHAR_HEIGHT);
	u8g.setColorIndex(0); // following text must be white on black
	u8g.setPrintPos(0 * DOG_CHAR_WIDTH, (row + 1) * DOG_CHAR_HEIGHT);
	u8g.print(' '); // Indent by 1 char

	while ((c = *filename) != '\0')
	{
		u8g.print(c);
		filename++;
		n--;
	}
	while (n--)
	{
		u8g.print(' ');
	}
	u8g.setColorIndex(1); // black on white
}

static void lcd_implementation_drawmenu_sdfile(uint8_t row, const char *pstr, const char *filename, char *longFilename)
{
	char c;
	uint8_t n = LCD_WIDTH - 1;

	if (longFilename[0] != '\0')
	{
		filename = longFilename;
		longFilename[LCD_WIDTH - 1] = '\0';
	}

	u8g.setPrintPos(0 * DOG_CHAR_WIDTH, (row + 1) * DOG_CHAR_HEIGHT);
	u8g.print(' ');

	while ((c = *filename) != '\0')
	{
		u8g.print(c);

		filename++;
		n--;
	}
	while (n--)
	{
		u8g.print(' ');
	}
}

static void lcd_implementation_drawmenu_sddirectory_selected(uint8_t row, const char *pstr, const char *filename, char *longFilename)
{
	char c;
	uint8_t n = LCD_WIDTH - 2;

	if (longFilename[0] != '\0')
	{
		filename = longFilename;
		longFilename[LCD_WIDTH - 2] = '\0';
	}
	u8g.setColorIndex(1); // black on white
	u8g.drawBox(0, row * DOG_CHAR_HEIGHT + 3, 128, DOG_CHAR_HEIGHT);
	u8g.setColorIndex(0); // following text must be white on black
	u8g.setPrintPos(0 * DOG_CHAR_WIDTH, (row + 1) * DOG_CHAR_HEIGHT);
	u8g.print(' '); // Indent by 1 char
	u8g.print(LCD_STR_FOLDER[0]);

	while ((c = *filename) != '\0')
	{
		u8g.print(c);

		filename++;
		n--;
	}
	while (n--)
	{
		u8g.print(' ');
	}
	u8g.setColorIndex(1); // black on white
}

static void lcd_implementation_drawmenu_sddirectory(uint8_t row, const char *pstr, const char *filename, char *longFilename)
{
	char c;
	uint8_t n = LCD_WIDTH - 2;

	if (longFilename[0] != '\0')
	{
		filename = longFilename;
		longFilename[LCD_WIDTH - 2] = '\0';
	}

	u8g.setPrintPos(0 * DOG_CHAR_WIDTH, (row + 1) * DOG_CHAR_HEIGHT);
	u8g.print(' ');
	u8g.print(LCD_STR_FOLDER[0]);

	while ((c = *filename) != '\0')
	{
		u8g.print(c);

		filename++;
		n--;
	}
	while (n--)
	{
		u8g.print(' ');
	}
}

#define lcd_implementation_drawmenu_back_selected(row, pstr, data) lcd_implementation_drawmenu_generic(row, pstr, LCD_STR_UPLEVEL[0], LCD_STR_UPLEVEL[0])
#define lcd_implementation_drawmenu_back(row, pstr, data) lcd_implementation_drawmenu_generic(row, pstr, ' ', LCD_STR_UPLEVEL[0])
#define lcd_implementation_drawmenu_submenu_selected(row, pstr, data) lcd_implementation_drawmenu_generic(row, pstr, '>', LCD_STR_ARROW_RIGHT[0])
#define lcd_implementation_drawmenu_submenu(row, pstr, data) lcd_implementation_drawmenu_generic(row, pstr, ' ', LCD_STR_ARROW_RIGHT[0])
#define lcd_implementation_drawmenu_gcode_selected(row, pstr, gcode) lcd_implementation_drawmenu_generic(row, pstr, '>', ' ')
#define lcd_implementation_drawmenu_gcode(row, pstr, gcode) lcd_implementation_drawmenu_generic(row, pstr, ' ', ' ')
#define lcd_implementation_drawmenu_function_selected(row, pstr, data) lcd_implementation_drawmenu_generic(row, pstr, '>', ' ')
#define lcd_implementation_drawmenu_function(row, pstr, data) lcd_implementation_drawmenu_generic(row, pstr, ' ', ' ')

static void lcd_implementation_quick_feedback()
{

#if BEEPER > -1
	SET_OUTPUT(BEEPER);
	for (int8_t i = 0; i < 10; i++)
	{
		WRITE(BEEPER, HIGH);
		delay(3);
		WRITE(BEEPER, LOW);
		delay(3);
	}
#endif
}
#endif //ULTRA_LCD_IMPLEMENTATION_DOGM_H
