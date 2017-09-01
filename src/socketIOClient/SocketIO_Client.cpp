#include "SocketIO_Client.h"

const String getEventName(const String msg) {
	return msg.substring(4, msg.indexOf("\"",4));
}

const String getEventPayload(const String msg) {
	String result = msg.substring(msg.indexOf("\"",4)+2,msg.length()-1);
	if(result.startsWith("\"")) {
		result.remove(0,1);
	}
	if(result.endsWith("\"")) {
		result.remove(result.length()-1);
	}
	return result;
}

SocketIO_Client::SocketIO_Client(){
	this->_connected = false;
	this->_debug = false;
}

SocketIO_Client::SocketIO_Client(bool debug){
	this->_connected = false;
	this->_debug = debug;
}


bool SocketIO_Client::isConnected(){
	return this->_connected;
}

bool SocketIO_Client::isDebugging(){
    return this->_debug;
}

void SocketIO_Client::setDebugging(bool debug){
    this->_debug =  debug;
}


void SocketIO_Client::webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
	String msg;
	switch(type) {
		case WStype_DISCONNECTED:
			this->_connected = false;
			if (this->_debug)
				SOCKETIOCLIENT_DEBUG("[SIoC] Disconnected!\n");
			break;
		case WStype_CONNECTED:
			if (this->_debug)
				SOCKETIOCLIENT_DEBUG("[SIoC] Connected to url: %s\n",  payload);
			break;
		case WStype_TEXT:
			msg = String((char*)payload);
			if(msg.startsWith("42")) {
				trigger(getEventName(msg).c_str(), getEventPayload(msg).c_str(), length);
			} else if(msg.startsWith("2")) {
				_webSocket.sendTXT("3");
			} else if(msg.startsWith("40")) {
				this->_connected = true;
				trigger("connect", NULL, 0);
			} else if(msg.startsWith("41")) {
				this->_connected = false;
				trigger("disconnect", NULL, 0);
			}
			break;
		case WStype_BIN:
			if (this->_debug)
				SOCKETIOCLIENT_DEBUG("[SIoC] get binary length: %u\n", length);
			hexdump(payload, length);
		break;
	}
}

void SocketIO_Client::beginSSL(const char* host, const int port, const char* url, const char* fingerprint) {
	_webSocket.beginSSL(host, port, url, fingerprint); 
    initialize();
}

void SocketIO_Client::begin(const char* host, const int port, const char* url) {
	_webSocket.begin(host, port, url);
    initialize();
}

void SocketIO_Client::initialize() {
    _webSocket.onEvent(std::bind(&SocketIO_Client::webSocketEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	_lastPing = millis();
}

void SocketIO_Client::loop() {
	_webSocket.loop();
	for(auto packet=_packets.begin(); packet != _packets.end();) {
		if(_webSocket.sendTXT(*packet)) {
			if (this->_debug)
				SOCKETIOCLIENT_DEBUG("[SIoC] packet \"%s\" emitted\n", packet->c_str());
			packet = _packets.erase(packet);
		} else {
			++packet;
		}
	}

	if(millis() - _lastPing > PING_INTERVAL) {
		_webSocket.sendTXT("2");
		_lastPing = millis();
	}
}

void SocketIO_Client::on(const char* event, std::function<void (const char * payload, size_t length)> func) {
	_events[event] = func;
}

void SocketIO_Client::emit(const char* event, const char * payload) {
	String msg = String("42");
  	msg += "[\"";
  	msg += event;
	msg += "\"";
	if(payload ) {
		msg += ",\"";
		msg += payload;
		msg += "\"";
	}
	msg += "]";
	if (this->_debug)
		SOCKETIOCLIENT_DEBUG("[SIoC] add packet %s\n", msg.c_str());
	_packets.push_back(msg);
}

void SocketIO_Client::trigger(const char* event, const char * payload, size_t length) {
	auto e = _events.find(event);
	if(e != _events.end()) {
		if (this->_debug)
			SOCKETIOCLIENT_DEBUG("[SIoC] trigger event %s\n", event);
		e->second(payload, length);
	} else {
		if (this->_debug)
			SOCKETIOCLIENT_DEBUG("[SIoC] event %s not found. %d events available\n", event, _events.size());
	}
}
