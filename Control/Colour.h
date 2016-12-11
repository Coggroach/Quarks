#ifndef COLOUR_H
#define COLOUR_H

typedef unsigned short Colour;

#define   Black			0x0000		/*   0,   0,   0 */
#define   Navy			0x000F      /*   0,   0, 128 */
#define   DarkGreen	0x03E0      /*   0, 128,   0 */
#define   DarkCyan	0x03EF      /*   0, 128, 128 */
#define   Maroon		0x7800      /* 128,   0,   0 */
#define   Purple		0x780F      /* 128,   0, 128 */
#define   Olive			0x7BE0      /* 128, 128,   0 */
#define   LightGray	0xC618      /* 192, 192, 192 */
#define   DarkGray	0x7BEF      /* 128, 128, 128 */
#define   Blue			0x001F      /*   0,   0, 255 */
#define   Green			0x07E0      /*   0, 255,   0 */
#define   Cyan      0x07FF      /*   0, 255, 255 */
#define   Red       0xF800      /* 255,   0,   0 */
#define   Magenta		0xF81F      /* 255,   0, 255 */
#define   Yellow		0xFFE0      /* 255, 255, 0   */
#define   White			0xFFFF      /* 255, 255, 255 */

#define		MuddyWhite 	0x6F5A
#define		Acoppola 		0x4A94
#define		TanahLess 	0x256B
#define		TanahLot 		0x14E7
#define		SpeedingOrange	0x79E8


#define 	RPos	10
#define		GPos 	5
#define		BPos 	0
#define		RMask	(0x1F << RPos)
#define		GMask (0x1F << GPos)
#define 	BMask (0x1F << BPos)

typedef struct ColourPalette
{
	Colour one, two, three, four;	
} ColourPalette;

#endif
