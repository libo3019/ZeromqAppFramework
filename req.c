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
    printf("usage : %s ip port filter1, [filter2, ...]\n", argv[0]);
    return -1;
  }

  int major, minor, patch;
  zmq_version (&major, &minor, &patch);
  printf ("Current 0MQ version is %d.%d.%d\n", major, minor, patch);
  printf("===========================================\n\n");

  char addr[128] = {0};
  snprintf(addr, sizeof(addr), "tcp://%s:%s", argv[1], argv[2]);

  void* context = zmq_ctx_new();
  void* reader = zmq_socket(context, ZMQ_REQ);
  
  int rec = zmq_connect(reader, addr);
  if (rec) {
    printf("zmq_connect %s error:%d\n", addr, rec);
    zmq_close(reader);
    zmq_ctx_destroy(context);
    return -2;
  }
  int i = 0;
  char data[8192];
  while (1)
  {
	  /* 要“广播”发布新配置的频道名称 */
	  char channel[20] = "zeromq";
	  char msg[1024] = "msg1";
	  sprintf(msg, "msg%d", ++i);

	  rec = zmq_send(reader, channel, strlen(channel), ZMQ_SNDMORE);
	  printf("send context(%s) ... %d\n", channel, rec);

	  rec = zmq_send(reader, msg, strlen(msg), 0);
	  printf("send context(%s) ... %d\n", msg, rec);
	  readOneMsg(reader, data, sizeof(data));
	  printf("Response: [%s]\n", data);
	  Sleep(1000);
  }
  
  zmq_close(reader);
  zmq_ctx_destroy(context);

  printf("good bye and good luck!\n");
  return 0;
}
