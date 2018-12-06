#include "PacketQueue.h"
#include "Log.h"

#include <iostream>
#include <functional>

#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Main Queue
PacketQueue gclsPacketQueue;

// SignalQueue
PacketQueue gclsSignalPacketQueue;

// VoiceQueue
PacketQueue gclsVoicePacketQueue;


void GenKey( uint32_t iIp, uint16_t sPort, std::string & strKey )
{
	char	szKey[21];

	snprintf( szKey, sizeof(szKey), "%x:%x", iIp, sPort );
	strKey = szKey;
}

void GenKey( const char * pszIp, int iPort, std::string & strKey )
{
	uint32_t iIp = inet_addr( pszIp );
	uint16_t sPort = htons( iPort );

	GenKey( iIp, sPort, strKey );
}


PacketItem::PacketItem(pcap_t *psttPcap, struct pcap_pkthdr *psttHeader, uint8_t *data)
{
    // std::hash< PacketItem* > hash_fn;

    m_data = new uint8_t[psttHeader->caplen];

    if (!m_data) {
        m_data = nullptr;
        m_len = 0;
        return;
    }
    
    m_psttPcap = psttPcap;
    memset((void *)&m_sttHeader, 0, sizeof(struct pcap_pkthdr));
    memcpy((void *)&m_sttHeader, (const void*)psttHeader, sizeof(struct pcap_pkthdr));

    m_len = m_sttHeader.caplen;

    memcpy(m_data, data, m_len);
    // m_hash = hash_fn( this );

    // std::cout << "[DEBUG] maked item(len:" << m_len <<")" << std::endl;
	CLog::Print( LOG_DEBUG, "maked item(len:%d)", m_len );
}

PacketItem::~PacketItem()
{
    // std::cout << "[DEBUG] erased item(len:" << m_len << ")" << std::endl;
	CLog::Print( LOG_DEBUG, "erased item(len:%d)", m_len );
    delete m_data;
    m_data = nullptr;
}

void PacketItem::Init(PacketItem *item)
{
    if( item->m_data[12] == 0x81 )
    {
        item->m_iIpPos = 18;		// VLAN
    }
    else if( item->m_data[12] == 0x08 )
    {
        item->m_iIpPos = 14;		// IP
    }

    item->m_psttIp4Header = ( Ip4Header * )( item->m_data + item->m_iIpPos );
    item->m_iIpHeaderLen = GetIpHeaderLength( item->m_psttIp4Header );
    item->m_psttUdpHeader = ( UdpHeader * )( item->m_data + item->m_iIpPos + item->m_iIpHeaderLen );
    item->m_pszUdpBody = ( char * )( item->m_data + item->m_iIpPos + item->m_iIpHeaderLen + 8 );
    item->m_iUdpBodyLen = item->m_len - ( item->m_iIpPos + item->m_iIpHeaderLen + 8 );

	GenKey( item->m_psttIp4Header->daddr, item->m_psttUdpHeader->dport, item->m_mapKey );
}

PacketQueue::PacketQueue()
{

}

PacketQueue::~PacketQueue()
{
    PACKET_QUEUE empty;
    PacketItem *item;

    while ( item = m_que.front() )
    {
        m_que.pop();
        delete item;
        item = nullptr;
    }

    m_que.swap(empty);
}

bool PacketQueue::push(pcap_t *psttPcap, struct pcap_pkthdr *psttHeader, uint8_t *data)
{
    PacketItem *item;

    item = new PacketItem(psttPcap, psttHeader, data);
    if (!item || !item->m_len) {
        if (item && !item->m_len) delete item;
        return false;
    }

    m_que.push( item );

    // std::cout << "[DEBUG] pushed item len(" << item->m_len << ") queue size(" << m_que.size() << ")" << std::endl;
	CLog::Print( LOG_DEBUG, "pushed item len(%d) queue size(%d)", item->m_len, m_que.size() );

    return true;
}

bool PacketQueue::push(PacketItem *item)
{
    if (!item) return false;

    m_que.push( item );

    // std::cout << "[DEBUG] pushed item len(" << item->m_len << ") queue size(" << m_que.size() << ")" << std::endl;
	CLog::Print( LOG_DEBUG, "pushed packet-item len(%d) queue size(%d)", item->m_len, m_que.size() );

    return true;
}

PacketItem* PacketQueue::pop()
{
    PacketItem *item = nullptr;

    if ( m_que.empty() ) return item;

    item = m_que.front();

    // std::cout << "[DEBUG] poped item len(" << item->m_len << ") queue size(" << m_que.size() << ")" << std::endl;
	CLog::Print( LOG_DEBUG, "poped item len(%d) queue size(%d)", item->m_len, m_que.size() );

    m_que.pop();

    return item;
}
