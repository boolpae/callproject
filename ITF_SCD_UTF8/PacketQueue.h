#ifndef _PACKET_QUEUE_H_
#define _PACKET_QUEUE_H_

#include "PacketHeader.h"

#include <string>
#include <queue>

#include <stdint.h>
#include <pcap.h>

class PacketItem {
    public:
    PacketItem(pcap_t *psttPcap, struct pcap_pkthdr *psttHeader, uint8_t *data);
    virtual ~PacketItem();
    static void Init(PacketItem *item);

    // size_t m_hash;
    std::string m_mapKey;

    int m_len;
    uint8_t *m_data;

	int			m_iIpHeaderLen, m_iUdpBodyLen, m_iIpPos;
	char		* m_pszUdpBody;
	Ip4Header		* m_psttIp4Header;	// IPv4 Header
	UdpHeader		* m_psttUdpHeader;	// UDP Header

    pcap_t	* m_psttPcap;
    struct pcap_pkthdr m_sttHeader;

    private:
    // static void GetKey( const char * pszIp, int iPort, std::string & strKey );
	// static void GetKey( uint32_t iIp, uint16_t sPort, std::string & strKey );

};

typedef std::queue< PacketItem* > PACKET_QUEUE;

class PacketQueue {
    public:
        PacketQueue();
        virtual ~PacketQueue();

        bool push(pcap_t *psttPcap, struct pcap_pkthdr *psttHeader, uint8_t *data);
        bool push(PacketItem *item);
        PacketItem* pop();

        int getSize() { return m_que.size(); }
    private:
        PACKET_QUEUE m_que;
};

extern PacketQueue gclsPacketQueue;
extern PacketQueue gclsSignalPacketQueue;
extern PacketQueue gclsVoicePacketQueue;

#endif // _PACKET_QUEUE_H_