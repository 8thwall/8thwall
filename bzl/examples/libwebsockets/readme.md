## Libwebsockets example

The test code runs a client and an echo server in one process. The client sends 2 messages
and finishes when

### How to build&run
bazel run //bzl/examples/libwebsockets:websocket_test

Example output looks like this.
```
[2022/11/09 11:49:22:5963] U: LWS minimal ws client echo + permessage-deflate + multifragment bulk message
[2022/11/09 11:49:22:5968] U:    lws-minimal-ws-client-echo [-n (no exts)] [-p port] [-o (once)]
[2022/11/09 11:49:22:5968] N: lws_create_context: LWS: 4.3.2-unknown, NET CLI SRV H1 H2 WS ConMon IPv6-absent
[2022/11/09 11:49:22:5968] N: lws_create_context: LWS: 4.3.2-unknown, NET CLI SRV H1 H2 WS ConMon IPv6-absent
[2022/11/09 11:49:22:5969] N: __lws_lc_tag:  ++ [wsi|0|pipe] (1)
[2022/11/09 11:49:22:5970] N: __lws_lc_tag:  ++ [vh|0|default||-1] (1)
[2022/11/09 11:49:22:5972] N: __lws_lc_tag:  ++ [wsi|0|pipe] (1)
[2022/11/09 11:49:22:5973] N: __lws_lc_tag:  ++ [vh|0|default||7681] (1)
[2022/11/09 11:49:22:5984] N: [vh|0|default||7681]: lws_socket_bind: source ads 0.0.0.0
[2022/11/09 11:49:22:5985] N: __lws_lc_tag:  ++ [wsi|1|listen|default||7681] (2)
[2022/11/09 11:49:22:6008] N: __lws_lc_tag:  ++ [wsicli|0|WS/h1/default/localhost] (1)
[2022/11/09 11:49:22:6018] N: __lws_lc_tag:  ++ [wsisrv|0|adopted] (1)
[2022/11/09 11:49:22:6019] W: [Server] LWS_CALLBACK_ESTABLISHED
[2022/11/09 11:49:22:6019] U: [Client] callback_minimal: established
[2022/11/09 11:49:25:6021] U: client wrote 11 bytes
[2022/11/09 11:49:25:6022] U: [Server] LWS_CALLBACK_RECEIVE:   11 (rpp     0, first 1, last 1, bin 0, msglen 0 (+ 11 = 11))
[2022/11/09 11:49:25:6024] U: [Server] LWS_CALLBACK_SERVER_WRITEABLE
[2022/11/09 11:49:25:6024] U:  wrote 11: flags: 0x0 first: 1 final 1
[2022/11/09 11:49:25:6024] U: [Server] LWS_CALLBACK_SERVER_WRITEABLE
[2022/11/09 11:49:25:6024] U:  (nothing in ring)
[2022/11/09 11:49:25:6024] U: [Client] Recieved data
[2022/11/09 11:49:25:6024] N:
[2022/11/09 11:49:25:6024] N: 0000: 7A 7A 7A 7A 5F 68 65 6C 6C 6F 2E                   zzzz_hello.
[2022/11/09 11:49:25:6024] N:
[2022/11/09 11:49:26:6772] N: __lws_lc_tag:  ++ [wsicli|1|WS/h1/default/localhost] (2)
[2022/11/09 11:49:26:6860] N: __lws_lc_tag:  ++ [wsisrv|1|adopted] (2)
[2022/11/09 11:49:26:6862] W: [Server] LWS_CALLBACK_ESTABLISHED
[2022/11/09 11:49:26:6862] U: [Client] callback_minimal: established
[2022/11/09 11:49:28:6031] U: client wrote 11 bytes
[2022/11/09 11:49:28:6032] U: [Server] LWS_CALLBACK_RECEIVE:   11 (rpp     0, first 1, last 1, bin 0, msglen 0 (+ 11 = 11))
[2022/11/09 11:49:28:6037] U: [Server] LWS_CALLBACK_SERVER_WRITEABLE
[2022/11/09 11:49:28:6037] U:  wrote 11: flags: 0x0 first: 1 final 1
[2022/11/09 11:49:28:6038] U: [Client] Recieved data
[2022/11/09 11:49:28:6038] N:
[2022/11/09 11:49:28:6038] N: 0000: 7A 7A 7A 7A 5F 68 65 6C 6C 6F 2E                   zzzz_hello.
[2022/11/09 11:49:28:6038] N:
[2022/11/09 11:49:28:6038] N: __lws_lc_untag:  -- [wsi|0|pipe] (0) 6.006s
[2022/11/09 11:49:28:6039] N: __lws_lc_untag:  -- [wsicli|1|WS/h1/default/localhost] (1) 1.926s
[2022/11/09 11:49:28:6060] N: __lws_lc_untag:  -- [vh|0|default||-1] (0) 6.009s
[2022/11/09 11:49:28:6060] N: __lws_lc_untag:  -- [wsicli|0|WS/h1/default/localhost] (0) 6.005s
[2022/11/09 11:49:28:6061] U: [Client] Completed
[2022/11/09 11:49:30:6041] N: __lws_lc_untag:  -- [wsi|0|pipe] (1) 8.006s
[2022/11/09 11:49:30:6041] U: [Server] LWS_CALLBACK_CLOSED
[2022/11/09 11:49:30:6042] N: __lws_lc_untag:  -- [wsisrv|1|adopted] (1) 3.918s
[2022/11/09 11:49:30:6042] U: [Server] LWS_CALLBACK_CLOSED
[2022/11/09 11:49:30:6043] N: __lws_lc_untag:  -- [wsisrv|0|adopted] (0) 8.002s
[2022/11/09 11:49:30:6044] N: __lws_lc_untag:  -- [vh|0|default||7681] (0) 8.007s
[2022/11/09 11:49:30:6044] N: __lws_lc_untag:  -- [wsi|1|listen|default||7681] (0) 8.005s
[2022/11/09 11:49:30:6064] U: [Server] Completed
```
