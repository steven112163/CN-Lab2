CC=gcc
TCPSERVER=TcpServer
TCPCLIENT=TcpClient
UDPSERVER=UdpServer
UDPCLIENT=UdpClient

all: tcp udp

tcp: $(TCPSERVER).c $(TCPCLIENT).c
	$(CC) -o $(TCPSERVER).exe $(TCPSERVER).c
	$(CC) -o $(TCPCLIENT).exe $(TCPCLIENT).c

udp: $(UDPSERVER).c $(UDPCLIENT).c
	$(CC) -o $(UDPSERVER).exe $(UDPSERVER).c
	$(CC) -o $(UDPCLIENT).exe $(UDPCLIENT).c

clean:
	rm $(TCPSERVER).exe $(TCPCLIENT).exe $(UDPSERVER).exe $(UDPCLIENT).exe