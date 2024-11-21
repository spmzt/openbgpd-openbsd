/*	$OpenBSD: imsg.h,v 1.10 2024/11/21 12:47:27 claudio Exp $	*/

/*
 * Copyright (c) 2023 Claudio Jeker <claudio@openbsd.org>
 * Copyright (c) 2006, 2007 Pierre-Yves Ritschard <pyr@openbsd.org>
 * Copyright (c) 2006, 2007, 2008 Reyk Floeter <reyk@openbsd.org>
 * Copyright (c) 2003, 2004 Henning Brauer <henning@openbsd.org>
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

#ifndef _IMSG_H_
#define _IMSG_H_

#include <sys/types.h>

#define IBUF_READ_SIZE		65535
#define IMSG_HEADER_SIZE	sizeof(struct imsg_hdr)
#define MAX_IMSGSIZE		16384

struct ibuf {
	TAILQ_ENTRY(ibuf)	 entry;
	unsigned char		*buf;
	size_t			 size;
	size_t			 max;
	size_t			 wpos;
	size_t			 rpos;
	int			 fd;
};

struct msgbuf {
	TAILQ_HEAD(, ibuf)	 bufs;
	uint32_t		 queued;
	int			 fd;
};

struct ibuf_read {
	unsigned char		 buf[IBUF_READ_SIZE];
	unsigned char		*rptr;
	size_t			 wpos;
};

struct imsg_fd;
struct imsgbuf {
	TAILQ_HEAD(, imsg_fd)	 fds;
	struct ibuf_read	 r;
	struct msgbuf		 w;
	int			 fd;
	pid_t			 pid;
};

#define IMSGF_HASFD	1

struct imsg_hdr {
	uint32_t	 type;
	uint16_t	 len;
	uint16_t	 flags;
	uint32_t	 peerid;
	uint32_t	 pid;
};

struct imsg {
	struct imsg_hdr	 hdr;
	void		*data;
	struct ibuf	*buf;
};

struct iovec;

/* imsg-buffer.c */
struct ibuf	*ibuf_open(size_t);
struct ibuf	*ibuf_dynamic(size_t, size_t);
int		 ibuf_add(struct ibuf *, const void *, size_t);
int		 ibuf_add_buf(struct ibuf *, const struct ibuf *);
int		 ibuf_add_ibuf(struct ibuf *, const struct ibuf *);
int		 ibuf_add_zero(struct ibuf *, size_t);
int		 ibuf_add_n8(struct ibuf *, uint64_t);
int		 ibuf_add_n16(struct ibuf *, uint64_t);
int		 ibuf_add_n32(struct ibuf *, uint64_t);
int		 ibuf_add_n64(struct ibuf *, uint64_t);
int		 ibuf_add_h16(struct ibuf *, uint64_t);
int		 ibuf_add_h32(struct ibuf *, uint64_t);
int		 ibuf_add_h64(struct ibuf *, uint64_t);
void		*ibuf_reserve(struct ibuf *, size_t);
void		*ibuf_seek(struct ibuf *, size_t, size_t);
int		 ibuf_set(struct ibuf *, size_t, const void *, size_t);
int		 ibuf_set_n8(struct ibuf *, size_t, uint64_t);
int		 ibuf_set_n16(struct ibuf *, size_t, uint64_t);
int		 ibuf_set_n32(struct ibuf *, size_t, uint64_t);
int		 ibuf_set_n64(struct ibuf *, size_t, uint64_t);
int		 ibuf_set_h16(struct ibuf *, size_t, uint64_t);
int		 ibuf_set_h32(struct ibuf *, size_t, uint64_t);
int		 ibuf_set_h64(struct ibuf *, size_t, uint64_t);
void		*ibuf_data(const struct ibuf *);
size_t		 ibuf_size(const struct ibuf *);
size_t		 ibuf_left(const struct ibuf *);
int		 ibuf_truncate(struct ibuf *, size_t);
void		 ibuf_rewind(struct ibuf *);
void		 ibuf_close(struct msgbuf *, struct ibuf *);
void		 ibuf_from_buffer(struct ibuf *, void *, size_t);
void		 ibuf_from_ibuf(struct ibuf *, const struct ibuf *);
int		 ibuf_get(struct ibuf *, void *, size_t);
int		 ibuf_get_ibuf(struct ibuf *, size_t, struct ibuf *);
int		 ibuf_get_n8(struct ibuf *, uint8_t *);
int		 ibuf_get_n16(struct ibuf *, uint16_t *);
int		 ibuf_get_n32(struct ibuf *, uint32_t *);
int		 ibuf_get_n64(struct ibuf *, uint64_t *);
int		 ibuf_get_h16(struct ibuf *, uint16_t *);
int		 ibuf_get_h32(struct ibuf *, uint32_t *);
int		 ibuf_get_h64(struct ibuf *, uint64_t *);
char		*ibuf_get_string(struct ibuf *, size_t);
int		 ibuf_skip(struct ibuf *, size_t);
void		 ibuf_free(struct ibuf *);
int		 ibuf_fd_avail(struct ibuf *);
int		 ibuf_fd_get(struct ibuf *);
void		 ibuf_fd_set(struct ibuf *, int);
int		 ibuf_write(struct msgbuf *);
void		 msgbuf_init(struct msgbuf *);
void		 msgbuf_clear(struct msgbuf *);
uint32_t	 msgbuf_queuelen(struct msgbuf *);
int		 msgbuf_write(struct msgbuf *);

/* imsg.c */
void	 imsg_init(struct imsgbuf *, int);
ssize_t	 imsg_read(struct imsgbuf *);
ssize_t	 imsg_get(struct imsgbuf *, struct imsg *);
int	 imsg_get_ibuf(struct imsg *, struct ibuf *);
int	 imsg_get_data(struct imsg *, void *, size_t);
int	 imsg_get_fd(struct imsg *);
uint32_t imsg_get_id(struct imsg *);
size_t	 imsg_get_len(struct imsg *);
pid_t	 imsg_get_pid(struct imsg *);
uint32_t imsg_get_type(struct imsg *);
int	 imsg_forward(struct imsgbuf *, struct imsg *);
int	 imsg_compose(struct imsgbuf *, uint32_t, uint32_t, pid_t, int,
	    const void *, size_t);
int	 imsg_composev(struct imsgbuf *, uint32_t, uint32_t,  pid_t, int,
	    const struct iovec *, int);
int	 imsg_compose_ibuf(struct imsgbuf *, uint32_t, uint32_t, pid_t,
	    struct ibuf *);
struct ibuf *imsg_create(struct imsgbuf *, uint32_t, uint32_t, pid_t, size_t);
int	 imsg_add(struct ibuf *, const void *, size_t);
void	 imsg_close(struct imsgbuf *, struct ibuf *);
void	 imsg_free(struct imsg *);
int	 imsg_flush(struct imsgbuf *);
void	 imsg_clear(struct imsgbuf *);

#endif
