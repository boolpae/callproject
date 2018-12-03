#ifndef _PACKET_QUEUE_H_
#define _PACKET_QUEUE_H_

#include <stdint.h>
#include <queue>

class PacketItem {
    public:
    PacketItem(int len, uint8_t *data);
    virtual ~PacketItem();

    int getLen() { return m_len; }
    uint8_t* getData() { return m_data; }

    size_t m_hash;
    
    private:
    int m_len;
    uint8_t *m_data;
};

typedef std::queue< PacketItem* > PACKET_QUEUE;

class PacketQueue {
    public:
        PacketQueue();
        virtual ~PacketQueue();

        bool push(int len, uint8_t *data);
        PacketItem* pop();
    private:
        PACKET_QUEUE m_que;
};

extern PacketQueue gclsPacketQueue;

#endif // _PACKET_QUEUE_H_