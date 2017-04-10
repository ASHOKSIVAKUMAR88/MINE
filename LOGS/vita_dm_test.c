#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/signalfd.h>
#include <glib.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "lib/bluetooth.h"
#include "lib/sdp.h"
#include "lib/uuid.h"

#include "src/shared/util.h"
#include "btio/btio.h"
#include "att.h"
#include "gattrib.h"
#include "gatt.h"
#include "gatttool.h"
#include "client/display.h"

static GIOChannel *iochannel = NULL;
static GAttrib *attrib = NULL;
static GMainLoop *event_loop;
static GString *prompt;

static char *opt_src = NULL;
static char *opt_dst = NULL;
static char *opt_dst_type = NULL;
static char *opt_sec_level = NULL;
static int opt_psm = 0;
static int opt_mtu = 0;
static int start;
static int end;
opt_dst=g_strdup("00:1A:7D:DA:71:13");
opt_src = g_strdup("00:18:96:01:D9:55");
opt_sec_level = g_strdup("low");
opt_dst_type = g_strdup("public");

//*************************connection status ******************************
static enum state {
	STATE_DISCONNECTED,
	STATE_CONNECTING,
	STATE_CONNECTED
} conn_state;
static void set_state(enum state st)
{
	conn_state = st;
	rl_set_prompt(get_prompt());
}
//*************************connection status ******************************
//*************************connection establishment************************
static void connect_cb(GIOChannel *io, GError *err, gpointer user_data)
{
	uint16_t mtu;
	uint16_t cid;

	if (err) {
		set_state(STATE_DISCONNECTED);
		error("%s\n", err->message);
		return;
	}

	bt_io_get(io, &err, BT_IO_OPT_IMTU, &mtu,
				BT_IO_OPT_CID, &cid, BT_IO_OPT_INVALID);

	if (err) {
		g_printerr("Can't detect MTU, using default: %s", err->message);
		g_error_free(err);
		mtu = ATT_DEFAULT_LE_MTU;
	}

	if (cid == ATT_CID)
		mtu = ATT_DEFAULT_LE_MTU;

	attrib = g_attrib_new(iochannel, mtu, false);
	g_attrib_register(attrib, ATT_OP_HANDLE_NOTIFY, GATTRIB_ALL_HANDLES,
						events_handler, attrib, NULL);
	g_attrib_register(attrib, ATT_OP_HANDLE_IND, GATTRIB_ALL_HANDLES,
						events_handler, attrib, NULL);
	set_state(STATE_CONNECTED);
    printf("mtu %d ",mtu);
	rl_printf("Connection successful\n");
}
static void cmd_connect(int argcp, char **argvp)
{
	GError *gerr = NULL;

	if (conn_state != STATE_DISCONNECTED)
		return;

	if (argcp > 1) {
		g_free(opt_dst);
		opt_dst = g_strdup(argvp[1]);

		g_free(opt_dst_type);
		if (argcp > 2)
			opt_dst_type = g_strdup(argvp[2]);
		else
			opt_dst_type = g_strdup("public");
	}

	if (opt_dst == NULL) {
		error("Remote Bluetooth address required\n");
		return;
	}

	rl_printf("Attempting to connect to %s\n", opt_dst);
	set_state(STATE_CONNECTING);
	iochannel = gatt_connect(opt_src, opt_dst, opt_dst_type, opt_sec_level,
					opt_psm, opt_mtu, connect_cb, &gerr);
    printf("**********psm %d mtu %d \n",opt_psm,opt_mtu);
	if (iochannel == NULL) {
		set_state(STATE_DISCONNECTED);
		error("%s\n", gerr->message);
		g_error_free(gerr);
	} else
		g_io_add_watch(iochannel, G_IO_HUP, channel_watcher, NULL);
}


//*************************connection establishment************************


