# Makefile for SipCallDump
#
# programmer : yee young han ( websearch@naver.com )
#            : http://blog.naver.com/websearch
# start date : 2012/08/09

include ../Makefile.mk

INC=-I../SipStack -I../SipParser -I../SdpParser -I../SipPlatform \
	-I../ServerPlatform -I../XmlParser \
	$(OPENSSL_INC)
LIB=-L../SipStack -lsipstack \
	-L../SipParser -lsipparser \
	-L../SdpParser -lsdpparser \
	-L../SipPlatform -lsipplatform \
	-L../ServerPlatform -lserverplatform \
	-L../XmlParser -lxmlparser \
	$(OPENSSL_LIB) -lssl -lcrypto -lpcap -lpthread

.SUFFIXES: .o .cpp
%.o : %.cpp
	$(CC) $(CFLAGS) -c $< $(INC)

all: scd

#SRV_OBJ=PacketDumpThread.o PcapUtility.o SipCallDump.o SipCallMap.o PacketHeader.o \
#	RtpMap.o SipCallDumpSetup.o g711.o
SRV_OBJ=PacketDivideThread.o PcapUtility.o SipCallDumpSetup.o VoicePacketDivideThread.o \
	PacketDumpThread.o RtpMap.o SipCallMap.o g711.o PacketHeader.o SignalHandleThread.o \
	VoiceHandleThread.o PacketQueue.o SipCallDump.o VoiceMap.o

scd: $(SRV_OBJ)
	$(CC) $(CFLAGS) -o $@ $(SRV_OBJ) $(INC) $(LIB)

clean:
	rm -f scd
	rm -f *.o
	rm -f core.*

