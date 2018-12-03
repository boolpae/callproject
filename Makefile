# Makefile for all of C++ SipStack
#
# programmer : yee young han ( websearch@naver.com )
#            : http://blog.naver.com/websearch
# start date : 2012/07/12

all:
	cd SipPlatform && make
	cd SipParser && make
	cd SdpParser && make
	cd XmlParser && make
	cd SipStack && make
	cd SipUserAgent && make
	cd ServerPlatform && make
	cd TestSipParser && make
	cd TestSipPlatform && make
	cd SipClient && make
	cd SimpleSipServer && make
	cd KSipServer && make
	cd SipLoadBalancer && make
	cd SipSpeedLinux && make
	cd TcpStack && make
	cd HttpParser && make
	cd HttpStack && make
	cd StunParser && make
	cd TestWebRtc && make

clean:
	cd SipPlatform && make clean
	cd SipParser && make clean
	cd SdpParser && make clean
	cd XmlParser && make clean
	cd SipStack && make clean
	cd SipUserAgent && make clean
	cd ServerPlatform && make clean
	cd TestSipParser && make clean
	cd TestSipPlatform && make clean
	cd SipClient && make clean
	cd SimpleSipServer && make clean
	cd KSipServer && make clean
	cd SipLoadBalancer && make clean
	cd SipSpeedLinux && make clean
	cd TcpStack && make clean
	cd HttpParser && make clean
	cd HttpStack && make clean
	cd StunParser && make clean
	cd TestWebRtc && make clean

install:

