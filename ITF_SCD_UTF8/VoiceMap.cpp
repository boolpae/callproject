
#include "VoiceMap.h"
#include "RtpMap.h"


VoiceMap::VoiceMap()
{

}

VoiceMap::~VoiceMap()
{

}

void VoiceMap::insert(PacketItem *item)
{
    std::string strCallId;

    // ip, port 값을 이용하여 RTPMAP에서 Call-ID 값을 찾음
    if ( !gclsRtpMap.FindCallId( item->m_mapKey, strCallId ) ) {
        // Call-ID 값을 찾지 못하면 item 폐기
        delete item; item = nullptr;
        return;
    }
    // Call-ID 값을 찾으면 m_clsMap에서 Call-ID가 있는지 찾음
    // Call-ID가 있다면 해당 Call-ID와 매치되는 que에 item을 push
    // Call-ID가 없다면 m_clsMap에 Call-ID(Key)/PacketQueue(Value) 등록 및 que에 item을 push
}