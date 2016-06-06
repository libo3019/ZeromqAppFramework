#include <zmq.h>
#include <stdio.h>
//#include <unistd.h>
#include <string.h>
#define snprintf _snprintf

int readOneMsg(void* socket, char *data, int size)
{
	int64_t rcvmore = 0;
	size_t sz = sizeof(rcvmore);
	int pos = 0;
	data[0] = 0;
	do {
		/* Create an empty OMQ message to hold the message part */
		zmq_msg_t part;
		int rc = zmq_msg_init(&part);
		if (rc) {
			printf("readOneMsg zmq_msg_init error:%d\n", rc);
		}
		/* Block until a message is available to be received from socket */
		rc = zmq_recv(socket, data + pos, size - 1 - pos, 0);
		if (rc <  0) {
			printf("readOneMsg zmq_recvmsg error:%d\n", rc);
		}
		else {
			pos += rc;
			// printf("part zmq_recvmsg (%d)=%s\n", rc, configure);
			//snprintf(configure, sizeof(configure), "%.*s", rc, (char*)zmq_msg_data(&part));
		}

		rc = zmq_getsockopt(socket, ZMQ_RCVMORE, &rcvmore, &sz);
		zmq_msg_close(&part);

	} while (rcvmore);
	data[pos] = 0;
	printf("recv new configure:\n%s\n", data);

	return 0;
}

int main(int argc, char** argv)
{
  if (argc < 3) {
    printf("usage : %s ip port\n", argv[0]);
    return -1;
  }

  int major, minor, patch;
  zmq_version (&major, &minor, &patch);
  printf ("Current 0MQ version is %d.%d.%d\n", major, minor, patch);
  printf("===========================================\n\n");

  char addr[128] = {0};
  snprintf(addr, sizeof(addr), "tcp://%s:%s", argv[1], argv[2]);

  void* context = zmq_ctx_new();
  void* broadcast = zmq_socket(context, ZMQ_PUSH);

  int rec = zmq_bind(broadcast, addr);
  if (rec) { 
    printf("zmq_bind %s error:%d\n", addr, rec);
    zmq_close(broadcast);
    zmq_ctx_destroy(context);
    return -2;
  }
  Sleep(3000);

  int i = 0;

  zmq_pollitem_t items[] = { { broadcast, 0, ZMQ_POLLIN | ZMQ_POLLOUT | ZMQ_POLLERR, 0 } };
  char data[8192];
  int idx = 0;
  while (1) {
	char channel[] = "mqzero";
	rec = zmq_send(broadcast, channel, strlen(channel), ZMQ_SNDMORE);
	printf("send context(%s) ... %d\n", channel, rec);
	char msg[8192];
	sprintf(msg, "msg%d", ++idx);
	rec = zmq_send(broadcast, msg, strlen(msg), 0);
	printf("send context(%s) ... %d\n", msg, rec);
	Sleep(7000);
	//readOneMsg(broadcast, data, sizeof(data));
	//printf("Handle msg: [%s]\n", data);
		 
  }

  zmq_close(broadcast);
  zmq_ctx_destroy(context);

  printf("good bye and good luck!\n");
  return 0;
}
