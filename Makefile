all:
	g++ Main.cpp Application.cpp ApplicationRunner.cpp IPCTest.cpp Messenger.cpp Connector.cpp FifoConnector.cpp FifoMessenger.cpp MessengerFactory.cpp MessengerFactoryImplementation.cpp -Wall -o ipc
clean:
	rm -rf ipc
