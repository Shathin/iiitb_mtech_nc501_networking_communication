# Congestion Control

> [Source](https://www.net.t-labs.tu-berlin.de/teaching/computer_networking/03.07.htm)

## Introduction

The congestion window, denoted as $`CongWin`$ , imposes a constraint on how much traffic a host can send into a connection. Specifically, the amount of unacknowledged data that a host can have within a TCP connection may not exceed the minimum of $`CongWin`$ and receiver window size $`RcvWin`$, i.e.,

$$
LastByteSent − LastByteAcked ≤ min\{CongWin, RcvWin\}.
$$

For the sake of simplicity, let us assume that the size of the receiver window is infinite i.e., it is as large as possible. Hence, the above equation now solely depends on the congestion window $CongWin$. 

Once a TCP connection is established between the two end systems, the application process at the sender writes bytes to the sender's TCP send buffer. TCP grabs chunks of size Maximum Segment Size (MSS), encapsulates each chunk within a TCP segment, and passes the segments to the network layer for transmission across the network. The TCP congestion window regulates the times at which the segments are sent into the network (i.e., passed to the network layer).

## Tahoe

### Slow Start Phase

Initially, the congestion window is equal to one MSS. TCP sends the first segment into the network and waits for an acknowledgment. If this segment is acknowledged before its timer times out, the sender increases the congestion window by one MSS and sends out two maximum-size segments. If these segments are acknowledged before their timeouts, the sender increases the congestion window by one MSS for each of the acknowledged segments, giving a congestion window of four MSS, and sends out four maximum-sized segments. This procedure continues as long as 

1. the congestion window is below the threshold and
2. the acknowledgments arrive before their corresponding timeouts.

This phase of the algorithm is called a **slow start** because it begins with a small congestion window equal to one MSS. The transmission rate of the connection starts slowly but accelerates rapidly.

![](/assets/assignment-4-http-cc-1.png)

### Congestion Avoidance

The slow start phase ends when the window size exceeds the value of *the threshold*. Once the congestion window is larger than the current value of *the threshold*, the congestion window grows linearly rather than exponentially. This has the effect of increasing the congestion window by one in each RTT for which an entire window's worth of acknowledgements arrives. This phase of the algorithm is called **congestion avoidance**.

## Handling congestion

When a timeout occurs, which is a symptom of congestion, the value of the *threshold* is set to half the value of the current congestion window, and the congestion window is reset to one MSS. The sender then again grows the congestion window exponentially fast using the slow-start procedure until the congestion window hits the threshold.

This algorithm is often referred to as an **additive-increase, multiplicative-decrease (AIMD)** algorithm, or Tahoe algorithm.

![](/assets/assignment-4-http-cc-2.png)

# Reno

### Fast Retransmit

Duplicate acknowledgment is the basis for the fast retransmit mechanism. After receiving a packet, an acknowledgment is sent for the last in-order byte of the data received. ~~For an in-order packet, the ACK is effectively the last packet's sequence number plus the current packet's payload length.~~ If the next packet in the sequence is lost, but a third packet in the sequence is received, then the receiver can only acknowledge the last in-order byte of data, which is the same value as was acknowledged for the first packet. The second packet is lost, and the third packet is not in order, so the last in-order byte of data remains the same as before. Thus a Duplicate acknowledgment occurs. The sender continues to send packets and the receiver receives the fourth and fifth packets. Again, the second packet is missing from the sequence, so the last in-order byte has not changed. Duplicate acknowledgments are sent for both of these packets.

When a sender receives three duplicate acknowledgments, it can be reasonably confident that a segment ~~carrying the data that followed the last in-order byte specified in the acknowledgment~~ was lost. A sender with a fast retransmit will then retransmit this packet immediately without waiting for its timeout. On receipt of the retransmitted segment, the receiver can acknowledge the last in-order byte of data received. In the above example, this would acknowledge the end of the payload of the fifth packet. There is no need to acknowledge intermediate packets since TCP uses cumulative acknowledgments by default.

## Reno's algorithm

One problem with the Tahoe algorithm is that when a segment is lost the sender side of the application may have to wait a long period of time for the timeout. For this reason, a variant of Tahoe, called Reno, is implemented by most operating systems. Like Tahoe's algorithm, Reno sets its congestion window to one segment upon the expiration of a timer. However, Reno also includes the fast retransmit mechanism that was discussed above based on which it employs a fast recovery mechanism, which essentially cancels the slow start phase after fast retransmission i.e., it restarts from the Congestion avoidance phase hence saving time. Most TCP implementations currently use the Reno algorithm.

> TODO : Insert related diagram

---

# CUBIC

## Long Fat Networks

In data communications, the bandwidth-delay product is the product of a data link's capacity (in bits per second) and its round-trip delay time (in seconds) i.e., $BW*RTT$. This represents the maximum amount of data on the network circuit at any given time, i.e., data that has been transmitted but not yet acknowledged.

A network with a large bandwidth-delay product is commonly known as a long fat network (shortened to LFN). As defined in RFC 1072, a network is considered an LFN if its bandwidth-delay product is significantly larger than $10^5$ bits (12,500 bytes). 

A high bandwidth-delay product is an important problem case in the design of protocols such as Transmission Control Protocol (TCP) in respect of TCP tuning, because the protocol can only achieve optimum throughput if a sender sends a sufficiently large quantity of data before being required to stop and wait until a confirming message is received from the receiver, acknowledging successful receipt of that data. If the quantity of data sent is insufficient compared with the bandwidth-delay product, then the link is not being kept busy and the protocol is operating below peak efficiency for the link.

## BIC

BIC TCP (Binary Increase Congestion control) is one of the congestion control algorithms that can be used for the Transmission Control Protocol (TCP). BIC is optimized for high-speed networks with high latency: so-called "long fat networks".

BIC implements a unique congestion window ($cwnd$) algorithm. This algorithm tries to find the maximum $cwnd$ by searching in three parts: binary search increase, additive increase, and slow start. When a network failure occurs, the BIC uses a multiplicative decrease in correcting the $cwnd$.

### Binary Search Increase

This views congestion control as a searching problem in which the system gives yes/no feedback through packet loss as to whether the current sending rate is larger than the network capacity. The starting points for this search are the current minimum window size $W_{min}$ and the maximum window size $W_{max}$. Usually $W_{max}$ is the window size just before the last fast recovery (i.e., where the last packet loss occurred), and $W_{min}$ is the window size just after the fast recovery. The algorithm repeatedly computes the midpoint between $W_{min}$ and $W_{max}$ and sets the current window size to the midpoint and checks for feedback in the form of packet losses. Based on this feedback, the midpoint is taken as the new $W_{max}$ if there is a packet loss, and as the new $W_{min}$ if not. The process repeats until the difference between $W_{max}$ and $W_{min}$ falls below a preset threshold, called the minimum increment $S_{min}$.

### Additive Increase

When the distance to the midpoint from the current minimum is too large, increasing the window size directly to that midpoint may add too much stress to the network. When the distance from the current window size to the target in binary search increase is larger than a prescribed maximum step, called the maximum increment $S_{max}$, we increase window size by $S_{max}$ until the distance becomes less than $S_{max}$, at which time window increases directly to the target. This, after a large window reduction, the strategy initially increases the window linearly and then increases logarithmically. This combination of binary search increases and the additive increase is called binary increase.

![](/assets/assignment-4-http-cc-3.png)

## CUBIC

Although BIC achieves pretty good scalability, fairness, and stability during the current high-speed environments, the BIC’s growth function can still be too aggressive for TCP, especially under short RTT or low-speed networks. Furthermore, several different phases of window control add a lot of complexity in analyzing the protocol. 

A variant of BIC, called CUBIC, was introduced, which retains the strengths of BIC (stability and scalability) and simplifies the window control and enhances its TCP friendliness.

The window growth function of CUBIC is a cubic function, whose shape is very similar to the growth function of BIC. More specifically, the congestion window of CUBIC is determined by the following function: 

$$
W_{cubic} = C(t-K)^3+W_{max}
$$

where $C$ is a scaling factor, $t$ is the elapsed time from the last window reduction, $W_{max}$ is the window size just before the last window reduction, and $K= \sqrt[3]{W_{max}*\frac{\beta}{C}}$ , where $\beta$ is a constant multiplication decrease factor applied for window reduction at the time of loss event (i.e., the window reduces to $W_{max}\beta$ at the time of the last reduction).

![](/assets/assignment-4-http-cc-4.png)

Fig. 2 shows the growth function of CUBIC with the origin at W max . The window grows very fast upon a window reduction, but as it gets closer to W max , it slows down its growth. Around W max , the window increment becomes almost zero. Above that, CUBIC starts probing for more bandwidth in which the window grows slowly initially, accelerating its growth as it moves away from W max . This slow growth around W max enhances the stability of the protocol, and increases the utilization of the network while the fast growth away from W max ensures the scalability of the protocol.
