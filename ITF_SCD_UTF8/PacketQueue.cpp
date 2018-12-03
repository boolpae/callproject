#include "PacketQueue.h"
#include "Log.h"

#include <iostream>
#include <functional>

#include <string.h>


PacketQueue gclsPacketQueue;

PacketItem::PacketItem(int len, uint8_t *data)
: m_len(len), m_data(nullptr)
{
    std::hash< PacketItem* > hash_fn;

    m_data = new uint8_t[len];
    memcpy(m_data, data, len);
    m_hash = hash_fn( this );

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

PacketQueue::PacketQueue()
{

}

PacketQueue::~PacketQueue()
{
    PACKET_QUEUE empty;
    m_que.swap(empty);
}

bool PacketQueue::push(int len, uint8_t *data)
{
    PacketItem *item;

    item = new PacketItem(len, data);
    if (!item) return false;

    m_que.push( item );

    // std::cout << "[DEBUG] pushed item len(" << item->getLen() << ") queue size(" << m_que.size() << ")" << std::endl;
	CLog::Print( LOG_DEBUG, "pushed item len(%d) queue size(%d)", item->getLen(), m_que.size() );

    return true;
}

PacketItem* PacketQueue::pop()
{
    PacketItem *item = nullptr;

    if ( m_que.empty() ) return item;

    item = m_que.front();

    // std::cout << "[DEBUG] poped item len(" << item->getLen() << ") queue size(" << m_que.size() << ")" << std::endl;
	CLog::Print( LOG_DEBUG, "poped item len(%d) queue size(%d)", item->getLen(), m_que.size() );

    m_que.pop();

    return item;
}
