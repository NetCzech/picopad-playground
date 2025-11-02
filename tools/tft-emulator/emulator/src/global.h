// ****************************************************************************
//
//                         Global common C definitions
//
// ****************************************************************************
// https://github.com/Pajenicko/Picopad/blob/main/sdk/global.h

#ifndef _GLOBAL_H
#define _GLOBAL_H

#include <cstdint>

// Display
#ifndef WIDTH
#define WIDTH	320			// display width
#endif

#ifndef HEIGHT
#define HEIGHT	240			// display height
#endif

#ifndef FRAMESIZE
#define FRAMESIZE (WIDTH*HEIGHT) 	// frame size in number of colors
#endif

#ifndef FONT
#define FONT		FontBold8x16	// default system font
#endif

#ifndef FONTW
#define FONTW		8		// width of system font
#endif

#ifndef FONTH
#define FONTH		16		// height of system font
#endif

#define COLBITS 16
#define COLTYPE		u16
#define DispStripInx  0		// current index of back buffer strip
#define DispMinY  0		// minimal Y; base of back buffer strip
#define DispMaxY HEIGHT		// maximal Y + 1; end of back buffer strip
#define FRAMETYPE	u16		// type of frame entry: u8 or u16
#define WIDTHLEN	WIDTH		// length of one line of one plane, in number of frame elements
#define	DISP_STRIP_NUM	1		// number of back strips

// request to use inline
#define INLINE __attribute__((always_inline)) inline

typedef unsigned char Bool;
#define True 1
#define False 0

// base types - to check type size, use CheckTypeSize()
typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;		// on 8-bit system use "signed int"
typedef unsigned short u16;		// on 8-bit system use "unsigned int"
typedef signed long int s32;		// on 64-bit system use "signed int"
typedef unsigned long int u32;		// on 64-bit system use "unsigned int"
typedef signed long long int s64;
typedef unsigned long long int u64;

typedef unsigned int uint;

typedef u16 uint16_t;		// used only by PIO program

// Sound
#ifndef USE_CONFIG
#define USE_CONFIG 1
#endif

#ifndef USE_PWMSND
#define USE_PWMSND 2
#endif

#if USE_CONFIG
  #define CONFIG_VOLUME_STEP      10
  #define CONFIG_VOLUME_FULL      10
  #define CONFIG_VOLUME_FULLSTEP  (CONFIG_VOLUME_FULL * CONFIG_VOLUME_STEP)
  #define CONFIG_VOLUME_MAX       25

  typedef struct {
      u8 volume;  // 0..CONFIG_VOLUME_FULLSTEP
  } sConfig;

  extern sConfig Config;

  void ConfigSetVolume(int level);
  void ConfigIncVolume();
  void ConfigDecVolume();
#endif

// ----------------------------------------------------------------------------
//                               Constants
// ----------------------------------------------------------------------------

#define	B0 (1<<0)
#define	B1 (1<<1)
#define	B2 (1<<2)
#define	B3 (1<<3)
#define	B4 (1<<4)
#define	B7 (1<<7)
#define	B8 (1U<<8)
#define	B9 (1U<<9)
#define	B10 (1U<<10)
#define	B12 (1U<<12)
#define	B13 (1U<<13)
#define	B14 (1U<<14)
#define	B15 (1U<<15)

#define FRACT		12	// number of bits of fractional part of fractint number (use max. 13, min. 8)
#define FRACTMUL (1<<FRACT)	// - update vga_config.h too

// count of array entries
#define count_of(a) (sizeof(a)/sizeof((a)[0]))

#define COLOR(r,g,b)	( (((r)&0xf8)<<8) | (((g)&0xfc)<<3) | (((b)&0xf8)>>3) )

//// 16-bit color definitions
// - base colors
#define COL_BLACK	COLOR(  0,  0,  0)
#define COL_BLUE	COLOR(  0,  0,255)
#define COL_GREEN	COLOR(  0,255,  0)
#define COL_CYAN	COLOR(  0,255,255)
#define COL_RED		COLOR(255,  0,  0)
#define COL_MAGENTA	COLOR(255,  0,255)
#define COL_YELLOW	COLOR(255,255,  0)
#define COL_WHITE	COLOR(255,255,255)
#define COL_GRAY	COLOR(128,128,128)
// - dark colors
#define COL_DKBLUE	COLOR(  0,  0,127)
#define COL_DKGREEN	COLOR(  0,127,  0)
#define COL_DKCYAN	COLOR(  0,127,127)
#define COL_DKRED	COLOR(127,  0,  0)
#define COL_DKMAGENTA	COLOR(127,  0,127)
#define COL_DKYELLOW	COLOR(127,127,  0)
#define COL_DKWHITE	COLOR(127,127,127)
#define COL_DKGRAY	COLOR( 64, 64, 64)
// - light colors
#define COL_LTBLUE	COLOR(127,127,255)
#define COL_LTGREEN	COLOR(127,255,127)
#define COL_LTCYAN	COLOR(127,255,255)
#define COL_LTRED	COLOR(255,127,127)
#define COL_LTMAGENTA	COLOR(255,127,255)
#define COL_LTYELLOW	COLOR(255,255,127)
#define COL_LTGRAY	COLOR(192,192,192)

#define COL_AZURE	COLOR(  0,127,255)
#define COL_ORANGE	COLOR(255,127,  0)


#define PI 3.14159265358979324

#endif // _GLOBAL_H