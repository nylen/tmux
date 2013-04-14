/* $Id$ */

/*
 * Copyright (c) 2012 Thomas Adam <thomas@xteddy.org>
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

#include <ctype.h>
#include <stdlib.h>

#include <string.h>

#include "tmux.h"

#define CMD_CHOOSE_LIST_DEFAULT_TEMPLATE "run-shell '%%'"

/*
 * Enter choose mode to choose a custom list.
 */

enum cmd_retval cmd_choose_list_exec(struct cmd *, struct cmd_q *);
void		cmd_choose_list_prepare(struct cmd *, struct cmd_q *);

const struct cmd_entry cmd_choose_list_entry = {
	"choose-list", NULL,
	"l:t:", 0, 1,
	"[-l items] " CMD_TARGET_WINDOW_USAGE "[template]",
	0,
	NULL,
	NULL,
	cmd_choose_list_exec,
	cmd_choose_list_prepare
};

void
cmd_choose_list_prepare(struct cmd *self, struct cmd_q *cmdq)
{
	struct args	*args = self->args;

	cmdq->cmd_ctx.c = cmd_current_client(cmdq);
	cmdq->cmd_ctx.wl = cmd_find_window(cmdq, args_get(args, 't'), NULL);
}

enum cmd_retval
cmd_choose_list_exec(struct cmd *self, struct cmd_q *cmdq)
{
	struct args			*args = self->args;
	struct client			*c;
	struct winlink			*wl;
	const char			*list1;
	char				*template, *item, *copy, *list;
	u_int				 idx;

	if ((c = cmdq->cmd_ctx.c) == NULL) {
		cmdq_error(cmdq, "no client available");
		return (CMD_RETURN_ERROR);
	}

	if ((list1 = args_get(args, 'l')) == NULL)
		return (CMD_RETURN_ERROR);

	if ((wl = cmdq->cmd_ctx.wl) == NULL)
		return (CMD_RETURN_ERROR);

	if (window_pane_set_mode(wl->window->active, &window_choose_mode) != 0)
		return (CMD_RETURN_NORMAL);

	if (args->argc != 0)
		template = xstrdup(args->argv[0]);
	else
		template = xstrdup(CMD_CHOOSE_LIST_DEFAULT_TEMPLATE);

	copy = list = xstrdup(list1);
	idx = 0;
	while ((item = strsep(&list, ",")) != NULL)
	{
		if (*item == '\0') /* no empty entries */
			continue;
		window_choose_add_item(wl->window->active, c, wl, item,
		    template, idx);
		idx++;
	}
	free(copy);

	if (idx == 0) {
		free(template);
		window_pane_reset_mode(wl->window->active);
		return (CMD_RETURN_ERROR);
	}

	window_choose_ready(wl->window->active, 0, NULL);

	free(template);

	return (CMD_RETURN_NORMAL);
}
