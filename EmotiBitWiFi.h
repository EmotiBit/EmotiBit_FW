/// EmotiBitWifi
///
/// Supports WiFi communications with the EmotiBit 
///
/// Written by produceconsumerobot Dec 2019
///
/// **** Overview of EmotiBitWiFi communication ****
/// EmotiBit uses 3 separate channels for network advertising, control
/// and data transmission. Exactly what protocols are used for each 
///	of these channels can change, but typically the following are used:
///		- Advertising: UDP on hard-coded EMOTIBIT_WIFI_ADVERTISING_PORT
///		- Control: TCP on port chosen by host 
///		- Data: UDP on port chosen by host 
///	Typical Usage:
///   - in setup():
///		  - Call EmotiBitWiFi.begin(ssid, password) in setup() to setup WiFi+advertising connection
///   - in loop():
///		  - Call EmotiBitWifi.update() to handle advertising, connection requests and time syncing
///				- Note that this can take up to MAX_SYNC_WAIT_INTERVAL (default 100 msec)
///     - Call emotibitWifi.readControl(String& controlPacket) to read an incoming control packet
///     - Call emotibitWifi.sendData(String& data) to send data
///	Typical program flow (behind the scenes)
///		- Host messages the network to determine which EmotiBits are present
///			- All EmotiBits respond indicating availability for connection
///		- Host initiates a connection with a specific EmotiBit on a specified port
///		- EmotiBit connects to the control channel and starts sending data on the specified port
///		- Host sends periodic messages to maintain Host-EmotiBit connection
///		- Host initiates disconnection from EmotiBit


#pragma once

#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include "EmotiBitComms.h"
#include "EmotiBitPacket.h"

class EmotiBitWiFi {
public:
	typedef struct Credential
	{
		String ssid = "";
		String pass = "";
	};
	static const uint8_t MAX_CREDENTIALS = 12;
	Credential credentials[MAX_CREDENTIALS];
	uint8_t currentCredential = 0;
	uint8_t numCredentials = 0;
	uint32_t wifiBeginStart;
	const uint8_t MAX_WIFI_RECONNECT_ATTEMPTS = 2;


	uint16_t _advertisingPort = EmotiBitComms::WIFI_ADVERTISING_PORT;	// Port to advertise EmotiBits on the network
	int32_t _controlPort = -1;								// Port to toggle EmotiBit controls
	int32_t _dataPort = -1;								// Port to sent EmotiBit data

	IPAddress _hostIp;

	bool _wifiOff = true;
	bool _needsAdvertisingBegin = true;		// Tracks whether advertising port has been started

	WiFiClient _controlCxn;
	WiFiUDP _advertisingCxn;
	WiFiUDP _dataCxn;

	volatile bool _isConnected = false;
	int _keyIndex = 0;            // your network key Index number (needed only for WEP)
	//bool gotIp = false;
	uint8_t _nDataSends = 1; // Number of times to send the same packet (e.g. for UDPx3 = 3)

	static const bool DEBUG_PRINT = false;
	static const uint8_t SUCCESS = 0;
	static const uint8_t FAIL = -1;
	static const uint16_t MAX_SEND_LEN = 512;							// messages longer than this will be broken up into multiple sends
	static const uint32_t WIFI_BEGIN_ATTEMPT_DELAY = 5000;
	static const uint32_t WIFI_BEGIN_SWITCH_CRED = 300000;// Set to 30000 for debug, 300000 for Release
	static const uint32_t SETUP_TIMEOUT = 61500;          // Enough time to run through list of network credentials twice
	static const uint8_t MAX_WIFI_CONNECT_HISTORY = 20;		// NO. of wifi connectivity status to remember
	static const uint16_t TIME_SYNC_INTERVAL = 5107;			// milliseconds between time syncs
	static const uint16_t MAX_SYNC_WAIT_INTERVAL = 100;		// milliseconds to wait for time sync ACK
	uint32_t connectionTimer;
	uint16_t connectionTimeout = 10000;	// if no PING for specified timeout sets _isConnected = false & calls disconnect()
	//uint16_t wifiTimeout = 20000;	// if no WiFi connection after timeout we disconnect from WiFi and tries to connect to one on our list 

	char _inPacketBuffer[EmotiBitPacket::MAX_TO_EMOTIBIT_PACKET_LEN + 1]; //buffer to hold incoming packet
	EmotiBitPacket::Header _packetHeader;
#ifdef ARDUINO
	String _receivedAdvertisingPacket = "";
	String _receivedControlMessage = "";
#else

#endif

	uint16_t advertisingPacketCounter = 0;
	uint16_t controlPacketCounter = 0;
	//uint16_t dataPacketCounter = 0;

	//int8_t setup();
	uint8_t begin(uint16_t timeout = 61500, uint16_t attemptDelay = 1000);
	//uint8_t begin(uint8_t credentialIndex, uint8_t maxAttempts = 10, uint16_t attemptDelay = 1000);
	uint8_t begin(const String &ssid, const String &pass, uint8_t maxAttempts = 10, uint16_t attemptDelay = 1000);
	void end();
	int8_t updateWiFi();
	int8_t connect(const IPAddress &hostIp, const String &connectPayload);
	int8_t connect(const IPAddress &hostIp, uint16_t controlPort, uint16_t dataPort);
	int8_t disconnect();
	int8_t update(String &syncPackets, uint16_t &syncPacketCounter);	// Handles advertising and time syncing. Can take up to MAX_SYNC_WAIT_INTERVAL.
	int8_t processAdvertising();
	uint8_t readControl(String &packet);
	int8_t sendControl(const String &message);
	int8_t sendData(const String &message);
	int8_t readUdp(WiFiUDP &udp, String &message);
	int8_t sendAdvertising(const String &message, const IPAddress &ip, uint16_t port);
	int8_t processTimeSync(String &syncPackets, uint16_t &syncPacketCounter);
	int8_t sendUdp(WiFiUDP &udp, const String &message, const IPAddress &ip, uint16_t port);
	String createPongPacket();
	void setTimeSyncInterval(const uint32_t &interval);
	void setAdvertisingInterval(const uint32_t &interval);
	int8_t addCredential(const String &ssid, const String &password);
	void printWiFiStatus();
	uint8_t listNetworks();
	bool isConnected();
	bool isOff();
	int8_t status();
};
