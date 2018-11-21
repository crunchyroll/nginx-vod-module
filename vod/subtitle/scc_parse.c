/*
 * Copyright (C) 2018 Rafik Mikhael <rmikhael@ellation.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include "scc_format.h"

#define SCC_STYLES_ALLOC 20

static const unsigned char lowertab[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
    0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
    0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20,
    0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b,
    0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
    0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x61,
    0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c,
    0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
    0x78, 0x79, 0x7a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62,
    0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d,
    0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
    0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81, 0x82, 0x83,
    0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e,
    0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99,
    0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4,
    0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
    0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba,
    0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5,
    0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0,
    0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb,
    0xdc, 0xdd, 0xde, 0xdf, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6,
    0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 0xf0, 0xf1,
    0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc,
    0xfd, 0xfe, 0xff
};

static const int rowdata[] = {11,-1,1,2,3,4,12,13,14,15,5,6,7,8,9,10};
// Relationship between the first PAC byte and the row number

static const unsigned char pac2_attribs[][3] = // Color, font, indent
{
	{ COL_WHITE,   FONT_REGULAR,           0 }, // 0x40 || 0x60
	{ COL_WHITE,   FONT_UNDERLINED,        0 }, // 0x41 || 0x61
	{ COL_GREEN,   FONT_REGULAR,           0 }, // 0x42 || 0x62
	{ COL_GREEN,   FONT_UNDERLINED,        0 }, // 0x43 || 0x63
	{ COL_BLUE,    FONT_REGULAR,           0 }, // 0x44 || 0x64
	{ COL_BLUE,    FONT_UNDERLINED,        0 }, // 0x45 || 0x65
	{ COL_CYAN,    FONT_REGULAR,           0 }, // 0x46 || 0x66
	{ COL_CYAN,    FONT_UNDERLINED,        0 }, // 0x47 || 0x67
	{ COL_RED,     FONT_REGULAR,           0 }, // 0x48 || 0x68
	{ COL_RED,     FONT_UNDERLINED,        0 }, // 0x49 || 0x69
	{ COL_YELLOW,  FONT_REGULAR,           0 }, // 0x4a || 0x6a
	{ COL_YELLOW,  FONT_UNDERLINED,        0 }, // 0x4b || 0x6b
	{ COL_MAGENTA, FONT_REGULAR,           0 }, // 0x4c || 0x6c
	{ COL_MAGENTA, FONT_UNDERLINED,        0 }, // 0x4d || 0x6d
	{ COL_WHITE,   FONT_ITALIC,            0 }, // 0x4e || 0x6e
	{ COL_WHITE,   FONT_UNDERLINED_ITALIC, 0 }, // 0x4f || 0x6f
	{ COL_WHITE,   FONT_REGULAR,           0 }, // 0x50 || 0x70
	{ COL_WHITE,   FONT_UNDERLINED,        0 }, // 0x51 || 0x71
	{ COL_WHITE,   FONT_REGULAR,           4 }, // 0x52 || 0x72
	{ COL_WHITE,   FONT_UNDERLINED,        4 }, // 0x53 || 0x73
	{ COL_WHITE,   FONT_REGULAR,           8 }, // 0x54 || 0x74
	{ COL_WHITE,   FONT_UNDERLINED,        8 }, // 0x55 || 0x75
	{ COL_WHITE,   FONT_REGULAR,          12 }, // 0x56 || 0x76
	{ COL_WHITE,   FONT_UNDERLINED,       12 }, // 0x57 || 0x77
	{ COL_WHITE,   FONT_REGULAR,          16 }, // 0x58 || 0x78
	{ COL_WHITE,   FONT_UNDERLINED,       16 }, // 0x59 || 0x79
	{ COL_WHITE,   FONT_REGULAR,          20 }, // 0x5a || 0x7a
	{ COL_WHITE,   FONT_UNDERLINED,       20 }, // 0x5b || 0x7b
	{ COL_WHITE,   FONT_REGULAR,          24 }, // 0x5c || 0x7c
	{ COL_WHITE,   FONT_UNDERLINED,       24 }, // 0x5d || 0x7d
	{ COL_WHITE,   FONT_REGULAR,          28 }, // 0x5e || 0x7e
	{ COL_WHITE,   FONT_UNDERLINED,       28 }  // 0x5f || 0x7f
};

#ifdef SCC_TEMP_VERBOSITY
static const char *command_type[] =
{
	"Unknown",
	"EDM - EraseDisplayedMemory",
	"RCL - ResumeCaptionLoading",
	"EOC - End Of Caption",
	"TO1 - Tab Offset, 1 column",
	"TO2 - Tab Offset, 2 column",
	"TO3 - Tab Offset, 3 column",
	"RU2 - Roll up 2 rows",
	"RU3 - Roll up 3 rows",
	"RU4 - Roll up 4 rows",
	"CR  - Carriage Return",
	"ENM - Erase non-displayed memory",
	"BS  - Backspace",
	"RTD - Resume Text Display",
	"AOF - Not Used (Alarm Off)",
	"AON - Not Used (Alarm On)",
	"DER - Delete to End of Row",
	"RDC - Resume Direct Captioning",
	"FON - Flash turened ON"
};

static const char *font_text[]=
{
	"regular",
	"italic",
	"underlined",
	"underlined_italic",
	"bold",
	"bold_italic",
	"bold_underlined"
	"bold_underlined_italic"
};

#define MAX_COLOR 10
static const char *color_text[MAX_COLOR][2]=
{
	{"white",""},
	{"green","<font color=\"#00ff00\">"},
	{"blue","<font color=\"#0000ff\">"},
	{"cyan","<font color=\"#00ffff\">"},
	{"red","<font color=\"#ff0000\">"},
	{"yellow","<font color=\"#ffff00\">"},
	{"magenta","<font color=\"#ff00ff\">"},
	{"userdefined","<font color=\""},
	{"black",""},
	{"transparent",""}
};
#endif
//====================================================================================

scc_event_t *get_writing_buffer(scc_track_t *track, request_context_t* request_context)
{
    if (track->n_events > 0)
        return (track->events + (track->n_events - 1)); // currently open event
    else
	    return NULL;
}


scc_event_t *scc_alloc_event(scc_track_t *track)
{
    if (track->n_events == track->max_events) {
        track->max_events = track->max_events * 2 + 1; // keep the +1 for the case max_events is init to 0
        track->events =
            (scc_event_t *) realloc(track->events,
                                    sizeof(scc_event_t) *
                                    track->max_events);
    }

    int eid = track->n_events++;
    return (track->events + eid);
}
scc_event_t *new_event(scc_track_t *track, request_context_t* request_context)
{
    scc_event_t *event = scc_alloc_event(track);

    if (event == NULL)
    {
        vod_log_error(VOD_LOG_ERR, request_context->log, 0,
            "FAILED TO ALLOC EVENT. nevents=%d", track->n_events);
        return NULL;
    }

    int i;
	for (i=0;i<15;i++)
	{
		memset(event->characters[i], 0, SCC_608_SCREEN_WIDTH+1);
		memset(event->iub[i],        0, SCC_608_SCREEN_WIDTH+1);
		event->row_used[i] = 0;
	}
	event->color           = COL_WHITE;
	event->bk_color        = COL_CYAN;
	event->len_text        = 0;
	event->event_text_done = EVENT_TEXT_OPEN;
	event->start_time      = event->end_time
	                       = track->cue_time;
	return event;
}

/* Process PREAMBLE ACCESS CODES (PAC) */
void handle_pac(unsigned char c1, unsigned char c2, scc_track_t *track, request_context_t* request_context)
{
    int row=rowdata[((c1<<1)&14)|((c2>>5)&1)];

    if (c2>=0x40 && c2<=0x5f)
    {
        c2=c2-0x40;
    }
    else
    {
        if (c2>=0x60 && c2<=0x7f)
        {
            c2=c2-0x60;
        }
        else
        {
            vod_log_error(VOD_LOG_ERR, request_context->log, 0, "THIS IS NOT A PAC");
            return;
        }
    }

    int           new_cursor_row = row - 1; // Since the array is 0 based
    int           new_cursor_col = pac2_attribs[c2][2];
    unsigned char new_color      = pac2_attribs[c2][0];
    unsigned char new_bk_color   = COL_CYAN;    // TODO: read from PACs
    scc_event_t * event = get_writing_buffer(track, request_context);

#ifdef SCC_TEMP_VERBOSITY
    vod_log_error(VOD_LOG_ERR, request_context->log, 0,
        "handle_pac() c1=%d, c2=%d, new_cursor_row=%d, track_cursor_row=%d, new_col=%d, track_cursor_col=%d, new_color=%d, track_color=%d",
        c1, c2, new_cursor_row, track->cursor_row, new_cursor_col, track->cursor_column, new_color, track->current_color);
#endif

    if (((new_cursor_row != track->cursor_row) && (new_cursor_row  != (track->cursor_row+1)))  ||
        new_color        != track->current_color    ||
        new_bk_color     != track->current_bk_color ||
        (track->n_events == 0)                      ||
        (event != NULL	&&  event->event_text_done == EVENT_TEXT_DONE))
    {
        // close previous event, open new one so we store them as 2 separate text cues with different positions
        if (event != NULL && event->event_text_done == EVENT_TEXT_OPEN)
        {
            // close last event text against further writing
            event->event_text_done = EVENT_TEXT_DONE;
        }

        track->cursor_row           = new_cursor_row;
        track->cursor_column        = new_cursor_col;
        track->current_color        = new_color;
        track->current_bk_color     = new_bk_color;
        track->current_iub          = 0;
        event = new_event(track, request_context);
    }

    if ((new_cursor_col !=  track->cursor_column   )     &&
        (new_cursor_col != (track->cursor_column+1))     &&
        (new_cursor_row == (track->cursor_row+1   )))
    {
        // new line in the same event, just insert new line in index 32, and update the track values
        event->characters[track->cursor_row][SCC_608_SCREEN_WIDTH] = '\n';
        event->iub       [track->cursor_row][SCC_608_SCREEN_WIDTH] = track->current_iub;
        track->cursor_row           = new_cursor_row;
        track->cursor_column        = new_cursor_col;
    }

    // now that newline was added, update the track's italic/underline/bold
    track->current_iub =  ((pac2_attribs[c2][1] == FONT_ITALIC)     || (pac2_attribs[c2][1] == FONT_UNDERLINED_ITALIC))       |
                         (((pac2_attribs[c2][1] == FONT_UNDERLINED) || (pac2_attribs[c2][1] == FONT_UNDERLINED_ITALIC)) << 1) |
                         (track->current_iub & 4);

#ifdef SCC_TEMP_VERBOSITY
    vod_log_error(VOD_LOG_ERR, request_context->log, 0,
        "handle_pac: %d %d  nevents=%d, trackrow=%d, col=%d, color=%d, iub=%d",
        c1, c2, track->n_events, track->cursor_row, track->cursor_column, track->current_color, track->current_iub);
#endif
}

/*void delete_to_end_of_row(scc_track_t *track, request_context_t* request_context)
{
    scc_event_t *event = get_writing_buffer(track, request_context);
    if (event == NULL || event->event_text_done == EVENT_TEXT_DONE)
        return; // do nothing

    int i;
    for (i = track->cursor_column; i <= SCC_608_SCREEN_WIDTH-1; i++)
    {
        // TODO: This can change the 'used' situation of a column, so we'd
        // need to check and correct.
        event->characters[track->cursor_row][i] = 0;
    }
    event->iub[track->cursor_row][track->cursor_column] = 0;
}*/

void write_char(const unsigned char c, scc_track_t *track, request_context_t* request_context)
{
    scc_event_t *event = get_writing_buffer(track, request_context);
    if (event == NULL || event->event_text_done == EVENT_TEXT_DONE)
        return; // do nothing
    else
    {
        event->characters[track->cursor_row][track->cursor_column] = c; // TODO: handle extended charsets
        event->iub       [track->cursor_row][track->cursor_column] = track->current_iub;
        event->row_used[track->cursor_row] = 1;
        event->len_text++;
    }

    if (track->cursor_column < SCC_608_SCREEN_WIDTH - 1)
        track->cursor_column++;
}

/* Handle MID-ROW CODES. */
void handle_text_attr(const unsigned char c1, const unsigned char c2, scc_track_t *track, request_context_t* request_context)
{
	if ((c1!=0x11 && c1!=0x19) || (c2<0x20 || c2>0x2f))
	{
		vod_log_error(VOD_LOG_ERR, request_context->log, 0, "This is not a text attribute! c1=%d, c2=%d", c1, c2);
	}
	else
	{
		int i = c2-0x20;
		track->current_color     = pac2_attribs[i][0];
		track->current_bk_color  = COL_CYAN;
		// Mid-row codes turn flashing off, so we reset bit 2 in the track's iub bit field
		track->current_iub       =  ((pac2_attribs[i][1] == FONT_ITALIC)     || (pac2_attribs[i][1] == FONT_UNDERLINED_ITALIC))      |
		                           (((pac2_attribs[i][1] == FONT_UNDERLINED) || (pac2_attribs[i][1] == FONT_UNDERLINED_ITALIC)) << 1);
#ifdef SCC_TEMP_VERBOSITY
		vod_log_error(VOD_LOG_ERR, request_context->log, 0,
			"handle_text_attr() --  Color: %s,  iub: %s",
			color_text[track->current_color][0],
			font_text[track->current_iub]);
#endif
		// Mid-row codes should put a non-transparent space at the current position
		// and advance the cursor so use write_char
		//write_char(SCC_UNUSED_CHAR, track, request_context);
	}
}

void handle_single(const unsigned char c1, scc_track_t *track, request_context_t* request_context)
{
	if (c1<0x20)
		return; // We don't allow teleprinting stuff here
	write_char (c1,track, request_context);
}

// CEA-608, Anex F 1.1.1. - Character Set Table / Special Characters
void handle_special_doublebytes(const unsigned char c1, const unsigned char c2, scc_track_t *track, request_context_t* request_context)
{
	unsigned char c;
	if (c2>=0x30 && c2<=0x3f)
	{
		c=c2 + 0x50; // So if c>=0x80 && c<=0x8f, it comes from here
		write_char(c, track, request_context);
	}
}

/* Process EXTENDED CHARACTERS */
unsigned char handle_extended(unsigned char hi, unsigned char lo, scc_track_t *track, request_context_t* request_context)
{
	unsigned char c=0;
	// For lo values between 0x20-0x3f
	if (lo>=0x20 && lo<=0x3f && (hi==0x12 || hi==0x13))
	{
		switch (hi)
		{
			case 0x12:
				c=lo+0x70; // So if c>=0x90 && c<=0xaf it comes from here
				break;
			case 0x13:
				c=lo+0x90; // So if c>=0xb0 && c<=0xcf it comes from here
				break;
		}
		// This column change is because extended characters replace
		// the previous character (which is sent for basic decoders
		// to show something similar to the real char)
		//if (track->cursor_column>0)
        //    track->cursor_column--;

		write_char(c, track, request_context);
	}
	return 1;
}

/* Process GLOBAL CODES */
void handle_command(unsigned char c1, const unsigned char c2, scc_track_t *track, request_context_t* request_context)
{
	enum command_code command = COM_UNKNOWN;

	if (c1==0x15)
		c1=0x14;
	if ((c1==0x14 || c1==0x1C) && c2==0x2C)
		command = COM_ERASEDISPLAYEDMEMORY;
	if ((c1==0x14 || c1==0x1C) && c2==0x20)
		command = COM_RESUMECAPTIONLOADING;
	if ((c1==0x14 || c1==0x1C) && c2==0x2F)
		command = COM_ENDOFCAPTION;
	if ((c1==0x14 || c1==0x1C) && c2==0x22)
		command = COM_ALARMOFF;
	if ((c1==0x14 || c1==0x1C) && c2==0x23)
		command = COM_ALARMON;
	if ((c1==0x14 || c1==0x1C) && c2==0x24)
		command = COM_DELETETOENDOFROW;
	if ((c1==0x17 || c1==0x1F) && c2==0x21)
		command = COM_TABOFFSET1;
	if ((c1==0x17 || c1==0x1F) && c2==0x22)
		command = COM_TABOFFSET2;
	if ((c1==0x17 || c1==0x1F) && c2==0x23)
		command = COM_TABOFFSET3;
	if ((c1==0x14 || c1==0x1C) && c2==0x25)
		command = COM_ROLLUP2;
	if ((c1==0x14 || c1==0x1C) && c2==0x26)
		command = COM_ROLLUP3;
	if ((c1==0x14 || c1==0x1C) && c2==0x27)
		command = COM_ROLLUP4;
	if ((c1==0x14 || c1==0x1C) && c2==0x28)
		command = COM_FLASHON;
	if ((c1==0x14 || c1==0x1C) && c2==0x29)
		command = COM_RESUMEDIRECTCAPTIONING;
	if ((c1==0x14 || c1==0x1C) && c2==0x2D)
		command = COM_CARRIAGERETURN;
	if ((c1==0x14 || c1==0x1C) && c2==0x2E)
		command = COM_ERASENONDISPLAYEDMEMORY;
	if ((c1==0x14 || c1==0x1C) && c2==0x21)
		command = COM_BACKSPACE;
	if ((c1==0x14 || c1==0x1C) && c2==0x2b)
		command = COM_RESUMETEXTDISPLAY;

#ifdef SCC_TEMP_VERBOSITY
	vod_log_error(VOD_LOG_ERR, request_context->log, 0,
	    "BEGIN{ handle_command(): %0d %0d (%s), Position=%d,%d  n_events=%d",
	    c1, c2, command_type[command], track->cursor_row, track->cursor_column, track->n_events);
#endif

    scc_event_t * event = get_writing_buffer(track, request_context);
	switch (command)
	{
		case COM_BACKSPACE:
			if (event != NULL && track->cursor_column>0)
			{
				track->cursor_column--;
				event->characters[track->cursor_row][track->cursor_column] = 0;
			}
			break;
		case COM_TABOFFSET1:
			if (track->cursor_column < SCC_608_SCREEN_WIDTH - 1)
				track->cursor_column++;
			break;
		case COM_TABOFFSET2:
			track->cursor_column += 2;
			if (track->cursor_column > SCC_608_SCREEN_WIDTH - 1)
				track->cursor_column = SCC_608_SCREEN_WIDTH - 1;
			break;
		case COM_TABOFFSET3:
			track->cursor_column += 3;
			if (track->cursor_column > SCC_608_SCREEN_WIDTH - 1)
				track->cursor_column = SCC_608_SCREEN_WIDTH - 1;
			break;
		case COM_RESUMECAPTIONLOADING:
		case COM_RESUMEDIRECTCAPTIONING:
		    if (event != NULL && event->event_text_done == EVENT_TEXT_OPEN)
		    {
		        // close last event if not done
		        event->event_text_done = EVENT_TEXT_DONE;
            }
			break;
		case COM_CARRIAGERETURN:
            track->cursor_column = 0;
            if (track->cursor_row < 15)
                track->cursor_row++;
            break;
		case COM_ERASEDISPLAYEDMEMORY:
		    if (event != NULL && event->event_text_done == EVENT_TEXT_OPEN)
		    {
		        // close last event if not done
		        event->event_text_done = EVENT_TEXT_DONE;
            }
            track->cursor_row        = track->cursor_column = 0;
            track->current_color     = COL_WHITE;
            track->current_bk_color  = COL_CYAN;
            track->current_iub       = 0;
			break;
		case COM_ENDOFCAPTION:
		    if (event != NULL && event->event_text_done == EVENT_TEXT_OPEN)
		    {
		        // close last event if not done
		        event->event_text_done = EVENT_TEXT_DONE;
            }
		    // TODO: make sure you check this flag when writing char
			break;
		case COM_FLASHON:
		    track->current_iub |= 4;
		    break;
		case COM_DELETETOENDOFROW:
		case COM_ROLLUP2:
		case COM_ROLLUP3:
		case COM_ROLLUP4:
		case COM_ALARMOFF:
		case COM_ALARMON:
		case COM_RESUMETEXTDISPLAY:
		case COM_ERASENONDISPLAYEDMEMORY:
			break;
		default:
			vod_log_error(VOD_LOG_ERR, request_context->log, 0, "Command not yet implemented.");
			break;
	}
#ifdef SCC_TEMP_VERBOSITY
	vod_log_error(VOD_LOG_ERR, request_context->log, 0, "}END handle_command(): Position=%d,%d n_events=%d, color=%d iub=%d",
		track->cursor_row, track->cursor_column, track->n_events, track->current_color, track->current_iub);
#endif
}



/* Handle Command, special char or attribute
 * Returns 1 if something was written to screen, 0 otherwise */
void disCommand(unsigned char hi, unsigned char lo, scc_track_t *track, request_context_t* request_context)
{
#ifdef SCC_TEMP_VERBOSITY
    vod_log_error(VOD_LOG_ERR, request_context->log, 0,
        "disCommand(): hi = %d, lo = %d", hi, lo);
#endif

        // Treat Ch 2,4 as Ch 1,3 respectively
	if (hi>=0x18 && hi<=0x1f)
            hi=hi-8;
	// Treat Ch 3 as Ch 1 in Global codes and {BS, DER, CR}.
        if (hi==0x15 && lo>=0x20 && lo<=0x2f)
            hi--;

	switch (hi)
	{
		case 0x10:
			if (lo>=0x40 && lo<=0x5f)                                // row 11 PAC (ch 1&3)
				handle_pac(hi, lo, track, request_context);
			break;
		case 0x11:
			if (lo>=0x20 && lo<=0x2f)                                // mid-row color/underline PAC (ch 1&3)
				handle_text_attr(hi, lo, track, request_context);
			if (lo>=0x30 && lo<=0x3f)                                // Special Characters
			{
				handle_special_doublebytes(hi, lo, track, request_context);
			}
			if (lo>=0x40 && lo<=0x7f)                                // rows 01/02 PAC (ch 1&3)
				handle_pac(hi, lo, track, request_context);
			break;
		case 0x12:
		case 0x13:
			if (lo>=0x20 && lo<=0x3f)                                // Extended Characters
			{
				handle_extended(hi, lo, track, request_context);
			}
			if (lo>=0x40 && lo<=0x7f)                                // rows 03/04 PAC (ch 1&3)
				handle_pac(hi, lo, track, request_context);
			break;
		case 0x14:
		case 0x15:
			if (lo>=0x20 && lo<=0x2f)                                // Global Codes PAC (ch 1&3)
				handle_command(hi, lo, track, request_context);
			if (lo>=0x40 && lo<=0x7f)                                // rows 05/06 and 14/15 PAC (ch 1&3)
				handle_pac(hi, lo, track, request_context);
			break;
		case 0x16:
			if (lo>=0x40 && lo<=0x7f)                                // rows 07/08 PAC (ch 1&3)
				handle_pac(hi, lo, track, request_context);
			break;
		case 0x17:
			if (lo>=0x21 && lo<=0x23)                                // Tab Offsets 1,2,3 (ch 1&3)
				handle_command(hi, lo, track, request_context);
			if (lo>=0x2e && lo<=0x2f)                                // mid-row Black and BlackUnderline PAC (ch 1&3)
				handle_text_attr(hi, lo, track, request_context);
			if (lo>=0x40 && lo<=0x7f)                                // rows 09/10 PAC (ch 1&3)
				handle_pac(hi, lo, track, request_context);
			break;
	}
	return;
}



/**
 * \brief Parse a header line
 * \param track scc_track_t that holds all running values for the SCC file
 * \param str string to parse, zero-terminated
*/
static int scc_process_line(scc_track_t *track, const char *str, request_context_t* request_context)
{
    int hr, mn, sc, fr, i, length;
    //bool_t ndp = FALSE;

    int res1 = sscanf(str, "%d:%d:%d:%d\t", &hr, &mn, &sc, &fr);
    if (res1 != 4)
    {
        int res2 = sscanf(str, "%d:%d:%d;%d\t", &hr, &mn, &sc, &fr);
        if (res2 != 4)
            return 0;  // no timing at the start of the line, ignore this line altogether
        //else
            //ndp = TRUE;
    }
    str+=12;
    // TODO: use ndp to scale the fr field?
    track->cue_time = (fr * 1000 / 30) + (1000 * (sc + 60 * (mn + 60LL * hr)));

    if (track->maxDuration < track->cue_time)
        track->maxDuration = track->cue_time;

	length = vod_strlen(str);

#ifdef SCC_TEMP_VERBOSITY
    vod_log_error(VOD_LOG_ERR, request_context->log, 0,
        "line_time= %D, lengthstr=%d, str = %s", track->cue_time, length, str);
#endif


    for (i=0; i < length; i+=4)
    {
        // code is still in ASCII text, need to convert every 2 consecutive chars to their equivalent hexadecimal digit
        unsigned char hi, lo;
        unsigned int fullword;

        // skip tabs and spaces
        while ((*(str+i) == ' ') || (*(str+i) == '\t'))
             i++;

        sscanf(str+i, "%x ", &fullword);
        hi = (fullword >> 8) & 0x7F; // Get rid of parity bit
        lo = fullword        & 0x7F; // Get rid of parity bit
            //vod_log_error(VOD_LOG_ERR, request_context->log, 0,
            //" FULLWORD:%d, hi=%d lo=%d", fullword, hi, lo);

        if (hi==0 && lo==0) // Just padding
            continue;

        if (hi>=0x10 && hi<=0x1F) // Non-character code or special/extended char
        // http://www.theneitherworld.com/mcpoodle/SCC_TOOLS/DOCS/CC_CODES.HTML
        // http://www.theneitherworld.com/mcpoodle/SCC_TOOLS/DOCS/CC_CHARS.HTML
        {
            if (track->last_c1 == hi && track->last_c2 == lo)
            {
                // Duplicate dual code, discard. Correct to do it only in
                // non-XDS, XDS codes shall not be repeated.
                // Ignore only the first repetition
                track->last_c1 = -1;
                track->last_c2 = -1;
                continue;
            }
            track->last_c1 = hi;
            track->last_c2 = lo;

            disCommand(hi, lo, track, request_context);
        }
        else
        {
            track->last_c1 = -1;
            track->last_c2 = -1;

            //vod_log_error(VOD_LOG_ERR, request_context->log, 0,
            //" GOING to handle 2 characters: %d %d, i=%d", hi, lo, i);
            if (hi>=0x20) // Standard characters (always in pairs)
            {
                handle_single(hi, track, request_context);
                handle_single(lo, track, request_context);

                track->last_c1 = 0;
                track->last_c2 = 0;

            }
        }
    } // for

	return 0; // Success
}

/**
 * \brief Process all text in an ASS/SSA file
 * \param track output ass_track_t pointer
 * \param str utf-8 string to parse, zero-terminated
*/
static int scc_process_text(scc_track_t *track, char *str, request_context_t* request_context)
{
    int retval = 0;
    char *p = str;

    while (1) {
        char *q;
        while (1) {
            if ((*p == '\r') || (*p == '\n'))
                ++p;
            else if (p[0] == '\xef' && p[1] == '\xbb' && p[2] == '\xbf')
                p += 3;         // U+FFFE (BOM)
            else
                break;
        }
        for (q = p; ((*q != '\0') && (*q != '\r') && (*q != '\n')); ++q) {
        };
        if (q == p)
            break;
        if (*q != '\0')
            *(q++) = '\0';
        retval |= scc_process_line(track, p, request_context);
        if (*q == '\0')
            break;
        p = q;
    }
    return retval;
}


/* Allocates scc_track_t, assumes caller will free the returned pointer. Cleans up everything else allocated internally. */
scc_track_t *scc_parse_memory(char *data, int length, request_context_t* request_context)
{
    int bfailed;
	if (!data)
	{
		return NULL;
	}

    // copy the input buffer, as the parsing is destructive.
    char *pcopy = vod_alloc(request_context->pool, length+1);
    if (pcopy == NULL)
    {
        vod_log_error(VOD_LOG_ERR, request_context->log, 0,
            "scc_parse_memory(): vod_alloc of pcopy failed");
        return NULL;
    }
    vod_memcpy(pcopy, data, length+1);

    // initializes all fields to zero. If that doesn't suit your need, use another track_init function.
    scc_track_t *track = vod_calloc(request_context->pool, sizeof(scc_track_t));
    if (track == NULL)
    {
        vod_log_error(VOD_LOG_ERR, request_context->log, 0,
            "scc_parse_memory(): vod_callocof 608_context failed");
        vod_free(request_context->pool, pcopy);
        return NULL;
    }
    //TODO: initialize non-zero variables

    // destructive parsing of pcopy
    bfailed = scc_process_text(track, pcopy, request_context);
    vod_free(request_context->pool, pcopy);   // not needed anymore whether parsing succeeded or failed
    if (bfailed == -1)
    {
        vod_log_error(VOD_LOG_ERR, request_context->log, 0,
            "scc_parse_memory(): process_text failed");
        return NULL;

    }

	return track;
}
