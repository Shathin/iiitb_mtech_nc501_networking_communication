# Latency derivation for TCP

> [Source](https://www.net.t-labs.tu-berlin.de/teaching/computer_networking/03.07.htm)

For any given object, **latency** is the time from when the client initiates a TCP connection until when the client receives the requested object in its entirety.

## Modeling Latency: Static Congestion Window

A few assumptions to simplifiy the derivation - 

1. We assume a very large TCP receive buffer, hence, limiting the amount of data that can be transmitted to the sender's congestion window.
2. There are no retransmission due to lost packets or corruption.
3. Ignore all the procotol header overheads of different layers.
4. The object size is an integer multiple of Maximum Segment Size (MSS).
5. The transmission times for request packets, ACKs, and TCP connection establishment is negligible. 
6. We assume a very large threshold in the TCP congestion control mechanism which is never attained

We also introduce the following notation:

1. The size of the object to be transferred is $O\ bits$.
2. The MSS (maximum size segment) is $S\ bits$.
3. The transmission rate of the link from the server to the client is $R\ bps$.
4. The round-trip time is denoted by $RTT$. The round trip time, here, is defined as the time elapsed for a small packet to travel from client to server and then back to the client, excluding the transmission time of the packet. It includes the two end-to-end propagation delays between the two end systems and the processing times at the two end systems.
5. Let $W$, a positive integer, denote a fixed-size static congestion window. 

The server then sends one segment into the network for each acknowledgment it receives from the client. The server continues to send one segment for each acknowledgment until all of the segments of the object have been sent.

### Case 1 - $WS/R > RTT + S/R$

In this case, the server receives an acknowledgement for the first segment in the first window before the server completes the transmission of the first window. 

Assume that windows size $W=4$ segments. 

The initial handshake requires a total of one $RTT$. After this first RTT, the client sends a request for the object (which is piggybacked onto the third segment in the three-way TCP handshake). After a total of two RTTs, the client begins to receive data from the server. Segments arrive periodically from the server every $\frac{S}{R}$ seconds, and the client acknowledges every segment it receives from the server. Because the server receives the first acknowledgment before it completes sending a window's worth of segments, the server continues to transmit segments after having transmitted the first window's worth of segments. And because the acknowledgments arrive periodically at the server every $\frac{S}{R}$ seconds from the time when the first acknowledgment arrives, the server transmits segments continuously until it has transmitted the entire object. Thus, once the server starts to transmit the object at rate $R$ bps, it continues to transmit the object at rate $R$ until the entire object is transmitted. The latency, therefore, is

$$
2*RTT + \frac{O}{R}.
$$

![](/assets/assignment-6-cidr-latency-1.png)

### Case 2 - $WS/R < RTT + S/R$

In this case, the server transmits the first window's worth of segments before the server receives an acknowledgement for the first segment in the window.

Assume that the windows size $W=2$ segments.

The initial handshake requires a total of one $RTT$. After this first RTT, the client sends a request for the object (which is piggybacked onto the third segment in the three-way TCP handshake). These segments arrive peridodically every $\frac{S}R$ seconds, and the client acknowledges every segment it receives from the server. But now the server completes the transmission of the first window before the first acknowledgment arrives from the client. Therefore, after sending a window, the server must stall and wait for an acknowledgement before resuming transmission. When an acknowledgement finally arrives, the server sends a new segment to the client. Once the first acknowledgement arrives, a window's worth of acknowledgements arrive, with each successive acknowledgement spaced by $\frac{S}R$ seconds. For each of these acknowledgements, the server sends exactly one segment. Thus, the server alternates between two states: a transmitting state, during which it transmits $W$ segments; and a **stalled state**, during which it transmits nothing and waits for an acknowledgement. The latency is equal to $2\*RTT$ plus the time required for the server to transmit the object, i.e., $\frac{O}R$ , plus the amount of time that the server is in the stalled state. To determine the amount of time the server is in the stalled state, let $K$ be the number of windows of data there are in the object of size $`O`$ i.e.,  $K = \frac{O}{WS}$; if $\frac{O} {WS}$ is not an integer, then round $K$ up to the nearest integer. The server is in the stalled state between the transmission of each of the windows, that is, for $K−1$ periods of time, with each period lasting $`RTT − (W−1)*\frac{S}R`$ . Thus, for Case 2,

$$
Latency = 2*RTT + \frac{O}R + (K−1)[\frac{S}R + RTT − \frac{WS}R]
$$

![](/assets/assignment-6-cidr-latency-2.png)

# Modeling Latency: Dynamic Congestion Window

The server first starts with a congestion window of one segment and sends one segment to the client. When it receives an acknowledgement for the segment, it increases its congestion window to two segments and sends two segments to the client (spaced apart by S/R seconds). As it receives the acknowledgements for the two segments, it increases the congestion window to four segments and sends four segments to the client (again spaced apart by S/R seconds). The process continues, with the congestion window doubling every RTT.

Note that O/S is the number of segments in the object; in the above diagram, $\frac{O}S = 15$. Consider the number of segments that are in each of the windows. The first window contains 1 segment; the second window contains 2 segments; the third window contains 4 segments. More generally, the $k^{th}$ window contains $2^{k}-1$ segments. Let K be the number of windows that cover the object; in the preceding diagram K = 4. In general we can express K in terms of O/S as follows:

$$
K = min\{ 2^{k} - 1, \frac{O}{S} \}\\
\Rightarrow K = min\{ k, log_2(\frac{O}{S} + 1) \}\\
\Rightarrow K = \lceil log_2(\frac{O}{S} + 1) \rceil
$$

![](/assets/assignment-6-cidr-latency-3.png)

After transmitting a window's worth of data, the server may stall (i.e., stop transmitting) while it waits for an acknowledgement. In the preceding diagram, the server stalls after transmitting the first and second windows, but not after transmitting the third. Let us now calculate the amount of stall time after transmitting the kth window. The time from when the server begins to transmit the kth window until when the server receives an acknowledgement for the first segment in the window is S/R + RTT. The transmission time of the kth window is (S/R) 2k−1. The stall time is the difference of these two quantities, that is,

$$
[S/R + RTT − 2k−1(S/R)]+
$$
