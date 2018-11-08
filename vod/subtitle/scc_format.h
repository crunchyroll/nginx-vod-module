#ifndef __SCC_FORMAT_H__
#define __SCC_FORMAT_H__

// includes
#include "../media_format.h"

#define VALIGN_SUB 0
#define VALIGN_CENTER 8
#define VALIGN_TOP 4

#define FFMAX(a,b) ((a) > (b) ? (a) : (b))
#define FFMIN(a,b) ((a) > (b) ? (b) : (a))
#define FFMINMAX(c,a,b) FFMIN(FFMAX(c, a), b)


#define SCC_608_SCREEN_WIDTH  32
#define SCC_NUM_OF_STYLES_INSERTED    10
#define SCC_UNUSED_CHAR  0

// globals
extern media_format_t scc_format;

enum cc_text_done
{
    EVENT_TEXT_OPEN = 0,
    EVENT_TEXT_DONE = 1
};

/*
 * scc_event corresponds to a single Dialogue line;
 * text is stored as-is, style overrides gets applied later.
 */
typedef struct scc_event {
	unsigned char      characters[15][33]; // Extra char at the end for potential '\n'
	unsigned char      italic    [15][33];
	unsigned char      underline [15][33];
	         char      row_used  [15];     // Any data in row?
	unsigned char      color;
	unsigned char      font;
	unsigned char      bk_color;
	int                len_text;           // number of visible characters added to this screen
	enum cc_text_done  event_text_done;    // when set to EVENT_TEXT_DONE, no further text is added. EOC was received already.

    long long          start_time;         // ms
    long long          end_time;           // ms


    // TODO: remove when parsed correctly
    int         Style;
    int         MarginL;
    int         MarginR;
    int         MarginV;
    char       *Text;
} scc_event_t;

/*
 * scc_track represents a fully parsed SCC file.
 * It is entirely parsed before events are rendered into WebVTT cues.
 */
typedef struct scc_track {
    long long       maxDuration;          // ms, added for needs of the vod-module

    int             n_events;
    int             max_events;
    scc_event_t    *events;

    long long       cue_time;             // ms
	int             cursor_row, cursor_column;
	unsigned char   last_c1, last_c2;
	unsigned char   current_color;        // Color we are currently using to write
	unsigned char   current_bk_color;     // Background color we are currently using to write
	unsigned char   current_italic;       // Italic flag we are currently using to write
	unsigned char   current_underline;    // Underline flag we are currently using to write

    // TODO: remove when parsed correctly
    int             PlayResX;
    int             PlayResY;
} scc_track_t;

scc_track_t *scc_parse_memory(char *data, int length, request_context_t* request_context);

enum scc_alignment
{
    SCC_ALIGN_CENTER = 0,
    SCC_ALIGN_LEFT   = 1,
    SCC_ALIGN_RIGHT  = 2
} scc_alignment_t;

enum scc_color_code
{
	COL_WHITE       = 0,
	COL_GREEN       = 1,
	COL_BLUE        = 2,
	COL_CYAN        = 3,
	COL_RED         = 4,
	COL_YELLOW      = 5,
	COL_MAGENTA     = 6,
	COL_USERDEFINED = 7,
	COL_BLACK       = 8,
	COL_TRANSPARENT = 9
} scc_color_code_t;

enum font_bits
{
	FONT_REGULAR            = 0,
	FONT_ITALICS            = 1,
	FONT_UNDERLINED         = 2,
	FONT_UNDERLINED_ITALICS = 3
};

enum command_code
{
	COM_UNKNOWN = 0,
	COM_ERASEDISPLAYEDMEMORY = 1,
	COM_RESUMECAPTIONLOADING = 2,
	COM_ENDOFCAPTION = 3,
	COM_TABOFFSET1 = 4,
	COM_TABOFFSET2 = 5,
	COM_TABOFFSET3 = 6,
	COM_ROLLUP2 = 7,
	COM_ROLLUP3 = 8,
	COM_ROLLUP4 = 9,
	COM_CARRIAGERETURN = 10,
	COM_ERASENONDISPLAYEDMEMORY = 11,
	COM_BACKSPACE = 12,
	COM_RESUMETEXTDISPLAY = 13,
	COM_ALARMOFF =14,
	COM_ALARMON = 15,
	COM_DELETETOENDOFROW = 16,
	COM_RESUMEDIRECTCAPTIONING = 17,
	// Non existing commands we insert to have the decoder
	// special stuff for us.
	COM_FAKE_RULLUP1 = 18
};

#endif //__SCC_FORMAT_H__


/*
Scenarist_SCC V1.0

00:00:08;16	942c

00:00:08;21	9420 91d0 b320 4631 94f4 97a1 4fce 20d9 4fd5 5220 cdc1 52cb aeae ae80 942c 8080 8080 942f

00:00:10;22	942c

00:00:11;28	9420 94f4 9723 c745 5420 d345 54ae aeae 942c 8080 8080 942f

00:00:13;10	942c

00:00:14;05	9420 9476 9723 c74f a180 942c 8080 8080 942f

00:00:15;02	9420 9452 54e9 e6e6 616e 79ba 2054 c845 d345 2054 574f 2054 45c1 cdd3 9470 9723 c152 4520 5452 d949 cec7 2054 4f20 c245 2054 c845 2046 4952 d354 942c 8080 8080 942f

00:00:18;01	9420 9452 97a2 544f 2043 c8d5 43cb 2054 c845 4952 2043 4c4f d5c4 9470 9723 c1ce c420 c452 45ce 43c8 2054 c845 4952 2054 45c1 cdcd c154 45d3 942c 8080 8080 942f

00:00:20;23	9420 94d0 97a1 5749 54c8 20c1 4c4c 2054 c845 20c7 4f4f dad9 2c20 c7c1 cbd9 2043 4f4c 4f52 d380 94f4 97a1 4f46 2054 c845 2052 c149 cec2 4f57 942c 8080 8080 942f

00:00:23;21	9420 9452 97a1 464f 5220 434f ce54 524f 4c20 4f46 2052 4fd5 cec4 2031 94f4 9723 4fce 2054 c845 20c7 c1cd 4580 942c 8080 8080 942f

00:00:25;24	9420 9452 97a2 57c8 4552 4520 c120 c74f 4fc4 20a2 d34c 49cd 45a2 94f4 49d3 20c8 c1c4 20c2 d920 c14c 4c20 adad 942c 8080 8080 942f

00:00:28;11	9420 94f2 9723 a2c4 4fd5 c24c 4520 c4c1 5245 2032 b0b0 b0a2 a180 942c 8080 8080 942f

00:00:31;27	9420 94f2 97a2 4a61 73ef 6eba 20c8 45c1 d649 cec7 2054 c845 cdae 942c 8080 8080 942f

00:00:33;10	9420 94f2 9723 c845 2046 49ce c14c 4cd9 20c7 4f54 204f ce45 ae80 942c 8080 8080 942f

00:00:34;18	9420 9470 9723 c845 c1d6 49ce c720 54c8 45cd a120 4fc8 2c20 d34f 2043 4c4f d345 a180 942c 8080 8080 942f

00:00:36;06	9420 9452 9723 c845 20cd 49c7 c854 20cb ce4f 43cb 204f d645 5280 94f4 97a1 54c8 4520 57c8 4f4c 4520 54c8 49ce c780 942c 8080 8080 942f

**************************

1
00:00:09,300 --> 00:00:10,766
ON YOUR MARK...

2
00:00:12,333 --> 00:00:13,366
GET SET...

3
00:00:14,466 --> 00:00:16,066
GO!

4
00:00:16,166 --> 00:00:19,000
Tiffany: THESE TWO TEAMS
ARE TRYING TO BE THE FIRST

5
00:00:19,100 --> 00:00:21,733
TO CHUCK THEIR CLOUD
AND DRENCH THEIR TEAMMATES

6
00:00:21,833 --> 00:00:24,466
WITH ALL THE GOOZY, GAKY COLORS
OF THE RAINBOW

7
00:00:24,566 --> 00:00:26,566
FOR CONTROL OF ROUND 1
ON THE GAME

8
00:00:26,666 --> 00:00:28,833
WHERE A GOOD "SLIME"
IS HAD BY ALL --

9
00:00:28,933 --> 00:00:32,366
"DOUBLE DARE 2000"!

10
00:00:32,466 --> 00:00:33,800
Jason: HEAVING THEM.

11
00:00:33,900 --> 00:00:35,200
HE FINALLY GOT ONE.

12
00:00:35,300 --> 00:00:37,000
HEAVING THEM! OH, SO CLOSE!

13
00:00:37,100 --> 00:00:39,500
HE MIGHT KNOCK OVER
THE WHOLE THING
*/