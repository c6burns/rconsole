#include "rc/buf.h"

#include "tn/error.h"

// --------------------------------------------------------------------------------------------------------------
int rc_buf_setup(rc_buf_t *buf, uint16_t capacity)
{
	TN_ASSERT(buf);
	TN_GUARD(aws_byte_buf_init(&buf->bb, aws_default_allocator(), capacity));
	buf->bb.buffer[0] = '\0';
	buf->pos = 0;
	return TN_SUCCESS;
}

// --------------------------------------------------------------------------------------------------------------
void rc_buf_cleanup(rc_buf_t *buf)
{
	TN_ASSERT(buf);
	aws_byte_buf_clean_up(&buf->bb);
}

// --------------------------------------------------------------------------------------------------------------
uint16_t rc_buf_length(rc_buf_t *buf)
{
	TN_ASSERT(buf);
	return (uint16_t)buf->bb.len;
}

// --------------------------------------------------------------------------------------------------------------
uint16_t rc_buf_capacity(rc_buf_t *buf)
{
	TN_ASSERT(buf);
	return (uint16_t)buf->bb.capacity;
}

// --------------------------------------------------------------------------------------------------------------
char *rc_buf_slice(rc_buf_t *buf, uint16_t offset)
{
	TN_ASSERT(buf && aws_byte_buf_is_valid(&buf->bb));
	TN_GUARD(offset > buf->bb.len);
	return (buf->bb.buffer + offset);
}

// --------------------------------------------------------------------------------------------------------------
void rc_buf_clear(rc_buf_t *buf)
{
	TN_ASSERT(buf);
	aws_byte_buf_reset(&buf->bb, false);
	buf->pos = 0;
}

// --------------------------------------------------------------------------------------------------------------
int rc_buf_insert(rc_buf_t *buf, char c)
{
	TN_ASSERT(buf && aws_byte_buf_is_valid(&buf->bb));
	TN_GUARD(buf->pos >= buf->bb.capacity - 1);
	const uint16_t copylen = (uint16_t)buf->bb.len - buf->pos;
	if (copylen) memmove(buf->bb.buffer + buf->pos + 1, buf->bb.buffer + buf->pos, copylen);
	buf->bb.buffer[buf->pos] = c;
	buf->bb.len++;
	buf->pos++;
	buf->bb.buffer[buf->bb.len] = '\0';
	return TN_SUCCESS;
}

// --------------------------------------------------------------------------------------------------------------
int rc_buf_delete(rc_buf_t *buf)
{ 
	TN_ASSERT(buf && aws_byte_buf_is_valid(&buf->bb));
	const uint16_t copylen = (uint16_t)buf->bb.len - buf->pos - 1;
	TN_GUARD(copylen > buf->bb.len);
	if (copylen) memmove(buf->bb.buffer + buf->pos, buf->bb.buffer + buf->pos + 1, copylen);
	buf->bb.len--;
	buf->bb.buffer[buf->bb.len] = '\0';
	return TN_SUCCESS;
}

// --------------------------------------------------------------------------------------------------------------
uint16_t rc_buf_cursor_get(rc_buf_t *buf)
{
	TN_ASSERT(buf);
	return buf->pos;
}

// --------------------------------------------------------------------------------------------------------------
int rc_buf_cursor_set(rc_buf_t *buf, uint16_t pos)
{
	TN_ASSERT(buf);
	TN_GUARD(pos > buf->bb.len);
	buf->pos = pos;
	return TN_SUCCESS;
}