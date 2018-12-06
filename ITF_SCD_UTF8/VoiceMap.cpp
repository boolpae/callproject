
#include "VoiceMap.h"
#include "RtpMap.h"


VoiceMap gclsVoiceMap;

VoiceMap::VoiceMap()
{

}

VoiceMap::~VoiceMap()
{

}

PacketQueue *VoiceMap::createQueue( std::string &strCallId )
{
    PacketQueue *que = nullptr;
	VOICE_MAP::iterator itMap;

	itMap = m_clsMap.find( strCallId );
	if( itMap == m_clsMap.end() )
    {
        que = new PacketQueue();
        if ( !que )
        {
            // PacketQueue 인스턴스 생성 실패
            return nullptr;
        }
        m_clsMap.insert( VOICE_MAP::value_type( strCallId, que ) );
    }
    else
    {
        que = itMap->second;
    }
    return que;
}

void VoiceMap::deleteQueue( std::string &strCallId )
{
	VOICE_MAP::iterator itMap;

	itMap = m_clsMap.find( strCallId );
	if( itMap != m_clsMap.end() )
    {
        delete itMap->second;
        m_clsMap.erase( itMap );
    }
}

void VoiceMap::insert(std::string strCallId, PacketItem *item)
{
    VOICE_MAP::iterator itMap;
    // Call-ID 값을 m_clsMap에서 Call-ID가 있는지 찾음
    // Call-ID가 있다면 해당 Call-ID와 매치되는 que에 item을 push
    // Call-ID가 없다면 item 폐기
    itMap = m_clsMap.find( strCallId );
    if ( itMap != m_clsMap.end() )
    {
        itMap->second->push( item );
    }
    else
    {
        // Push할 큐가 없으므로 item을 폐기한다.
        delete item;
    }
}