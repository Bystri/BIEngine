#include "gtest/gtest.h"

#include "../../BIEngine/Network/NetworkClient.h"
#include "../../BIEngine/Network/NetworkServer.h"

TEST(NetworkClient_NetworkServer, Connection) {
	BIEngine::SocketUtil::Init();
	
	BIEngine::NetworkServer server;
	server.Init(40000);
	
	const BIEngine::SocketAddressPtr sockAddr = BIEngine::SocketUtil::CreateIPv4SocketFromString("127.0.0.1:40000");
	BIEngine::NetworkClient client;
	client.Init(*sockAddr);

	EXPECT_EQ(client.GetState(), BIEngine::NetworkClient::State::SayingHello);
	EXPECT_EQ(server.GetConnectedClients(), 0);

	float time = 0.0f;
	for (int i = 0; i < 10; ++i) {
		server.Update();
		client.Update(time);

		time += 0.5f;
	}

	EXPECT_EQ(client.GetState(), BIEngine::NetworkClient::State::Welcomed);
	EXPECT_EQ(server.GetConnectedClients(), 1);

	BIEngine::SocketUtil::Terminate();
}

TEST(NetworkClient_NetworkServer, MultiplyClientConnection) {
	BIEngine::SocketUtil::Init();

	BIEngine::NetworkServer server;
	server.Init(40000);

	constexpr int CLIENT_NUM = 5;

	const BIEngine::SocketAddressPtr sockAddr = BIEngine::SocketUtil::CreateIPv4SocketFromString("127.0.0.1:40000");
	BIEngine::NetworkClient client[CLIENT_NUM];
	
	for (int i = 0; i < CLIENT_NUM; ++i) {
		client[i].Init(*sockAddr);
	}

	for (int i = 0; i < CLIENT_NUM; ++i) {
		EXPECT_EQ(client[i].GetState(), BIEngine::NetworkClient::State::SayingHello);
	}
	EXPECT_EQ(server.GetConnectedClients(), 0);

	float time = 0.0f;
	for (int i = 0; i < 10; ++i) {
		server.Update();
		for (int i = 0; i < CLIENT_NUM; ++i) {
			client[i].Update(time);
		}

		time += 0.5f;
	}

	for (int i = 0; i < CLIENT_NUM; ++i) {
		EXPECT_EQ(client[i].GetState(), BIEngine::NetworkClient::State::Welcomed);
	}
	EXPECT_EQ(server.GetConnectedClients(), CLIENT_NUM);

	BIEngine::SocketUtil::Terminate();
}

TEST(NetworkClient_NetworkServer, ClientSendPacket) {
	BIEngine::SocketUtil::Init();

	BIEngine::NetworkServer server;
	server.Init(40000);

	const BIEngine::SocketAddressPtr sockAddr = BIEngine::SocketUtil::CreateIPv4SocketFromString("127.0.0.1:40000");
	BIEngine::NetworkClient client;
	client.Init(*sockAddr);

	EXPECT_EQ(client.GetState(), BIEngine::NetworkClient::State::SayingHello);
	EXPECT_EQ(server.GetConnectedClients(), 0);

	float time = 0.0f;
	for (int i = 0; i < 10; ++i) {
		server.Update();
		client.Update(time);

		time += 0.5f;
	}

	constexpr uint32_t PACKET_DATA[] = { 12345, 23455, 12421, 10245, 2454 };
	constexpr int PACKETS_NUM = sizeof(PACKET_DATA) / sizeof(uint32_t);

	for (int i = 0; i < PACKETS_NUM; ++i) {
		BIEngine::OutputMemoryBitStream packet;
		BIEngine::Serialize(packet, PACKET_DATA[i]);
		client.SendPacket(packet);
	}

	server.Update();
	
	int cnt = 0;
	while (BIEngine::UniquePtr<BIEngine::InputMemoryBitStream> pStream = server.ReceivePacket(0)) {
		uint32_t packetData;
		BIEngine::Deserialize(*pStream, packetData);
		EXPECT_EQ(packetData, PACKET_DATA[cnt]);

		++cnt;
	}

	EXPECT_EQ(client.GetState(), BIEngine::NetworkClient::State::Welcomed);
	EXPECT_EQ(server.GetConnectedClients(), 1);
	EXPECT_EQ(cnt, PACKETS_NUM);

	BIEngine::SocketUtil::Terminate();
}

TEST(NetworkClient_NetworkServer, MultiplyClientSendPacket) {
	BIEngine::SocketUtil::Init();

	BIEngine::NetworkServer server;
	server.Init(40000);

	constexpr int CLIENT_NUM = 5;

	const BIEngine::SocketAddressPtr sockAddr = BIEngine::SocketUtil::CreateIPv4SocketFromString("127.0.0.1:40000");
	BIEngine::NetworkClient client[CLIENT_NUM];

	for (int i = 0; i < CLIENT_NUM; ++i) {
		client[i].Init(*sockAddr);
	}

	for (int i = 0; i < CLIENT_NUM; ++i) {
		EXPECT_EQ(client[i].GetState(), BIEngine::NetworkClient::State::SayingHello);
	}
	EXPECT_EQ(server.GetConnectedClients(), 0);

	float time = 0.0f;
	for (int i = 0; i < 10; ++i) {
		server.Update();
		for (int i = 0; i < CLIENT_NUM; ++i) {
			client[i].Update(time);
		}

		time += 0.5f;
	}

	constexpr uint32_t PACKET_DATA[] = { 12345, 23455, 12421, 10245, 2454 };
	constexpr int PACKETS_NUM = sizeof(PACKET_DATA) / sizeof(uint32_t);

	for (int i = 0; i < PACKETS_NUM; ++i) {
		BIEngine::OutputMemoryBitStream packet;
		BIEngine::Serialize(packet, PACKET_DATA[i]);
		client[i].SendPacket(packet);
	}

	server.Update();

	int cnt = 0;
	for (int i = 0; i < server.GetConnectedClients(); ++i) {
		while (BIEngine::UniquePtr<BIEngine::InputMemoryBitStream> pStream = server.ReceivePacket(i)) {
			uint32_t packetData;
			BIEngine::Deserialize(*pStream, packetData);
			EXPECT_EQ(packetData, PACKET_DATA[cnt]);

			++cnt;
		}
	}

	for (int i = 0; i < CLIENT_NUM; ++i) {
		EXPECT_EQ(client[i].GetState(), BIEngine::NetworkClient::State::Welcomed);
	}
	EXPECT_EQ(server.GetConnectedClients(), CLIENT_NUM);
	EXPECT_EQ(cnt, PACKETS_NUM);

	BIEngine::SocketUtil::Terminate();
}

TEST(NetworkClient_NetworkServer, ServerSendPacket) {
	BIEngine::SocketUtil::Init();

	BIEngine::NetworkServer server;
	server.Init(40000);

	const BIEngine::SocketAddressPtr sockAddr = BIEngine::SocketUtil::CreateIPv4SocketFromString("127.0.0.1:40000");
	BIEngine::NetworkClient client;
	client.Init(*sockAddr);

	EXPECT_EQ(client.GetState(), BIEngine::NetworkClient::State::SayingHello);
	EXPECT_EQ(server.GetConnectedClients(), 0);

	float time = 0.0f;
	for (int i = 0; i < 10; ++i) {
		server.Update();
		client.Update(time);

		time += 0.5f;
	}

	constexpr uint32_t PACKET_DATA[] = { 12345, 23455, 12421, 10245, 2454 };
	constexpr int PACKETS_NUM = sizeof(PACKET_DATA) / sizeof(uint32_t);

	for (int i = 0; i < PACKETS_NUM; ++i) {
		BIEngine::OutputMemoryBitStream packet;
		BIEngine::Serialize(packet, PACKET_DATA[i]);
		server.SendPacket(0, packet);
	}

	client.Update(time);

	int cnt = 0;
	while (BIEngine::UniquePtr<BIEngine::InputMemoryBitStream> pStream = client.ReceivePacket()) {
		uint32_t packetData;
		BIEngine::Deserialize(*pStream, packetData);
		EXPECT_EQ(packetData, PACKET_DATA[cnt]);

		++cnt;
	}

	EXPECT_EQ(client.GetState(), BIEngine::NetworkClient::State::Welcomed);
	EXPECT_EQ(server.GetConnectedClients(), 1);
	EXPECT_EQ(cnt, PACKETS_NUM);

	BIEngine::SocketUtil::Terminate();
}

TEST(NetworkClient_NetworkServer, SendPacketSendPacketToMultiplyClient) {
	BIEngine::SocketUtil::Init();

	BIEngine::NetworkServer server;
	server.Init(40000);

	constexpr int CLIENT_NUM = 5;

	const BIEngine::SocketAddressPtr sockAddr = BIEngine::SocketUtil::CreateIPv4SocketFromString("127.0.0.1:40000");
	BIEngine::NetworkClient client[CLIENT_NUM];

	for (int i = 0; i < CLIENT_NUM; ++i) {
		client[i].Init(*sockAddr);
	}

	for (int i = 0; i < CLIENT_NUM; ++i) {
		EXPECT_EQ(client[i].GetState(), BIEngine::NetworkClient::State::SayingHello);
	}
	EXPECT_EQ(server.GetConnectedClients(), 0);

	float time = 0.0f;
	for (int i = 0; i < 10; ++i) {
		server.Update();
		for (int i = 0; i < CLIENT_NUM; ++i) {
			client[i].Update(time);
		}

		time += 0.5f;
	}

	constexpr uint32_t PACKET_DATA[] = { 12345, 23455, 12421, 10245, 2454 };
	constexpr int PACKETS_NUM = sizeof(PACKET_DATA) / sizeof(uint32_t);

	for (int i = 0; i < PACKETS_NUM; ++i) {
		BIEngine::OutputMemoryBitStream packet;
		BIEngine::Serialize(packet, PACKET_DATA[i]);
		server.SendPacket(i, packet);
	}

	for (int i = 0; i < CLIENT_NUM; ++i) {
		client[i].Update(time);
	}

	int cnt = 0;
	for (int i = 0; i < CLIENT_NUM; ++i) {
		while (BIEngine::UniquePtr<BIEngine::InputMemoryBitStream> pStream = client[i].ReceivePacket()) {
			uint32_t packetData;
			BIEngine::Deserialize(*pStream, packetData);
			EXPECT_EQ(packetData, PACKET_DATA[cnt]);

			++cnt;
		}
	}

	for (int i = 0; i < CLIENT_NUM; ++i) {
		EXPECT_EQ(client[i].GetState(), BIEngine::NetworkClient::State::Welcomed);
	}
	EXPECT_EQ(server.GetConnectedClients(), CLIENT_NUM);
	EXPECT_EQ(cnt, PACKETS_NUM);

	BIEngine::SocketUtil::Terminate();
}