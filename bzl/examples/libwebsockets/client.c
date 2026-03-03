// Adapted from minimal-ws-client
// This is C code

#include <libwebsockets.h>
#include <string.h>
#include <signal.h>

const uint32_t MAX_SENDING_MESSAGE_COUNT = 2;
static uint32_t sent_message = 0;
const uint32_t MAX_RECEIVING_MESSAGE_COUNT = 2;
static uint32_t received_message = 0;

static struct my_conn {
	lws_sorted_usec_list_t	sul;	     /* schedule connection retry */
	struct lws		*wsi;	     /* related wsi if any */
	uint16_t		retry_count; /* count of consequetive retries */
} mco;

static struct lws_context *context;
//static int interrupted, port = 443, ssl_connection = LCCSCF_USE_SSL; // default
static int interrupted, port = 7681, ssl_connection = LCCSCF_USE_SSL;
static const char *server_address = "localhost",
		  *pro = "dumb-increment-protocol";

static const uint32_t backoff_ms[] = { 1000, 2000, 3000, 4000, 5000 };

static const lws_retry_bo_t retry = {
	.retry_ms_table			= backoff_ms,
	.retry_ms_table_count		= LWS_ARRAY_SIZE(backoff_ms),
	.conceal_count			= LWS_ARRAY_SIZE(backoff_ms),

	.secs_since_valid_ping		= 3,  /* force PINGs after secs idle */
	.secs_since_valid_hangup	= 10, /* hangup after secs idle */

	.jitter_percent			= 20,
};

static void
connect_client(lws_sorted_usec_list_t *sul)
{
	struct my_conn *m = lws_container_of(sul, struct my_conn, sul);
	struct lws_client_connect_info i;

	memset(&i, 0, sizeof(i));

	i.context = context;
	i.port = port;
	i.address = server_address;
	i.path = "/";
	i.host = i.address;
	//i.origin = i.address; // needed??
	//i.ssl_connection = ssl_connection; // enable ssl when connecting to proper secure host
	i.ssl_connection = 0;
	//i.protocol = pro; // not sure how this "protocol" work. not set here.
	i.local_protocol_name = "lws-minimal-client";
	i.pwsi = &m->wsi;
	i.retry_and_idle_policy = &retry;
	i.userdata = m;

	if (!lws_client_connect_via_info(&i))
		/*
		 * Failed... schedule a retry... we can't use the _retry_wsi()
		 * convenience wrapper api here because no valid wsi at this
		 * point.
		 */
		if (lws_retry_sul_schedule(context, 0, sul, &retry,
					   connect_client, &m->retry_count)) {
			lwsl_err("[Client] %s: connection attempts exhausted\n", __func__);
			interrupted = 1;
		}
}

static int
callback_minimal(struct lws *wsi, enum lws_callback_reasons reason,
		 void *user, void *in, size_t len)
{
	struct my_conn *m = (struct my_conn *)user;

	switch (reason) {

	case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
		lwsl_err("[Client] CLIENT_CONNECTION_ERROR: %s\n",
			 in ? (char *)in : "(null)");
		goto do_retry;
		break;

	case LWS_CALLBACK_CLIENT_RECEIVE:
		lwsl_user("[Client] Recieved data");
		lwsl_hexdump_notice(in, len);
		received_message++;
		break;

	case LWS_CALLBACK_CLIENT_ESTABLISHED:
		lwsl_user("[Client] %s: established\n", __func__);
		break;

	case LWS_CALLBACK_CLIENT_WRITEABLE:
	{
		if (sent_message < MAX_SENDING_MESSAGE_COUNT) {
			// send a test message to the server
			char msg[] = "zzzz_hello.";
			size_t msg_size = strlen(msg);
			int m = lws_write(wsi, (unsigned char*)msg ,msg_size, LWS_WRITE_TEXT);
			lwsl_user("client wrote %d bytes\n", m);
			sent_message++;
		}
	}

	case LWS_CALLBACK_CLIENT_CLOSED:
		goto do_retry;

	case LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER:
	{
		// Set http headers if host requires
		/*
		unsigned char **p = (unsigned char **)in, *end = (*p) + len;

		if (lws_add_http_header_by_name(wsi,
				(const unsigned char *)"io.nia.ctx.usertoken",
				(const unsigned char *)"12345", 5, p, end)) {
			return -1;
		}
		*/
		break;
	}

	default:
		break;
	}

	return lws_callback_http_dummy(wsi, reason, user, in, len);

do_retry:
	/*
	 * retry the connection to keep it nailed up
	 *
	 * For this example, we try to conceal any problem for one set of
	 * backoff retries and then exit the app.
	 *
	 * If you set retry.conceal_count to be larger than the number of
	 * elements in the backoff table, it will never give up and keep
	 * retrying at the last backoff delay plus the random jitter amount.
	 */
	if (lws_retry_sul_schedule_retry_wsi(wsi, &m->sul, connect_client,
					     &m->retry_count)) {
		lwsl_err("%s: connection attempts exhausted\n", __func__);
		interrupted = 1;
	}

	return 0;
}

static const struct lws_protocols protocols[] = {
	{ "lws-minimal-client", callback_minimal, 0, 0, 0, NULL, 0 },
	LWS_PROTOCOL_LIST_TERM
};

void RunClient() {

	struct lws_context_creation_info info;
	int n = 0;

	memset(&info, 0, sizeof info);

	info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
	info.port = CONTEXT_PORT_NO_LISTEN; /* we do not run any server */
	info.protocols = protocols;

	info.fd_limit_per_thread = 1 + 1 + 1;

	context = lws_create_context(&info);
	if (!context) {
		lwsl_err("lws init failed\n");
		return ;
	}

	/* schedule the first client connection attempt to happen immediately */
	lws_sul_schedule(context, 0, &mco.sul, connect_client, 1);

	while (n >= 0 && !interrupted &&
				(received_message < MAX_RECEIVING_MESSAGE_COUNT ||
				sent_message < MAX_SENDING_MESSAGE_COUNT)) {
		n = lws_service(context, 0);
	}

	lws_context_destroy(context);
	lwsl_user("[Client] Completed\n");

}

void StopClient() {
	interrupted = 1;
}
