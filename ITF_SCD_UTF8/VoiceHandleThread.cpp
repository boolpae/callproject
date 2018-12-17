/* 
 * Copyright (C) 2012 Yee Young Han <websearch@naver.com> (http://blog.naver.com/websearch)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 */

#include "ServerService.h"
#include "Log.h"
#include "PacketHeader.h"
#include "ServerUtility.h"
#include "TimeUtility.h"
#include "PacketQueue.h"
#include "VoiceMap.h"
#include "SipCallDumpSetup.h"
#include "TimeString.h"
#include "SipCallMap.h"
#include "g711.h"

#include <fstream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


// #include "MemoryDebug.h"

#define RTP_HEAD_SIZE 12

#define WAVE_FORMAT_UNKNOWN      0X0000;
#define WAVE_FORMAT_PCM          0X0001;
#define WAVE_FORMAT_MS_ADPCM     0X0002;
#define WAVE_FORMAT_IEEE_FLOAT   0X0003;
#define WAVE_FORMAT_ALAW         0X0006;
#define WAVE_FORMAT_MULAW        0X0007;
#define WAVE_FORMAT_IMA_ADPCM    0X0011;
#define WAVE_FORMAT_YAMAHA_ADPCM 0X0016;
#define WAVE_FORMAT_GSM          0X0031;
#define WAVE_FORMAT_ITU_ADPCM    0X0040;
#define WAVE_FORMAT_MPEG         0X0050;
#define WAVE_FORMAT_EXTENSIBLE   0XFFFE;

typedef struct
{
	unsigned char ChunkID[4];    // Contains the letters "RIFF" in ASCII form
	unsigned int ChunkSize;      // This is the size of the rest of the chunk following this number
	unsigned char Format[4];     // Contains the letters "WAVE" in ASCII form
} RIFF;

//-------------------------------------------
// [Channel]
// - streo     : [left][right]
// - 3 channel : [left][right][center]
// - quad      : [front left][front right][rear left][reat right]
// - 4 channel : [left][center][right][surround]
// - 6 channel : [left center][left][center][right center][right][surround]
//-------------------------------------------
typedef struct
{
	unsigned char  ChunkID[4];    // Contains the letters "fmt " in ASCII form
	unsigned int   ChunkSize;     // 16 for PCM.  This is the size of the rest of the Subchunk which follows this number.
	unsigned short AudioFormat;   // PCM = 1
	unsigned short NumChannels;   // Mono = 1, Stereo = 2, etc.
	unsigned int   SampleRate;    // 8000, 44100, etc.
	unsigned int   AvgByteRate;   // SampleRate * NumChannels * BitsPerSample/8
	unsigned short BlockAlign;    // NumChannels * BitsPerSample/8
	unsigned short BitPerSample;  // 8 bits = 8, 16 bits = 16, etc
} FMT;


typedef struct
{
	char          ChunkID[4];    // Contains the letters "data" in ASCII form
	unsigned int  ChunkSize;     // NumSamples * NumChannels * BitsPerSample/8
} DATA;


typedef struct
{
	RIFF Riff;
	FMT	 Fmt;
	DATA Data;
} WAVE_HEADER;

/**
 * @ingroup SipCallDump
 * @brief VoiceQueue로부터 item을 가져와 처리한다. 이 쓰레드는 Call-ID를 키로 생성되는 쓰레드이다.
 *        큐에서 item을 가져와 decoding 및 파일에 저장한다.
 * @param lpParameter 
 * @returns 0 을 리턴한다.
 */
THREAD_API VoiceHandleThread( LPVOID lpParameter )
{
	PacketItem *item = nullptr;
    PacketQueue *que = nullptr;
	std::string strCallId( (char *)lpParameter );

	que = gclsVoiceMap.createQueue( strCallId );

	if ( !que ) {
		// 큐 생성 실패, 전용 쓰레드 종료
		return nullptr;
	}

    time_t	iTime;
    struct tm	sttTm;
    char szTemp[21];
    uint32_t mask = inet_addr(gclsSetup.m_strCSMask.c_str());

	WAVE_HEADER wHdr;
	uint64_t totalVoiceDataLen[2];

    time( &iTime );

    LocalTime( iTime, sttTm );

    std::string strFileName_r;
    std::string strFileName_l = gclsSetup.m_strVoiceFolder;
    snprintf( szTemp, sizeof(szTemp), "%04d%02d%02d", sttTm.tm_year + 1900, sttTm.tm_mon + 1, sttTm.tm_mday );
    CDirectory::AppendName( strFileName_l, szTemp );
    CDirectory::Create( strFileName_l.c_str() );
    
    snprintf( szTemp, sizeof(szTemp), "%04d%02d%02d_", sttTm.tm_hour, sttTm.tm_min, sttTm.tm_sec );
    CDirectory::AppendName( strFileName_l, szTemp );
    strFileName_l.append( strCallId );
    strFileName_r = strFileName_l;

    strFileName_l.append( "_l.wav" );
    strFileName_r.append( "_r.wav" );

    std::ofstream pcmFile_l;
    std::ofstream pcmFile_r;

	memcpy(&wHdr.Riff.ChunkID, "RIFF", 4);
	wHdr.Riff.ChunkSize = 0;
	memcpy(&wHdr.Riff.Format, "WAVE", 4);

	memcpy(&wHdr.Fmt.ChunkID, "fmt ", 4);
	wHdr.Fmt.ChunkSize = 16;
	wHdr.Fmt.AudioFormat = 1;
	wHdr.Fmt.NumChannels = 1;
	wHdr.Fmt.SampleRate = 8000;
	wHdr.Fmt.AvgByteRate = 8000 * 1 * 16 / 8 ;
	wHdr.Fmt.BlockAlign = 1 * 16 / 8;
	wHdr.Fmt.BitPerSample = 16;

	memcpy(&wHdr.Data.ChunkID, "data", 4);
	wHdr.Data.ChunkSize = 0;

	totalVoiceDataLen[0] = 0;
	totalVoiceDataLen[1] = 0;

    pcmFile_l.open(strFileName_l.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
    if (!pcmFile_l.is_open())
    {
		CLog::Print( LOG_ERROR, "%s(%s) failed to open file(%s)", __FUNCTION__, strCallId.c_str(), strFileName_l.c_str() );
        goto FUNC_END;
    }

    pcmFile_r.open(strFileName_r.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
    if (!pcmFile_r.is_open())
    {
		pcmFile_l.close();
		CLog::Print( LOG_ERROR, "%s(%s) failed to open file(%s)", __FUNCTION__, strCallId.c_str(), strFileName_r.c_str() );
        goto FUNC_END;
    }

	pcmFile_l.write((const char*)&wHdr, sizeof(WAVE_HEADER));
	pcmFile_r.write((const char*)&wHdr, sizeof(WAVE_HEADER));

	CLog::Print( LOG_INFO, "%s(%s) is started", __FUNCTION__, strCallId.c_str() );
	short linearData[1024];
	int vdLen;
	int vdRtLen;
	unsigned char *vceData;

	while( gbStop == false )
	{
		// gclsSignalPacketQueue에서 패킷을 가져온다.
		// SIP패킷 분석 및 신규 호 시작 시엔 전용 쓰레드 생성
		item = que->pop();
		if ( item )
		{
			vdLen = item->m_iUdpBodyLen - RTP_HEAD_SIZE;
			vceData = (unsigned char *)item->m_pszUdpBody + RTP_HEAD_SIZE;
			vdRtLen = vdLen * sizeof(short);
			for (int i=0; i<vdLen; i++) {
				linearData[i] = alaw2linear(vceData[i]);
			}

			if ( mask != (item->m_psttIp4Header->daddr & mask) ) {
				// 상담원 목소리 저장(right)
				pcmFile_r.write((const char*)linearData, vdRtLen);
				totalVoiceDataLen[0] += vdRtLen;
			}
			else
			{
				// 고객 목소리 저장(left)
				pcmFile_l.write((const char*)linearData, vdRtLen);
				totalVoiceDataLen[1] += vdRtLen;
			}

			// 이 곳에서 처리한 item은 삭제한다.
			delete item;
			item = nullptr;

		}
		else
		{
			MiliSleep(1);

            // search Call-ID at SIP Call MAP
			if ( !gclsCallMap.FindCall(strCallId) ) break;
            // if not exist break.
		}
	}

	wHdr.Riff.ChunkSize = totalVoiceDataLen[0] + sizeof(WAVE_HEADER) - 8;
	wHdr.Data.ChunkSize = totalVoiceDataLen[0];

	pcmFile_r.seekp(0);
	pcmFile_r.write((const char*)&wHdr, sizeof(WAVE_HEADER));

	pcmFile_r.close();

	wHdr.Riff.ChunkSize = totalVoiceDataLen[1] + sizeof(WAVE_HEADER) - 8;
	wHdr.Data.ChunkSize = totalVoiceDataLen[1];

	pcmFile_l.seekp(0);
	pcmFile_l.write((const char*)&wHdr, sizeof(WAVE_HEADER));

	pcmFile_l.close();

FUNC_END:

	CLog::Print( LOG_INFO, "%s(%s) is stoped", __FUNCTION__, strCallId.c_str() );

	gclsVoiceMap.deleteQueue( strCallId );

	return 0;
}

/**
 * @ingroup SipCallDump
 * @brief 패킷 덤프 쓰레드를 시작한다.
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool StartVoiceHandleThread( std::string &strCallId )
{
	return StartThread( "StartVoiceHandleThread", VoiceHandleThread, (void *)strCallId.c_str() );
}
