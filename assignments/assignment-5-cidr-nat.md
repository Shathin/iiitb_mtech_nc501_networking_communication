# CIDR

> Why we need CIDR concept in case of IPv4 addressing? Explain CIDR.

> [Source 1](https://www.keycdn.com/support/what-is-cidr#how-does-cidr-work), [Source 2](https://en.wikipedia.org/wiki/Classless_Inter-Domain_Routing)

**Classless Inter-Domain Routing (CIDR)** is a method for allocating IP addresses. The **Internet Engineering Task Force** introduced CIDR in 1993 to replace the previous classful network addressing architecture on the Internet. The goal was to slow the growth of routing tables on routers across the Internet and to help slow the rapid exhaustion of IPv4 addresses.

## Problems with class-based IP addressing

IP addresses are described as consisting of two groups of bits in the address: the most significant bits are the network prefix, which identifies a whole network or subnet, and the least significant set forms the host identifier, which specifies a particular host on that network. This division is used as the basis of traffic routing between IP networks and for address allocation policies.

Classful network design for IPv4 sized the network prefix as one or more 8-bit groups, resulting in the blocks of Class A, B, or C addresses. 

- **Class A** - Over 16 million host identifiers
- **Class B** - 65,535 host identifiers
- **Class C** - 254 host identifiers

The old method of IP addressing came with inefficiencies that exhausted the availability of IPv4 addresses faster than it needed to. The problem would commonly occur when an organization required more than 254 host machines and therefore would no longer fall into class C but rather class B. This means that the organization would use a class B license even though they had far less than 65,535 hosts. Therefore if an organization only required 2,500 hosts, they would be wasting about 63,000 hosts by holding a class B license which would greatly decrease the availability of IPv4 addresses unnecessarily.

## How does CIDR work?

CIDR is based on [variable-length subnet masking](http://searchnetworking.techtarget.com/definition/variable-length-subnet-mask) (VLSM). This allows it to define prefixes of arbitrary lengths making it much more efficient than the old system. CIDR IP addresses are composed of two sets of numbers. The first part is similar to how we write IP addresses in the classful representation i.e., `a.b.c.d`. The second part, which is prefixed by a forward-slash `/`, defines how many bits of the IP address is part of the network prefix. So, together they're written as `a.b.c.d / n` where `n` is the number of bits used as the network prefix. 

For example, if the IP address is `192.168.0.15` and the second part is the suffix which indicates how many bits are in the entire address (e.g. `/24`). Putting it together, a CIDR IP address would look like the following - `192.168.0.15/24`. Here, we can say that the first $24$ bits are the network part of the address while the last 8 bits are for host addresses.

## Example on CIDR

An Internet Service Provider (ISP) is granted a block of addresses starting with `162.72.0.0/16`. The ISP needs to distribute these addresses to a group that has $128$ customers & each needs $256$ addresses.

- Given address is `162.72.0.0/16`
- 128 customers each requiring 256 addresses
    - Each customer needs 256 addresses  Number of bits required for the host is  $⌈ log2(256) ⌉ = 8$ bits Number of network bits is 32-8 = 24 bits
    - Network range for each customer:
        - 1st customer: $162.72.0.0/24 \rightarrow 162.72.0.255/24$
        - 2nd customer: $162.72.1.0/24 \rightarrow  162.72.1.255/24$
            
            ...
            
        - 128th customer: $162.72.127.0/24\rightarrow  162.72.127.255/24$
    - Total number of addresses used by group 1 is $128*256 = 32768$

---

# NAT

> Explain NAT IP addressing scheme. How NAT works, explain with a figure.
> 

> [Source 1](https://computer.howstuffworks.com/nat.htm), [Source 2](https://www.geeksforgeeks.org/network-address-translation-nat/)
> 

The Internet has grown larger than anyone ever imagined it could be. The current estimate of the size is 100 million - 350 million active users and this will keep increasing, effectively doubling in size each year.

For a computer to communicate with other computers and Web servers on the Internet, it must have an IP address. An IPv4 is a unique 32-bit number that identifies the location of your computer on a network. When IP addressing first came out, everyone thought that there were plenty of addresses to cover any need. Theoretically, you could have $2^{32}$ unique addresses. The actual number of available addresses is smaller (somewhere between 3.2 billion - 3.3 billion) because of the way that the addresses are separated into classes, and because some addresses are set aside for multicasting, testing, or other special uses.

With the explosion of the Internet and the increase in [home networks](https://computer.howstuffworks.com/home-network.htm) and business networks, the number of available IP addresses is simply not enough. One solution to this is to have a larger number that identifies a device. That's precisely what IPv6 did by using a 128 bit IP address. But the implementation of IPv6 will take several years because it requires modification of the entire infrastructure of the Internet.

 This is where NAT comes to the rescue. Network Address Translation allows a single device, such as a [router](https://computer.howstuffworks.com/router.htm), to act as an agent between the Internet (or "public network") and a local (or "private") network. This means that only a single, unique IP address is required to represent an entire group of computers. Developed by Cisco, Network Address Translation is used by a device (firewall, router, or computer that sits between an internal network and the rest of the world.

A few terms being used - 

- **Inside local address –** An IP address that is assigned to a host on the Inside (local) network. These are generally private IP addresses. This is the inside host seen from the inside network.
- **Inside global address –** IP address that represents one or more inside local IP addresses to the outside world. This is the inside host as seen from the outside network.
- **Outside global address –** This is the outside host as seen from the outside network.

**Network Address Translation (NAT) working –** Generally, the border router is configured for NAT i.e the router which has one interface in the local (inside) network and one interface in the global (outside) network. When a packet traverse outside the local (inside) network, then NAT converts that local (private) IP address to a global (public) IP address. When a packet enters the local network, the global (public) IP address is converted to a local (private) IP address. This is done using a NAT table that stores the mapping of the inside local IP address to the outside global IP address.

If NAT runs out of addresses, i.e., no address is left in the pool configured then the packets will be dropped and an Internet Control Message Protocol (ICMP) host unreachable packet to the destination is sent.

In most cases, port numbers are also changed during translation. This is to avoid ambiguity in translation when two hosts in the local network try to communicate with the same outside global address. Hence, using different port numbers for communication between the router (having the inside global IP address) and the end host (having the outside global IP address) helps resolve the ambiguity. This is known as port address translation.

Note that NAT might be employed on the destination host as well and this will completely invisible to the source host.

![](/assets/assignment-5-cidr-nat-1.png)

**Network Address Translation (NAT) Types –** There are 3 ways to configure NAT:

1. **Static NAT –** In this, a single unregistered (Private) IP address is mapped with a legally registered (Public) IP address i.e one-to-one mapping between local and global addresses. This is generally used for Web hosting. 
2. **Dynamic NAT –** In this type of NAT, an unregistered IP address is translated into a registered (Public) IP address from a pool of public IP addresses. If the IP address of the pool is not free, then the packet will be dropped as only a fixed number of private IP addresses can be translated to public addresses. 
3. **Port Address Translation (PAT) –** This is also known as NAT overload. In this, many local (private) IP addresses can be translated to a single registered IP address. Port numbers are used to distinguish the traffic i.e., which traffic belongs to which IP address. This is most frequently used as it is cost-effective as thousands of users can be connected to the Internet by using only one real global (public) IP address.

**Advantages of NAT –**

- NAT conserves legally registered IP addresses .
- It provides privacy as the device IP address, sending and receiving the traffic, will be hidden.
- Eliminates address renumbering when a network evolves.

**Disadvantage of NAT –**

- Translation results in switching path delays.
- Certain applications will not function while NAT is enabled.
- Complicates tunneling protocols such as IPsec.
- Also, router being a network layer device, should not tamper with port numbers(transport layer) but it has to do so because of NAT.
