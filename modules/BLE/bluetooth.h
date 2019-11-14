#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <csignal>
#include <unistd.h>
#include <getopt.h>
#include <sys/time.h>
#include <sys/poll.h>
#include <sys/socket.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>


class Bluetooth {
public:
	static Bluetooth& getInstance();
	static int getSentPkt() { return sent_pkt; }
	static int getRecvPkt() { return recv_pkt; }
	static int getLoss() { return loss; }
	static void setLoss(int _loss) { loss = _loss; }
	double getLatency() { return (latency / (count - loss)); }
	void setCount(int count) { this->count = count; }
	void setDelay(int micro_sec) { this->delay = micro_sec; }
	int ping(const char *svr);

private:
	Bluetooth() {};
	~Bluetooth() {
		free(send_buf);
		free(recv_buf);
		free(_instance);
	}
	void initMemberValue();

	static Bluetooth* _instance;

	unsigned char *send_buf;
	unsigned char *recv_buf;

	bdaddr_t bdaddr;

	int size;
	int ident;
	int delay;
	int count;
	int timeout;
	int reverse;
	int verify;

	double latency;

	static int sent_pkt;
	static int recv_pkt;
	static int loss;

};
#endif
