load("//bzl/httpfileserver:http-file-server.bzl", "http_file_server")

http_file_server(
    name = "serve",
    data = [
        ":index",
    ],
    https = False,  # Default is True
    port = 4444,  # Default is 8888
    root_redirect = "bzl/examples/web/client/index.html",  # Default is ""
)

filegroup(
    name = "index",
    srcs = [
        "index.html",
    ],
)
