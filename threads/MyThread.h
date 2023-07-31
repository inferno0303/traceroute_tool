#include <QThread>
#include <QString>

// 发送ICMP类型的IP数据包,需要用到的 Windows sock 库
#include <cstdint>    // Include this header for uint8_t, uint16_t, uint32_t types
#include <winsock2.h> // Windows下的套接字编程头文件
#include <ws2tcpip.h> // Windows下的套接字高级编程头文件，在这里是用于设置sock的TTL


class MyThread : public QThread
{
    Q_OBJECT

public:
    // 构造函数第一个参数是 const char * 类型的字符串,表示ip地址或域名,赋值给成员变量 ipAddress
    // 第二个参数是 const int 类型的 ttl 最大生存时间,赋值给成员变量 ttl
    MyThread(const char * = nullptr, const int = 1);
    ~MyThread();

private:
    const char *ipAddress = nullptr;
    const int ttl = 1;

    // IP Header
    struct IP_HEADER
    {
        uint8_t headerLength : 4;      // 4-bit header length
        uint8_t version : 4;           // 4-bit version number
        uint8_t serviceType;           // 8-bit service type
        uint16_t totalLength;          // 16-bit total length
        uint16_t identifier;           // 16-bit identifier
        uint16_t fragmentAndFlags;     // 3-bit flags + 13-bit fragment offset
        uint8_t timeToLive;            // 8-bit time to live
        uint8_t upperLayerProtocol;    // 8-bit upper-layer protocol number
        uint16_t checksum;             // 16-bit checksum
        uint32_t sourceIPAddress;      // 32-bit source IP address
        uint32_t destinationIPAddress; // 32-bit destination IP address
    };

    // ICMP Header
    struct ICMP_HEADER
    {
        uint8_t type;            // 8-bit type field
        uint8_t code;            // 8-bit code field
        uint16_t checksum;       // 16-bit checksum
        uint16_t identifier;     // 16-bit identifier
        uint16_t sequenceNumber; // 16-bit sequence number
    };

    // const for ICMP header type
    const uint8_t ICMP_ECHO_REQUEST = 8; // 请求回显
    const uint8_t ICMP_ECHO_REPLY = 0;   // 回显应答
    const uint8_t ICMP_TIMEOUT = 11;     // 传输超时

    // IP Packet Parse Result
    struct IP_PACKET_PARSE_RESULT
    {
        bool isOk;
        in_addr ipAddress;       // IP address of the reply packet
        uint32_t roundTripTime;  // Round-trip time
        uint8_t timeToLive;      // Current IP data packet's TTL
        uint8_t icmpType;        // ICMP Header Type
        uint16_t sequenceNumber; // Sequence number
    };

    // Calculate the 16-bit checksum of an ICMPHeader
    uint16_t calculateChecksum(const uint16_t *pBuffer, int bufferSize);

    // Parse IP Packet to Result
    bool parseIpPacket(const char *ipPacketBuffer, const int ipPacketBufferSize, IP_PACKET_PARSE_RESULT &parseResult);

    // Send ICMP Packet and Receive
    IP_PACKET_PARSE_RESULT IcmpSenderReceiver(const uint32_t destIpAddr, const int timeout, const int ttl);

    void run() override;

signals:
    void signal_myThread_ok(QList<QString>);
};

// 在全局范围内注册自定义数据类型
Q_DECLARE_METATYPE(QList<QString>)

// end MYTHREAD_H
