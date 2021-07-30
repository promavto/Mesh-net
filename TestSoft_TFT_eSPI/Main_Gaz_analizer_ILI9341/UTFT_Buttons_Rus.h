

#ifndef UTFT_Buttons_Rus_h
#define UTFT_Buttons_Rus_h

#include "CONFIG.h"

#if DISPLAY_USED == DISPLAY_ILI9341

#include <UTFT.h>
#include <URTouch_ILI9341.h>

#define UTFT_BUTTONS_VERSION	103

#if defined(__AVR__)
	#include "Arduino.h"
#elif defined(__PIC32MX__)
	#include "WProgram.h"
#elif defined(__arm__)
	#include "Arduino.h"
#endif

#include "CONFIG.h"

#define MAX_BUTTONS	20	// Maximum number of buttons available at one time

// Define presets for button status
#define BUTTON_DISABLED			0x0001
#define BUTTON_SYMBOL			0x0002
#define BUTTON_SYMBOL_REP_3X	0x0004
#define BUTTON_BITMAP			0x0008	
#define BUTTON_NO_BORDER		0x0010
#define BUTTON_UNUSED			0x8000
#define BUTTON_HAS_BACK_COLOR 0x0020
#define BUTTON_HAS_FONT_COLOR 0x0040
#define BUTTON_VISIBLE 0x0080
#define BUTTON_SELECTED 0x0100

class UTFT_Buttons_Rus;

typedef struct
{
  int16_t			pos_x, pos_y, width, height;
  word backColor, fontColor;
  uint16_t			flags;
  const char				*label;
  bitmapdatatype	data;
} button_type;

typedef void (*DrawButtonsUpdateFunc)(void);
typedef void (*OnCheckButtonsFunc)(int button);
typedef int (*PrusPrintFunc)(const char* st,int x, int y, int deg, bool computeStringLengthOnly);

class UTFT_Buttons_Rus
{
	public:
		UTFT_Buttons_Rus(UTFT* _tft, URTouch* _touch, PrusPrintFunc rusPrinter);

		int		addButton(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const char *label, uint16_t flags=0);
		int		addButton(uint16_t x, uint16_t y, uint16_t width, uint16_t height, bitmapdatatype data, uint16_t flags=0);
		void	drawButtons(DrawButtonsUpdateFunc func=NULL);
		void	drawButton(int buttonID);
		void	enableButton(int buttonID, boolean redraw=false);
		void	disableButton(int buttonID, boolean redraw=false);
		void	relabelButton(int buttonID, const char *label, boolean redraw=false);
		boolean	buttonEnabled(int buttonID);
		void	deleteButton(int buttonID);
		void	deleteAllButtons();
		int		checkButtons(OnCheckButtonsFunc func = NULL);
		void	setTextFont(uint8_t* font);
		void	setSymbolFont(uint8_t* font);
		void	setButtonColors(word atxt, word iatxt, word brd, word brdhi, word back);
    void selectButton(int buttonID, bool selected, boolean redraw=false);
		void 	setButtonBackColor(int buttonID, word color);
		void 	setButtonFontColor(int buttonID, word color);
    void showButton(int buttonID, boolean redraw=false);
    void hideButton(int buttonID, boolean redraw=false);
    const char* getLabel(int buttonID);

	protected:
		UTFT		*_UTFT;
		URTouch		*_URTouch;

    PrusPrintFunc pRusPrinter;
    
		button_type	buttons[MAX_BUTTONS];
		word		_color_text, _color_text_inactive, _color_background, _color_border, _color_hilite;
		uint8_t		*_font_text, *_font_symbol;
};

#endif

#endif
