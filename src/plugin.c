#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

#include "replxx.h"
#include "util.h"

#include "uv.h"

#include "tn/term.h"
#include "rc/buf.h"


void completionHook(char const* context, replxx_completions* lc, int* contextLen, void* ud) {
	char** examples = (char**)( ud );
	size_t i;

	int utf8ContextLen = context_len( context );
	int prefixLen = strlen( context ) - utf8ContextLen;
	*contextLen = utf8str_codepoint_len( context + prefixLen, utf8ContextLen );
	for (i = 0;	examples[i] != NULL; ++i) {
		if (strncmp(context + prefixLen, examples[i], utf8ContextLen) == 0) {
			replxx_add_completion(lc, examples[i]);
		}
	}
}

void hintHook(char const* context, replxx_hints* lc, int* contextLen, ReplxxColor* c, void* ud) {
	char** examples = (char**)( ud );
	int i;
	int utf8ContextLen = context_len( context );
	int prefixLen = strlen( context ) - utf8ContextLen;
	*contextLen = utf8str_codepoint_len( context + prefixLen, utf8ContextLen );
	if ( *contextLen > 0 ) {
		for (i = 0;	examples[i] != NULL; ++i) {
			if (strncmp(context + prefixLen, examples[i], utf8ContextLen) == 0) {
				replxx_add_hint(lc, examples[i]);
			}
		}
	}
}

void colorHook( char const* str_, ReplxxColor* colors_, int size_, void* ud ) {
	int i = 0;
	for ( ; i < size_; ++ i ) {
		if ( isdigit( str_[i] ) ) {
			colors_[i] = REPLXX_COLOR_BRIGHTMAGENTA;
		}
	}
	if ( ( size_ > 0 ) && ( str_[size_ - 1] == '(' ) ) {
		replxx_emulate_key_press( ud, ')' );
		replxx_emulate_key_press( ud, REPLXX_KEY_LEFT );
	}
}

char const* recode( char* s ) {
	char const* r = s;
	while ( *s ) {
		if ( *s == '~' ) {
			*s = '\n';
		}
		++ s;
	}
	return ( r );
}

void split( char* str_, char** data_, int size_ ) {
	int i = 0;
	char* p = str_, *o = p;
	while ( i < size_ ) {
		int last = *p == 0;
		if ( ( *p == ',' ) || last ) {
			*p = 0;
			data_[i ++] = o;
			o = p + 1;
			if ( last ) {
				break;
			}
		}
		++ p;
	}
	data_[i] = 0;
}


struct rc_term {
	uint16_t w, h;
	uint16_t prompt_len;
	rc_buf_t buf;
};

tn_term_t term = TN_TERM_INIT;
struct rc_term rc_term = { 0 };

uint32_t rc_prompt();

void on_term_char_cb(char c)
{
	rc_buf_insert(&rc_term.buf, c);
	tn_term_write(&term, "%c", c);
	tn_term_pos_store(&term);
	tn_term_write(&term, "%s", rc_buf_slice(&rc_term.buf, rc_buf_cursor_get(&rc_term.buf)));
	tn_term_pos_restore(&term);
}

void on_term_key_cb(enum tn_term_key key)
{
	uint16_t pos;

	switch (key) {
	case TN_TERM_KEY_F1:
		break;
	case TN_TERM_KEY_F2:
		break;
	case TN_TERM_KEY_F3:
		break;
	case TN_TERM_KEY_F4:
		break;
	case TN_TERM_KEY_F5:
		break;
	case TN_TERM_KEY_F6:
		break;
	case TN_TERM_KEY_F7:
		break;
	case TN_TERM_KEY_F8:
		break;
	case TN_TERM_KEY_F9:
		break;
	case TN_TERM_KEY_F10:
		break;
	case TN_TERM_KEY_F11:
		break;
	case TN_TERM_KEY_F12:
		break;
	case TN_TERM_KEY_HOME:
		break;
	case TN_TERM_KEY_END:
		break;
	case TN_TERM_KEY_INSERT:
		break;
	case TN_TERM_KEY_DELETE:
		rc_buf_delete(&rc_term.buf);
		tn_term_pos_store(&term);
		tn_term_write(&term, "%s ", rc_buf_slice(&rc_term.buf, rc_buf_cursor_get(&rc_term.buf)));
		tn_term_pos_restore(&term);
		break;
	case TN_TERM_KEY_BACKSPACE:
		pos = rc_buf_cursor_get(&rc_term.buf);
		if (pos) {
			rc_buf_cursor_set(&rc_term.buf, pos - 1);
			rc_buf_delete(&rc_term.buf);
			tn_term_pos_left(&term, 1);
			tn_term_pos_store(&term);
			tn_term_write(&term, "%s ", rc_buf_slice(&rc_term.buf, rc_buf_cursor_get(&rc_term.buf)));
			tn_term_pos_restore(&term);
		}
		break;
	case TN_TERM_KEY_UP:
		break;
	case TN_TERM_KEY_DOWN:
		break;
	case TN_TERM_KEY_RIGHT:
		pos = rc_buf_cursor_get(&rc_term.buf);
		if (rc_buf_cursor_set(&rc_term.buf, pos + 1) == TN_SUCCESS) {
			tn_term_pos_right(&term, 1);
		}
		break;
	case TN_TERM_KEY_LEFT:
		pos = rc_buf_cursor_get(&rc_term.buf);
		if (pos) {
			rc_buf_cursor_set(&rc_term.buf, pos - 1);
			tn_term_pos_left(&term, 1);
		}
		break;
	case TN_TERM_KEY_TAB:
		break;
	case TN_TERM_KEY_ESC:
		break;
	case TN_TERM_KEY_BREAK:
		break;
	case TN_TERM_KEY_ENTER:
		break;
	case TN_TERM_KEY_NONE:
	case TN_TERM_KEY_INVALID:
	default:
		break;
	}

	tn_term_flush(&term);
}

uint32_t rc_prompt()
{
	uint32_t len = 0;

	tn_term_pos_set(&term, 1, rc_term.h);

	//tn_term_clear_line(&term);
	
	tn_term_color_set(&term, tn_term_color16(&term, TN_TERM_COLOR_GREEN_BRIGHT));
	tn_term_write(&term, "rc");
	len += 2;
	
	tn_term_color_set(&term, tn_term_color16(&term, TN_TERM_COLOR_GREY_BRIGHT));
	tn_term_write(&term, "> ");
	len += 2;
	
	tn_term_pos_get(&term);
	tn_term_write(&term, "%s", rc_term.buf.bb.buffer);
	
	tn_term_flush(&term);

	return len;
}

void on_term_resize_cb(uint16_t x, uint16_t y)
{
	rc_term.w = x;
	rc_term.h = y;
	rc_term.prompt_len = rc_prompt();
	//tn_term_write(&term, "\nCommand: %s\n\n", cmd);
	//rc_prompt();
}

int main(int argc, char **argv)
{
	tn_term_state_t term_state;

	rc_buf_setup(&rc_term.buf, TN_TERM_MAX_LINE);

	tn_term_setup(&term);
	tn_term_callback_char(&term, on_term_char_cb);
	tn_term_callback_key(&term, on_term_key_cb);
	tn_term_callback_resize(&term, on_term_resize_cb);
	//tn_term_debug_print(&term, true);
	TN_GUARD(tn_term_start(&term));
	//tn_term_write(&term, "I love potatoes :D :D :D\n\n");
	//rc_prompt();
	//tn_term_flush(&term);
	while (1) {
		term_state = tn_term_state(&term);
		if (term_state == TN_TERM_STATE_STOPPING || term_state == TN_TERM_STATE_STOPPED || term_state == TN_TERM_STATE_ERROR) break;

		tn_thread_sleep_ms(50);
	}

	tn_term_cleanup(&term);

	return 0;

#define MAX_EXAMPLE_COUNT 128
	char* examples[MAX_EXAMPLE_COUNT + 1] = {
		"db", "hello", "hallo", "hans", "hansekogge", "seamann", "quetzalcoatl", "quit", "power", NULL
	};
	Replxx* replxx = replxx_init();
	replxx_install_window_change_handler( replxx );

	int quiet = 0;
	char const* prompt = "\x1b[1;32mreplxx\x1b[0m> ";
	while ( argc > 1 ) {
		-- argc;
		++ argv;
#ifdef __REPLXX_DEBUG__
		if ( !strcmp( *argv, "--keycodes" ) ) {
			replxx_debug_dump_print_codes();
			exit(0);
		}
#endif
		switch ( (*argv)[0] ) {
			case 'b': replxx_set_beep_on_ambiguous_completion( replxx, (*argv)[1] - '0' ); break;
			case 'c': replxx_set_completion_count_cutoff( replxx, atoi( (*argv) + 1 ) );   break;
			case 'e': replxx_set_complete_on_empty( replxx, (*argv)[1] - '0' );            break;
			case 'd': replxx_set_double_tab_completion( replxx, (*argv)[1] - '0' );        break;
			case 'h': replxx_set_max_hint_rows( replxx, atoi( (*argv) + 1 ) );             break;
			case 's': replxx_set_max_history_size( replxx, atoi( (*argv) + 1 ) );          break;
			case 'i': replxx_set_preload_buffer( replxx, recode( (*argv) + 1 ) );          break;
			case 'w': replxx_set_word_break_characters( replxx, (*argv) + 1 );             break;
			case 'm': replxx_set_no_color( replxx, (*argv)[1] - '0' );                     break;
			case 'p': prompt = recode( (*argv) + 1 );                                      break;
			case 'q': quiet = atoi( (*argv) + 1 );                                         break;
			case 'x': split( (*argv) + 1, examples, MAX_EXAMPLE_COUNT );                   break;
		}

	}

	const char* file = "./replxx_history.txt";

	replxx_history_load( replxx, file );
	replxx_set_completion_callback( replxx, completionHook, examples );
	replxx_set_highlighter_callback( replxx, colorHook, replxx );
	replxx_set_hint_callback( replxx, hintHook, examples );

	printf("starting...\n");

	while (1) {
		char const* result = NULL;
		do {
			result = replxx_input( replxx, prompt );
		} while ( ( result == NULL ) && ( errno == EAGAIN ) );

		if (result == NULL) {
			printf("\n");
			break;
		} else if (!strncmp(result, "/history", 8)) {
			/* Display the current history. */
			int index = 0;
			int size = replxx_history_size( replxx );
			for ( ; index < size; ++index) {
				char const* hist = replxx_history_line( replxx, index );
				if (hist == NULL) {
					break;
				}
				replxx_print( replxx, "%4d: %s\n", index, hist );
			}
		}
		if (*result != '\0') {
			replxx_print( replxx, quiet ? "%s\n" : "thanks for the input: %s\n", result );
			replxx_history_add( replxx, result );
		}
	}
	replxx_history_save( replxx, file );
	printf( "Exiting Replxx\n" );
	replxx_end( replxx );
}

