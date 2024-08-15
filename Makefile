all:
	g++ Main.cpp Application.cpp ApplicationRunner.cpp IPCTest.cpp Messenger.cpp Connector.cpp FifoConnector.cpp FifoMessenger.cpp -Wall -o ipc
