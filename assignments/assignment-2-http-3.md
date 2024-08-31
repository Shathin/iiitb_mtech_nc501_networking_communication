# HTTP/3

HTTP/3 is the third and upcoming major version of HTTP used to exchange information on the World Wide Web, alongside HTTP/1.1 and HTTP/2.

HTTP semantics remain unchanged. The differences are in the mapping of these semantics to underlying transports. Both HTTP/1.1 and HTTP/2 use TCP as their transport where as HTTP/3 uses QUIC.

The original proposal for HTTP/3 was named "HTTP/2 Semantics Using The QUIC Transport Protocol" and later named "Hypertext Transfer Protocol (HTTP) over QUIC". It was then finally renamed to HTTP/3.

## Brief about QUIC

QUIC (pronounced "quick") is a general-purpose transport layer network protocol initially designed by Jim Roskind at Google.

Among other applications, QUIC improves the performance of connection-oriented web applications that are currently using TCP. It does this by establishing a number of multiplexed connections between two endpoints over the User Datagram Protocol (UDP). This works hand-in-hand with HTTP/2's multiplexed connections, allowing multiple streams of data to reach all the endpoints independently, and hence independent of packet losses involving other streams. In contrast, HTTP/2 hosted on TCP can suffer head-of-line-blocking delays of all multiplexed streams if any of the TCP packets are delayed or lost.

Head-of-line blocking (HOL blocking) is a performance-limiting phenomenon that occurs when a line of packets is held up by the first packet. HOL blocking occurs in HTTP/1.1 is when the number of allowed parallel requests in the browser is used up, and subsequent requests need to wait for the former ones to complete. [HTTP/2](https://en.wikipedia.org/wiki/HTTP/2) addresses this issue through request multiplexing, which eliminates HOL blocking at the application layer, but HOL still exists at the transport (TCP) layer i.e., when TCP sees an error on a connection as a blocking operation, it stops further transfers until the error is resolved or the connection is considered failed.

## How QUIC is used in HTTP

QUIC aims to be nearly equivalent to a TCP connection but with much-reduced latency. It does this primarily through two changes -

- The first change is to greatly reduce overhead during connection setup. As most HTTP connections will demand TLS, QUIC makes the exchange of setup keys and supported protocols part of the initial handshake process. (Explained in the next section)
- QUIC uses UDP as its basis, which does not include loss recovery. Instead, each QUIC stream is separately flow controlled and lost data retransmitted at the level of QUIC, not UDP. This means that if an error occurs in one stream the protocol stack can continue servicing other streams independently.

![](/assets/assignment-1-http-3-1.png)

The above diagram shows the comparison between HTTP/2 over TCP and HTTP/2 over QUIC (known as HTTP/3). 

## Flow Diagram

QUIC relies on a combined cryptographic and transport handshake for setting up a secure transport connection. On a successful handshake, a client caches information about the origin (An origin is identified by the set of URI scheme, hostname, and port number). On subsequent connections to the same origin, the client can establish an encrypted connection with no additional round trips, and the data can be sent immediately following the client handshake packet without waiting for a reply from the server.

![](/assets/assignment-1-http-3-2.png)

Initial handshake: Initially, the client has no information about the server and so, before a handshake can be attempted, the client sends an inchoate client hello (CHLO) message to the server to elicit a reject (REJ) message. The REJ message contains: 

1. A server config that includes the server’s long-term Diffie-Hellman public value, 
2. A certificate chain authenticating the server, 
3. A signature of the server config using the private key from the leaf certificate of the chain, and 
4. A source-address token: an authenticated-encryption block that contains the client’s publicly visible IP address (as seen at the server) and a timestamp by the server. 

The client sends this token back to the server in later handshakes, demonstrating ownership of its IP address. Once the client has received a server config, it authenticates the config by verifying the certificate chain and signature. It then sends a complete CHLO, containing the client’s ephemeral
Diffie-Hellman public value. 

Final (and repeat) handshake: All keys for a connection are established using Diffie-Hellman. After sending a complete CHLO, the client is in possession of initial keys for the connection since it can calculate the shared value from the server’s long-term DiffieHellman public value and its own ephemeral Diffie-Hellman private key. At this point, the client is free to start sending application data to the server

If the client wishes to achieve 0-RTT latency for data, then it must start sending data encrypted with its initial keys before waiting for the server’s reply. If the handshake is successful, the server returns a server hello (SHLO) message. This message is encrypted using the initial keys and contains the server’s ephemeral Diffie-Hellman public value. With the peer’s ephemeral public value in hand, both sides can calculate the final or forward-secure keys for the connection. Upon sending an SHLO message, the server immediately switches to sending packets encrypted with the forward-secure keys. Upon receiving the SHLO message, the client switches to sending packets encrypted
with the forward-secure keys.

## Other differences between HTTP/2 & HTTP/3

The compression scheme used in HTTP/2 (i.e., HPACK) cannot be used for HTTP/3 since HPACK heavily depends on the order in which different HTTP requests and responses are delivered to the endpoints. QUIC enforces the delivery order of bytes within single streams but does not guarantee ordering among different streams. This behavior required the creation of a new HTTP header compression scheme, called QPACK, which fixes the problem. 

In addition, some of the features offered by HTTP/2 (like per-stream flow control) are already offered by QUIC itself, so they were dropped from HTTP/3 in order to remove unnecessary complexity from the protocol.
