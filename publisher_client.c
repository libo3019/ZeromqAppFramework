#include <zmq.h>
#include <stdio.h>
//#include <unistd.h>
#include <string.h>
#define snprintf _snprintf
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
  void* broadcast = zmq_socket(context, ZMQ_PUB);

  int rec = zmq_connect(broadcast, addr);
  if (rec) {
    printf("zmq_connect %s error:%d\n", addr, rec);
	zmq_close(broadcast);
    zmq_ctx_destroy(context);
    return -2;
  }
  Sleep(3000);
  int i = 0;
  while (1) 
  {
    /* 要“广播”发布新配置的频道名称 */
	  char channel[20] = "zeromq";
	  char msg[1024] = "msg1";
	  sprintf(msg, "msg%d", ++i);

    rec = zmq_send(broadcast, channel, strlen(channel), ZMQ_SNDMORE);
    printf("send context(%s) ... %d\n", channel, rec);

    rec = zmq_send(broadcast, msg, strlen(msg), 0);
    printf("send context(%s) ... %d\n", msg, rec);
	Sleep(7000);
  }

  zmq_close(broadcast);
  zmq_ctx_destroy(context);

  printf("good bye and good luck!\n");
  return 0;
}
