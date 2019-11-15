#include "bluetooth.h"

Bluetooth* Bluetooth::_instance = NULL;

int Bluetooth::sent_pkt = 0;
int Bluetooth::recv_pkt = 0;
int Bluetooth::loss = 0;

static float tv2fl(struct timeval tv)
{
	return (float)(tv.tv_sec*1000.0) + (float)(tv.tv_usec/1000.0);
}

static void stat(int sig)
{
	int sent_pkt = Bluetooth::getSentPkt();
	int recv_pkt = Bluetooth::getRecvPkt();
	int loss = 0;
	if (sent_pkt) {
		loss = sent_pkt - recv_pkt;
	} else {
		loss = 0;
	}

	Bluetooth::setLoss(loss);
}

Bluetooth& Bluetooth::getInstance()
{
	Bluetooth *_instance = Bluetooth::_instance;
	if (_instance == NULL) {
		int size = 0;

		_instance = new Bluetooth();
		_instance->initMemberValue();

		size = _instance->size;

		_instance->send_buf = new unsigned char[L2CAP_CMD_HDR_SIZE + size];
		_instance->recv_buf = new unsigned char[L2CAP_CMD_HDR_SIZE + size];
	} else {
		_instance->initMemberValue();
	}

	return *_instance;
}

void Bluetooth::setTimeout(int millisec)
{
	if (millisec > delay) {
		timeout = millisec;
	} else {
		timeout = delay;
	}
}

void Bluetooth::initMemberValue()
{
	bdaddr_t bdaddr_any = { };
	size     = 44;
	ident    = 200;
	delay    = 300;
	count    = 10;
	timeout  = 10 * 1000; /* 10 sec */
	reverse  = 0;
	verify   = 0;

	sent_pkt = 0;
	recv_pkt = 0;

	loss     = 0;
	bacpy(&bdaddr, &bdaddr_any);
}

int Bluetooth::ping(const char *svr) 
{
	struct sigaction sa;
	struct sockaddr_l2 addr;
	socklen_t optlen;
	int sk;
	int count = this->count;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = stat;
	sigaction(SIGINT, &sa, NULL);

	latency = 0;

	if (!send_buf || !recv_buf) {
		std::cerr << "Can't allocate buffer" << std::endl;
		return -1;
	}

	try {
		int i;
		char str[18];
		uint8_t id;

		/* Create socket*/
		sk = socket(PF_BLUETOOTH, SOCK_RAW, BTPROTO_L2CAP);
		if (sk < 0) {
			throw std::runtime_error("Can't create socket");
		}

		/* Bind to local address */
		memset(&addr, 0, sizeof(addr));
		addr.l2_family = AF_BLUETOOTH;
		bacpy(&addr.l2_bdaddr, &bdaddr);

		if (bind(sk, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
			throw std::runtime_error("Can't bind socket");
		}

		/* Connect to remote device */
		memset(&addr, 0, sizeof(addr));
		addr.l2_family = AF_BLUETOOTH;
		if (svr != NULL)
			str2ba(svr, &addr.l2_bdaddr);

		if (connect(sk, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
			throw std::runtime_error("Can't connect");
		}

		/* Get local address */
		memset(&addr, 0, sizeof(addr));
		optlen = sizeof(addr);

		if (getsockname(sk, (struct sockaddr *)&addr, &optlen) < 0) {
			throw std::runtime_error("Can't get local address");
		}

		ba2str(&addr.l2_bdaddr, str);

		/* Initialize send buffer */
		for (i = 0; i < size; i++)
			send_buf[L2CAP_CMD_HDR_SIZE + i] = (i % 40) + 'A';

		id = ident;

		while (count == -1 || count-- > 0) {
			struct timeval tv_send, tv_recv, tv_diff;
			int lost;
			l2cap_cmd_hdr *send_cmd = (l2cap_cmd_hdr *) send_buf;
			l2cap_cmd_hdr *recv_cmd = (l2cap_cmd_hdr *) recv_buf;

			/* Build command header */
			send_cmd->ident = id;
			send_cmd->len = htobs(size);

			if (reverse)
				send_cmd->code = L2CAP_ECHO_RSP;
			else
				send_cmd->code = L2CAP_ECHO_REQ;

			gettimeofday(&tv_send, NULL);

			/* Send Echo Command */
			if (send(sk, send_buf, L2CAP_CMD_HDR_SIZE + size, 0) <= 0) {
				throw std::runtime_error("Send failed");
			}

			/* Wait for Echo Response */
			lost = 0;
			while (1) {
				struct pollfd pf[1];
				int err;

				pf[0].fd = sk;
				pf[0].events = POLLIN;

				if ((err = poll(pf, 1, timeout)) < 0) {
					throw std::runtime_error("Poll failed");
				}

				if (!err) {
					lost = 1;
					break;
				}

				if ((err = recv(sk, recv_buf,
								L2CAP_CMD_HDR_SIZE + size, 0)) < 0) {
					throw std::runtime_error("Recv failed");
				}

				if (!err) {
					throw std::runtime_error("Disconnected");
				}

				recv_cmd->len = btohs(recv_cmd->len);

				/* Check for our id */
				if (recv_cmd->ident != id)
					continue;

				/* Check type */
				if (!reverse && recv_cmd->code == L2CAP_ECHO_RSP)
					break;

				if (recv_cmd->code == L2CAP_COMMAND_REJ) {
					throw std::runtime_error("Peer doesn't support echo packets");
				}
			} // end of response while
			sent_pkt++;

			if (!lost) {
				recv_pkt++;

				gettimeofday(&tv_recv, NULL);
				timersub(&tv_recv, &tv_send, &tv_diff);

				if (verify) {
					/* check payload length */
					if (recv_cmd->len != size) {
						std::stringstream stream;
						std::string temp_str;
						stream << "Received " << recv_cmd->len
							<< " bytes, expected " << size
							<< std::endl;
						stream.str(temp_str);
						throw std::runtime_error(temp_str);
					}

					if (memcmp(&send_buf[L2CAP_CMD_HDR_SIZE],
							&recv_buf[L2CAP_CMD_HDR_SIZE], size)) {
						throw std::runtime_error("Response payload different\n");
					}
				} // end of if

				latency += tv2fl(tv_diff);

				if (delay > 0) {
					struct timespec req =  {0};
					req.tv_sec = 0;
					req.tv_nsec = delay * 1000000L;
					nanosleep(&req, (struct timespec *)NULL);
				}
			} else {
				throw std::runtime_error("No response...");
			}// end of if
			if (++id > 254)
				id = ident;
		} // end of ping while
		stat(0);
	} catch (const std::exception& e) {
		close(sk);
		std::cerr << e.what() << std::endl;
		return -1;
	}
	close(sk);
	return 0;
}

#ifdef BLUETOOTH_TEST_MODE_ON
int main(void)
{
	Bluetooth& blue = Bluetooth::getInstance();
	blue.setCount(10);
	blue.setDelay(10);
	blue.setTimeout(100);
	blue.ping("B8:27:EB:04:C9:79");
	std::cout << "Result >>>>> " << blue.getLoss()  << "/" << blue.getLatency() << std::endl;
	return 0;
}
#endif
