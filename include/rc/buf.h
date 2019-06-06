#ifndef RC_BUF_H
#define RC_BUF_H

#include "aws/common/byte_buf.h"

typedef struct rc_buf_s {
	struct aws_byte_buf bb;
	uint16_t pos;
} rc_buf_t;

int rc_buf_setup(rc_buf_t *buf, uint16_t capacity);
void rc_buf_cleanup(rc_buf_t *buf);

uint16_t rc_buf_length(rc_buf_t *buf);
uint16_t rc_buf_capacity(rc_buf_t *buf);
char *rc_buf_slice(rc_buf_t *buf, uint16_t offset);

void rc_buf_clear(rc_buf_t *buf);
int rc_buf_insert(rc_buf_t *buf, char c);
int rc_buf_delete(rc_buf_t *buf);

uint16_t rc_buf_cursor_get(rc_buf_t *buf);
int rc_buf_cursor_set(rc_buf_t *buf, uint16_t pos);

#endif
