#include "tn/term.h"
#include "tn/buffer.h"
#include "uv.h"


typedef struct tn_term_testpriv_s {
	uv_loop_t uv_loop;
	uv_signal_t uv_signal;
	int ttyin_fd, ttyout_fd;
	uv_tty_t uv_tty_in, uv_tty_out;
	uv_timer_t uv_timer_tty;
	struct aws_byte_buf bbuf;
	struct aws_byte_cursor bpos;
} tn_term_testpriv_t;


tn_term_t g_term;
tn_term_testpriv_t g_term_priv;

void test_io()
{
	tn_term_t *term = &g_term;
    tn_term_testpriv_t *priv = &g_term_priv;
    term->priv = priv;

	tn_mutex_setup(&term->mtx);

	TN_GUARD_CLEANUP(aws_byte_buf_init(&priv->bbuf, aws_default_allocator(), TN_TERM_MAX_LINE));
	aws_byte_buf_reset(&priv->bbuf, true);
	priv->bpos = aws_byte_cursor_from_buf(&priv->bbuf);

	TN_GUARD_CLEANUP(uv_loop_init(&priv->uv_loop));

	priv->uv_timer_tty.data = term;

#ifdef _WIN32
	HANDLE handle;
	handle = CreateFileA("conin$",
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	TN_ASSERT(handle != INVALID_HANDLE_VALUE);
	priv->ttyin_fd = _open_osfhandle((intptr_t)handle, 0);

	handle = CreateFileA("conout$",
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	TN_ASSERT(handle != INVALID_HANDLE_VALUE);
	priv->ttyout_fd = _open_osfhandle((intptr_t)handle, 0);
#else /* unix */
	priv->ttyin_fd = open("/dev/tty", O_RDONLY, 0);
	if (priv->ttyin_fd < 0) {
		fprintf(stderr, "Cannot open /dev/tty as read-only: %s\n", strerror(errno));
		fflush(stderr);
		goto cleanup;
	}

	priv->ttyout_fd = open("/dev/tty", O_WRONLY, 0);
	if (priv->ttyout_fd < 0) {
		fprintf(stderr, "Cannot open /dev/tty as write-only: %s\n", strerror(errno));
		fflush(stderr);
		goto cleanup;
	}
#endif

	TN_ASSERT(priv->ttyin_fd >= 0);
	TN_ASSERT(priv->ttyout_fd >= 0);

	TN_ASSERT(UV_UNKNOWN_HANDLE == uv_guess_handle(-1));

	TN_ASSERT(UV_TTY == uv_guess_handle(priv->ttyin_fd));
	TN_ASSERT(UV_TTY == uv_guess_handle(priv->ttyout_fd));

	TN_GUARD_CLEANUP(uv_tty_init(&priv->uv_loop, &priv->uv_tty_in, priv->ttyin_fd, 1));
	TN_ASSERT(uv_is_readable((uv_stream_t*)&priv->uv_tty_in));
	TN_ASSERT(!uv_is_writable((uv_stream_t*)&priv->uv_tty_in));
	priv->uv_tty_in.data = term;

	TN_GUARD_CLEANUP(uv_tty_init(&priv->uv_loop, &priv->uv_tty_out, priv->ttyout_fd, 0));
	TN_ASSERT(!uv_is_readable((uv_stream_t*)&priv->uv_tty_out));
	TN_ASSERT(uv_is_writable((uv_stream_t*)&priv->uv_tty_out));
	priv->uv_tty_out.data = term;

	TN_GUARD_CLEANUP(uv_tty_get_winsize(&priv->uv_tty_out, &term->size.x, &term->size.y));

	TN_GUARD_CLEANUP(uv_tty_set_mode(&priv->uv_tty_in, UV_TTY_MODE_RAW));

    uv_buf_t buf;
    buf.base = "Party people in the house :D\n\n";
    buf.len = strlen(buf.base);
    uv_try_write((uv_stream_t*)&priv->uv_tty_out, &buf, 1);

	TN_GUARD_CLEANUP(uv_run(&priv->uv_loop, UV_RUN_ONCE));

    uv_tty_reset_mode();

	tn_mutex_cleanup(&term->mtx);
	return;

cleanup:
	printf("term thread error\n");
}

int main(void)
{
    test_io();
    return 0;
}
