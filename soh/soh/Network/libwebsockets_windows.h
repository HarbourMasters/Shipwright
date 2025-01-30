// undef a couple int types
#undef u32
#undef u64

// undef all the HTTP response codes from wininet.h
#undef HTTP_STATUS_CONTINUE
#undef HTTP_STATUS_SWITCH_PROTOCOLS
#undef HTTP_STATUS_OK
#undef HTTP_STATUS_CREATED
#undef HTTP_STATUS_ACCEPTED
#undef HTTP_STATUS_PARTIAL
#undef HTTP_STATUS_NO_CONTENT
#undef HTTP_STATUS_RESET_CONTENT
#undef HTTP_STATUS_PARTIAL_CONTENT
#undef HTTP_STATUS_AMBIGUOUS
#undef HTTP_STATUS_MOVED
#undef HTTP_STATUS_REDIRECT
#undef HTTP_STATUS_REDIRECT_METHOD
#undef HTTP_STATUS_NOT_MODIFIED
#undef HTTP_STATUS_USE_PROXY
#undef HTTP_STATUS_REDIRECT_KEEP_VERB
#undef HTTP_STATUS_PERMANENT_REDIRECT
#undef HTTP_STATUS_BAD_REQUEST
#undef HTTP_STATUS_DENIED
#undef HTTP_STATUS_PAYMENT_REQ
#undef HTTP_STATUS_FORBIDDEN
#undef HTTP_STATUS_NOT_FOUND
#undef HTTP_STATUS_BAD_METHOD
#undef HTTP_STATUS_NONE_ACCEPTABLE
#undef HTTP_STATUS_PROXY_AUTH_REQ
#undef HTTP_STATUS_REQUEST_TIMEOUT
#undef HTTP_STATUS_CONFLICT
#undef HTTP_STATUS_GONE
#undef HTTP_STATUS_LENGTH_REQUIRED
#undef HTTP_STATUS_PRECOND_FAILED
#undef HTTP_STATUS_REQUEST_TOO_LARGE
#undef HTTP_STATUS_URI_TOO_LONG
#undef HTTP_STATUS_UNSUPPORTED_MEDIA
#undef HTTP_STATUS_MISDIRECTED_REQUEST
#undef HTTP_STATUS_RETRY_WITH
#undef HTTP_STATUS_SERVER_ERROR
#undef HTTP_STATUS_NOT_SUPPORTED
#undef HTTP_STATUS_BAD_GATEWAY
#undef HTTP_STATUS_SERVICE_UNAVAIL
#undef HTTP_STATUS_GATEWAY_TIMEOUT
#undef HTTP_STATUS_VERSION_NOT_SUP
#undef HTTP_STATUS_FIRST
#undef HTTP_STATUS_LAST

// actually include libwebsockets
#include <libwebsockets.h>

// redefine the int types
#define u32 uint32_t
#define u64 uint64_t

// redefine the responses from wininet.h
#define HTTP_STATUS_CONTINUE            100 // OK to continue with request
#define HTTP_STATUS_SWITCH_PROTOCOLS    101 // server has switched protocols in upgrade header

#define HTTP_STATUS_OK                  200 // request completed
#define HTTP_STATUS_CREATED             201 // object created, reason = new URI
#define HTTP_STATUS_ACCEPTED            202 // async completion (TBS)
#define HTTP_STATUS_PARTIAL             203 // partial completion
#define HTTP_STATUS_NO_CONTENT          204 // no info to return
#define HTTP_STATUS_RESET_CONTENT       205 // request completed, but clear form
#define HTTP_STATUS_PARTIAL_CONTENT     206 // partial GET furfilled

#define HTTP_STATUS_AMBIGUOUS           300 // server couldn't decide what to return
#define HTTP_STATUS_MOVED               301 // object permanently moved
#define HTTP_STATUS_REDIRECT            302 // object temporarily moved
#define HTTP_STATUS_REDIRECT_METHOD     303 // redirection w/ new access method
#define HTTP_STATUS_NOT_MODIFIED        304 // if-modified-since was not modified
#define HTTP_STATUS_USE_PROXY           305 // redirection to proxy, location header specifies proxy to use
#define HTTP_STATUS_REDIRECT_KEEP_VERB  307 // HTTP/1.1: keep same verb
#define HTTP_STATUS_PERMANENT_REDIRECT  308 // Object permanently moved keep verb

#define HTTP_STATUS_BAD_REQUEST         400 // invalid syntax
#define HTTP_STATUS_DENIED              401 // access denied
#define HTTP_STATUS_PAYMENT_REQ         402 // payment required
#define HTTP_STATUS_FORBIDDEN           403 // request forbidden
#define HTTP_STATUS_NOT_FOUND           404 // object not found
#define HTTP_STATUS_BAD_METHOD          405 // method is not allowed
#define HTTP_STATUS_NONE_ACCEPTABLE     406 // no response acceptable to client found
#define HTTP_STATUS_PROXY_AUTH_REQ      407 // proxy authentication required
#define HTTP_STATUS_REQUEST_TIMEOUT     408 // server timed out waiting for request
#define HTTP_STATUS_CONFLICT            409 // user should resubmit with more info
#define HTTP_STATUS_GONE                410 // the resource is no longer available
#define HTTP_STATUS_LENGTH_REQUIRED     411 // the server refused to accept request w/o a length
#define HTTP_STATUS_PRECOND_FAILED      412 // precondition given in request failed
#define HTTP_STATUS_REQUEST_TOO_LARGE   413 // request entity was too large
#define HTTP_STATUS_URI_TOO_LONG        414 // request URI too long
#define HTTP_STATUS_UNSUPPORTED_MEDIA   415 // unsupported media type
#define HTTP_STATUS_MISDIRECTED_REQUEST 421 // misdirected request
#define HTTP_STATUS_RETRY_WITH          449 // retry after doing the appropriate action.

#define HTTP_STATUS_SERVER_ERROR        500 // internal server error
#define HTTP_STATUS_NOT_SUPPORTED       501 // required not supported
#define HTTP_STATUS_BAD_GATEWAY         502 // error response received from gateway
#define HTTP_STATUS_SERVICE_UNAVAIL     503 // temporarily overloaded
#define HTTP_STATUS_GATEWAY_TIMEOUT     504 // timed out waiting for gateway
#define HTTP_STATUS_VERSION_NOT_SUP     505 // HTTP version not supported

#define HTTP_STATUS_FIRST               HTTP_STATUS_CONTINUE
#define HTTP_STATUS_LAST                HTTP_STATUS_VERSION_NOT_SUP
