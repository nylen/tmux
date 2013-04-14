/* $Id$ */

/*
 * Copyright (c) 2008 Nicholas Marriott <nicm@users.sourceforge.net>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF MIND, USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/types.h>

#include <pwd.h>
#include <string.h>
#include <unistd.h>

#include "tmux.h"

/*
 * Lock commands.
 */

enum cmd_retval	 cmd_lock_server_exec(struct cmd *, struct cmd_q *);
void		 cmd_lock_server_prepare(struct cmd *, struct cmd_q *);

const struct cmd_entry cmd_lock_server_entry = {
	"lock-server", "lock",
	"", 0, 0,
	"",
	0,
	NULL,
	NULL,
	cmd_lock_server_exec,
	cmd_lock_server_prepare,
};

const struct cmd_entry cmd_lock_session_entry = {
	"lock-session", "locks",
	"t:", 0, 0,
	CMD_TARGET_SESSION_USAGE,
	0,
	NULL,
	NULL,
	cmd_lock_server_exec,
	cmd_lock_server_prepare
};

const struct cmd_entry cmd_lock_client_entry = {
	"lock-client", "lockc",
	"t:", 0, 0,
	CMD_TARGET_CLIENT_USAGE,
	0,
	NULL,
	NULL,
	cmd_lock_server_exec,
	cmd_lock_server_prepare
};

void
cmd_lock_server_prepare(struct cmd *self, struct cmd_q *cmdq)
{
	struct args	*args = self->args;

	if (self->entry == &cmd_lock_session_entry) {
		cmdq->cmd_ctx.s = cmd_find_session(cmdq,
				args_get(args, 't'), 0);
	} else if (self->entry == &cmd_lock_client_entry) {
		cmdq->cmd_ctx.c = cmd_find_client(cmdq,
				args_get(args, 't'), 0);
	}
}

enum cmd_retval
cmd_lock_server_exec(struct cmd *self, unused struct cmd_q *cmdq)
{
	struct client	*c;
	struct session	*s;

	if (self->entry == &cmd_lock_server_entry)
		server_lock();
	else if (self->entry == &cmd_lock_session_entry) {
		if ((s = cmdq->cmd_ctx.s) == NULL)
			return (CMD_RETURN_ERROR);
		server_lock_session(s);
	} else {
		if ((c = cmdq->cmd_ctx.c) == NULL)
			return (CMD_RETURN_ERROR);
		server_lock_client(c);
	}
	recalculate_sizes();

	return (CMD_RETURN_NORMAL);
}
