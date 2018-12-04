#ifndef _VOICEMAP_H_
#define _VOICEMAP_H_


#include "PacketQueue.h"

#include <map>


typedef std::map< std::string, PacketQueue* > VOICE_MAP;

class VoiceMap
{
	public:
	VoiceMap();
	virtual ~VoiceMap();

	void insert();

	private:
};


extern VoiceMap gclsVoiceMap;

#endif // _VOICEMAP_H_