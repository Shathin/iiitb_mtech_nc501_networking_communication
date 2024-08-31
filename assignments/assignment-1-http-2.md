# HTTP/2

## Introduction

HTTP/2 (original named or HTTP/2.0) was developed by the HTTP Working Group of the Internet Engineering Task Force (IETF). It was derived from an experimental technology by Google called SPDY. The HTTP/2 specification was published as RFC 7540 on May 14, 2015.

HTTP/2 does not modify the application semantics of HTTP in any way. All the core concepts, such as HTTP methods, status codes, URIs, and header fields, remain in place. Instead, HTTP/2 modifies how the data is formatted (framed) and transported between the client and server, both of which manage the entire process, and hides all the complexity from our applications within the new framing layer. As a result, all existing applications can be delivered without modification.

## Goals

The primary goals for HTTP/2 are to - 

1. Reduce latency by enabling full request and response multiplexing, 
2. Minimize protocol overhead via efficient compression of HTTP header fields, and,
3. Add support for request prioritization and server push.

## Features

### Streams, Messages & Frames

We start off with a few basic terminologies related to HTTP/2

- **Stream**: A bidirectional flow of bytes within an established connection, which may carry one or more messages.
- **Message**: A complete sequence of frames that map to a logical request or response message.
- **Frame**: The smallest unit of communication in HTTP/2, each containing a frame header, which at a minimum identifies the stream to which the frame belongs.

The relation of these terms can be summarized as follows:

- All communication is performed over a single TCP connection that can carry any number of bidirectional streams.
- Each stream has a unique identifier and optional priority information that is used to carry bidirectional messages.
- Each message is a logical HTTP message, such as a request, or response, which consists of one or more frames.
- The frame is the smallest unit of communication that carries a specific type of data—e.g., HTTP headers, message payload, and so on. Frames from different streams may be interleaved and then reassembled via the embedded stream identifier in the header of each frame.

![HTTP 2.0 Steam](/assets/assignment-1-http-2-1.png)

In short, HTTP/2 breaks down the HTTP protocol communication into an exchange of binary-encoded frames, which are then mapped to messages that belong to a particular stream, all of which are multiplexed within a single TCP connection.

### HTTP/1 vs HTTP2 message

![HTTP/1 vs HTTP/2](/assets/assignment-1-http-2-2.png)

### Request & Response Multiplexing

With HTTP/1.x, if the client wants to make multiple parallel requests to improve performance, then multiple TCP connections must be used. This is an inefficient use of the underlying TCP connection.

The new binary framing layer in HTTP/2 removes these limitations, and enables full request and response multiplexing, by allowing the client and server to break down an HTTP message into independent frames, interleave them, and then reassemble them on the other end.

![Req-Res Multiplexing](/assets/assignment-1-http-2-3.png)

The ability to break down an HTTP message into independent frames, interleave them, and then reassemble them on the other end is the single most important enhancement of HTTP/2. In fact, it introduces a ripple effect of numerous performance benefits across the entire stack of all web technologies, enabling us to:

- Interleave multiple requests in parallel without blocking on any one.
- Interleave multiple responses in parallel without blocking on any one.
- Use a single connection to deliver multiple requests and responses in parallel.
- And much more

### Server Push

Another powerful new feature of HTTP/2 is the ability of the server to send multiple responses for a single client request. That is, in addition to the response to the original request, the server can push additional resources to the client (Figure 12-5), without the client having to request each one explicitly. The advantage of such a feature is latency is reduced because the server knows what resources are needed by the client and it pushes it along with the other resources.

![Server Push](/assets/assignment-1-http-2-4.png)

### Header Compression

Each HTTP transfer carries a set of headers that describe the transferred resource and its properties. In HTTP/1.x, this metadata is always sent as plain text and adds anywhere from 500–800 bytes of overhead per transfer, and sometimes kilobytes more if HTTP cookies are being used. (See [Measuring and Controlling Protocol Overhead](https://hpbn.co/http1x/#measuring-and-controlling-protocol-overhead)) To reduce this overhead and improve performance, HTTP/2 compresses request and response header metadata using the HPACK compression format that uses two simple but powerful techniques:

1. It allows the transmitted header fields to be encoded via a static Huffman code, which reduces their individual transfer size.
2. It requires that both the client and server maintain and update an indexed list of previously seen header fields (in other words, it establishes a shared compression context), which is then used as a reference to efficiently encode previously transmitted values.

![Header Compression](/assets/assignment-1-http-2-5.png)
