#include "MyThread.h"
#include <QDebug>
#include <iostream>

using namespace std;

// 实现MyThread类
MyThread::MyThread(const char *ip, const int t): ipAddress(ip), ttl(t)
{
}

MyThread::~MyThread()
{
    WSACleanup();
}

// Calculate the 16-bit checksum of an ICMPHeader
uint16_t MyThread::calculateChecksum(const uint16_t *pBuffer, int bufferSize)
{
    uint32_t sum = 0;

    // Calculate the sum of all 16-bit words
    while (bufferSize > 1)
    {
        sum += *pBuffer++;
        bufferSize -= sizeof(uint16_t);
    }

    // If the bufferSize is odd, add the last byte (padded with 0s)
    if (bufferSize)
    {
        sum += static_cast<uint16_t>(*reinterpret_cast<const uint8_t *>(pBuffer));
    }

    // Fold 32-bit sum into 16 bits (add high 16 bits to low 16 bits)
    while (sum >> 16)
    {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    // One's complement the result and return the 16-bit checksum
    return static_cast<uint16_t>(~sum);
}

// Parse IP Packet to Result
bool MyThread::parseIpPacket(const char *ipPacketBuffer, const int ipPacketBufferSize, IP_PACKET_PARSE_RESULT &parseResult)
{
    // 校验IP数据包大小是否合法
    if (ipPacketBufferSize < 0)
        return false;
    if (ipPacketBufferSize < sizeof(IP_HEADER) + sizeof(ICMP_HEADER))
        return false;
    const IP_HEADER *ipHeader = reinterpret_cast<const IP_HEADER *>(ipPacketBuffer);
    const int ipHeaderLength = ipHeader->headerLength * 4;
    if (ipPacketBufferSize < ipHeaderLength + sizeof(ICMP_HEADER))
        return false;

    // 解析ICMP报文类型
    const ICMP_HEADER *icmpHeader = reinterpret_cast<const ICMP_HEADER *>(ipPacketBuffer + ipHeaderLength);
    uint16_t identifier = 0, sequenceNumber = 0; // 记录报文ID、报文序列号
    // 如果是ICMP_ECHO_REPLY类型
    if (icmpHeader->type == ICMP_ECHO_REPLY)
    {
        identifier = icmpHeader->identifier;         // 报文ID
        sequenceNumber = icmpHeader->sequenceNumber; // 报文序列号
    }
    // 如果是ICMP_TIMEOUT类型
    else if (icmpHeader->type == ICMP_TIMEOUT)
    {
        // 解析ICMP数据载荷里包含的 IP Packet
        const IP_HEADER *innerIpHeader = reinterpret_cast<const IP_HEADER *>(icmpHeader + sizeof(ICMP_HEADER));          // 载荷中的IP头
        const int innerIpHeaderLength = innerIpHeader->headerLength * 4;                                                 // 载荷中的IP头长度
        const ICMP_HEADER *innerIcmpHeader = reinterpret_cast<const ICMP_HEADER *>(innerIpHeader + innerIpHeaderLength); // 载荷中的ICMP头
        identifier = innerIcmpHeader->identifier;                                                                        // 报文ID
        sequenceNumber = innerIcmpHeader->sequenceNumber;                                                                // 报文序列号
    }
    // 如果是无效的ICMP_TYPE类型
    else
        return false;

    // 校验收到的ICMP数据包的报文ID和报文序列号，是否与发出去的一致
    // if (identifier != static_cast<const uint16_t>(GetCurrentProcessId()) || sequenceNumber != parseResult.sequenceNumber)
    //     return false;

    // 计算往返时间
    parseResult.roundTripTime = GetTickCount() - parseResult.roundTripTime;
    // 记录IP数据包的源地址
    parseResult.ipAddress.s_addr = ipHeader->sourceIPAddress;
    // 记录ICMP_ECHO_Type响应类型
    parseResult.icmpType = icmpHeader->type;
    parseResult.timeToLive = ipHeader->timeToLive;
    parseResult.isOk = true;
    return true;
}

// Send ICMP Packet and Receive
MyThread::IP_PACKET_PARSE_RESULT MyThread::IcmpSenderReceiver(const uint32_t destIpAddr, const int timeout, const int ttl)
{
    SOCKET sockRaw = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, NULL, 0, WSA_FLAG_OVERLAPPED);               // 创建原始套接字
    setsockopt(sockRaw, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char *>(&timeout), sizeof(timeout)); // 设置接收超时
    setsockopt(sockRaw, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char *>(&timeout), sizeof(timeout)); // 设置发送超时
    setsockopt(sockRaw, IPPROTO_IP, IP_TTL, reinterpret_cast<const char *>(&ttl), sizeof(ttl));              // 设置TTL

    // 初始化发送缓冲区
    char senderBuffer[sizeof(ICMP_HEADER) + 32]; // ICMP数据包内含有32字节数据
    memset(senderBuffer, 0, sizeof(senderBuffer));

    // 充填ICMP Header信息
    uint16_t seq = 6; // ICMP报文序列号
    ICMP_HEADER *icmpHeader = reinterpret_cast<ICMP_HEADER *>(senderBuffer);
    icmpHeader->type = ICMP_ECHO_REQUEST;                                        // 发送 ECHO Request 回显请求类型的ICMP数据包
    icmpHeader->code = 0;                                                        // 回显请求情况下code字段是0
    icmpHeader->identifier = static_cast<const uint16_t>(GetCurrentProcessId()); // 取进程Id作为ICMP报文ID
    icmpHeader->sequenceNumber = htons(seq);
    memset(senderBuffer + sizeof(ICMP_HEADER), 'F', 32); // 充填ICMP所携带的数据
    icmpHeader->checksum = calculateChecksum(reinterpret_cast<const uint16_t *>(senderBuffer), sizeof(senderBuffer));

    // 初始化目标地址结构
    sockaddr_in destSockAddr;
    memset(&destSockAddr, 0, sizeof(destSockAddr));
    destSockAddr.sin_family = AF_INET;         // AF_INET是使用IPv4协议族
    destSockAddr.sin_addr.s_addr = destIpAddr; // 目标地址

    // 初始化IP数据包解析结构
    IP_PACKET_PARSE_RESULT parseResult;
    memset(&parseResult, 0, sizeof(parseResult));
    parseResult.sequenceNumber = seq;
    parseResult.roundTripTime = GetTickCount(); // 记录开始发送的时间

    // 发送
    sendto(sockRaw, senderBuffer, sizeof(senderBuffer), 0,
           reinterpret_cast<sockaddr *>(&destSockAddr), sizeof(destSockAddr));

    // 接收
    sockaddr_in fromAddr; // 初始化发送方地址结构
    int fromAddrLen = sizeof(fromAddr);
    char recvBuffer[1024]; // 准备接收缓冲区
    memset(recvBuffer, 0, sizeof(recvBuffer));
    int recvDatarLen = recvfrom(sockRaw, reinterpret_cast<char *>(recvBuffer), sizeof(recvBuffer),
                                0, (sockaddr *)&fromAddr, &fromAddrLen);
    if (recvDatarLen == SOCKET_ERROR)
    {
        cout << "TTL=" << ttl << " Timeout." << endl;
        return parseResult;
    }

    bool isOk = parseIpPacket(recvBuffer, recvDatarLen, parseResult);
    if (!isOk)
    {
        parseResult.isOk = false;
        cout << "ip packet parse error" << endl;
        return parseResult;
    }
    else
    {
        parseResult.isOk = true;
        cout << "TTL=" << ttl << " From  " << inet_ntoa(parseResult.ipAddress) << "  Reply 32 Bytes. RTT=" << parseResult.roundTripTime
             << "ms. TTL=" << (int)parseResult.timeToLive << endl;
    }
    return parseResult;
}

void MyThread::run()
{
    // 初始化Windows sockets 网络环境
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    // 将字符串转换为IP地址
    unsigned long destIPAddr = inet_addr(this->ipAddress);
    // 转换不成功时尝试域名解析
    if (destIPAddr == INADDR_NONE)
    {
        hostent *pHostent = gethostbyname(this->ipAddress);
        if (pHostent)
        {
            destIPAddr = ((in_addr *)pHostent->h_addr)->s_addr;
        }
        else
        {
            cout << "The entered IP address or domain name is invalid." << endl;
            WSACleanup();
            return;
        }
    }

    int timeout = 1000;
    int ttl = this->ttl;

    // 循环发送ICMP,直到ttl增大到30,最大有30跳
    while (ttl <= 30)
    {
        // 根据参数发送ICMP数据包,并返回解析好的结果
        IP_PACKET_PARSE_RESULT parseResult = IcmpSenderReceiver(destIPAddr, timeout, ttl);
        // 每次都发送信号回UI
        if (parseResult.isOk)
        {
            // 发送信号给Widget组件用于展示数据
            QList<QString> list;
            list.append(QString::number(ttl)); // TTL
            list.append(QString(inet_ntoa(parseResult.ipAddress))); //IP地址
            list.append(QString::number(parseResult.roundTripTime)); // RTT
            qRegisterMetaType<QList<QString>>();
            emit signal_myThread_ok(list);
            // 如果最终到目的IP了,就跳出循环
            if (parseResult.ipAddress.S_un.S_addr == destIPAddr)
            {
                cout << "Trace Route Finish!" << endl;
                break;
            }
        } else {
            // 即使没有追踪到某一个跃点的响应，也要发送信号给Widget组件用于展示"*"
            QList<QString> list;
            list.append(QString::number(ttl)); // TTL
            list.append(QString("*")); //IP地址
            list.append(QString("*")); // RTT
            qRegisterMetaType<QList<QString>>();
            emit signal_myThread_ok(list);
        }
        ttl++;
    }
    WSACleanup();
}
