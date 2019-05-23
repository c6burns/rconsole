#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <uv.h>
#include <assert.h>

#include "replxx.h"
#include "util.h"


void tty_alloc_cb(uv_handle_t* handle, size_t size, uv_buf_t* buf)
{
	buf->base = malloc(size);
	buf->len = size;
}

void tty_read_cb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf)
{
	if (nread > 0) {
		if (buf->base[nread - 1] == '\n') {

		}
		buf->base[nread] = 0;
		//printf("Read %d bytes: %s\n\n", nread, buf->base);
		printf("%d -- %d -- %s\n", nread, (int)buf->base[0], buf->base);
	}
}

static void repeat_cb(uv_timer_t* handle)
{
	//printf("REPEAT_CB\n");

	assert(handle != NULL);
	assert(1 == uv_is_active((uv_handle_t*)handle));

	//repeat_cb_called++;

	//if (repeat_cb_called == 5) {
	//	uv_close((uv_handle_t*)handle, repeat_close_cb);
	//}
}


int uvmain()
{
	int ret;
	int width, height;
	uv_tty_t tty_in, tty_out;
	uv_timer_t tty_timer;
	uv_loop_t *loop = uv_default_loop();

	uv_timer_init(uv_default_loop(), &tty_timer);
	uv_timer_start(&tty_timer, repeat_cb, 100, 100);

	ret = uv_tty_init(loop, &tty_in, 0, 1);
	assert(ret == 0);
	assert(uv_is_readable((uv_stream_t*)&tty_in));
	assert(!uv_is_writable((uv_stream_t*)&tty_in));
	assert(0 == uv_tty_set_mode(&tty_in, UV_TTY_MODE_RAW));

	ret = uv_tty_init(loop, &tty_out, 1, 0);
	assert(ret == 0);
	assert(!uv_is_readable((uv_stream_t*)&tty_out));
	assert(uv_is_writable((uv_stream_t*)&tty_out));

	ret = uv_tty_get_winsize(&tty_out, &width, &height);
	assert(ret == 0);

	printf("width=%d height=%d\n", width, height);

	if (uv_read_start((uv_stream_t *)&tty_in, tty_alloc_cb, tty_read_cb)) {
		printf("Error starting read on tty!!!!\n");
	}

	int tty_w, tty_h;
	if (uv_tty_get_winsize(&tty_out, &tty_w, &tty_h)) {
		printf("Error getting winsize!!!!\n");
	} else {
		printf("winsize: %d x %d\n", tty_w, tty_h);
	}

	//if (uv_guess_handle(1) == UV_TTY) {
	//	uv_write_t req;
	//	uv_buf_t buf;
	//	buf.base = "\033[41;37m";
	//	buf.len = strlen(buf.base);
	//	uv_write(&req, (uv_stream_t*)&tty_out, &buf, 1, NULL);
	//}

	uv_write_t req;
	uv_buf_t buf;
	buf.base = "Hello TTY\n";
	buf.len = strlen(buf.base);
	uv_write(&req, (uv_stream_t*)&tty_out, &buf, 1, NULL);
	uv_tty_reset_mode();

	//printf("AWWWWWW YEAH!!!!");
	return uv_run(loop, UV_RUN_DEFAULT);
}

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

int main( int argc, char** argv ) {
	uvmain();
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

