TARGET = ipc

PACKAGES = Application IPCTest Messenger MessengerFactory Fifo Socket

BUILDEDAPPLICATION = build/Application.o build/ApplicationRunner.o
BUILDEDIPCTEST = build/IPCTest.o
BUILDEDMESSENGER = build/Connector.o build/Messenger.o
BUILDEDMESSENGERFACTORY = build/MessengerFactory.o build/MessengerFactoryImplementation.o
BUILDEDFIFO = build/FifoConnector.o build/FifoMessenger.o
BUILDEDSOCKET = build/Socket.o build/SocketConnector.o build/SocketMessenger.o

.PHONY: all clean Application IPCTest Messenger MessengerFactory Fifo

all: $(TARGET)

clean:
	rm -rf bin/$(TARGET) build/*.o
	rmdir build
	rmdir bin

build:
	mkdir build

bin:
	mkdir bin

$(TARGET): bin $(PACKAGES) Main.o
	g++ -Wall -o bin/$(TARGET) $(BUILDEDAPPLICATION) $(BUILDEDIPCTEST) $(BUILDEDMESSENGER) $(BUILDEDMESSENGERFACTORY) $(BUILDEDFIFO) $(BUILDEDSOCKET) build/Main.o

Main.o: build
	g++ -c -o build/Main.o Main.cpp

Application: build
	g++ -c -o build/Application.o code/Application.cpp
	g++ -c -o build/ApplicationRunner.o code/ApplicationRunner.cpp

IPCTest: build
	g++ -c -o build/IPCTest.o code/IPCTest.cpp

Messenger: build
	g++ -c -o build/Connector.o code/Connector.cpp
	g++ -c -o build/Messenger.o code/Messenger.cpp

MessengerFactory: build
	g++ -c -o build/MessengerFactory.o code/MessengerFactory.cpp
	g++ -c -o build/MessengerFactoryImplementation.o code/MessengerFactoryImplementation.cpp

Fifo: build
	g++ -c -o build/FifoConnector.o code/FifoConnector.cpp
	g++ -c -o build/FifoMessenger.o code/FifoMessenger.cpp

Socket: build
	g++ -c -o build/Socket.o code/Socket.cpp
	g++ -c -o build/SocketConnector.o code/SocketConnector.cpp
	g++ -c -o build/SocketMessenger.o code/SocketMessenger.cpp
