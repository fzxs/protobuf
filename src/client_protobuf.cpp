#include <iostream>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "test.pb.h"

#include "include/google/protobuf/map.h"

using namespace std;

//读取文件
int readAudio(char *path,char **pcBuf,long *plSize)
{
	FILE *pfr = NULL;
	char *pcOut = NULL;
	long size = 0;

	pfr = fopen(path, "r");
	if (NULL == pfr)
	{
		return -1;
	}
	//计算不大于2G文件的大小
	fseek(pfr, 0L, SEEK_END);
	size = ftell(pfr);

	//文件指针复位
	rewind(pfr);
	pcOut = (char *)malloc(size);
	fread(pcOut, size, 1, pfr);
	*pcBuf = pcOut;
	*plSize = size;

	fclose(pfr);
	pfr = NULL;

	return 0;
}

int connect_server()
{
	int fd = 0;
	struct sockaddr_in addr;
	int len = 0;
	char *pcData = NULL;
	long fileSize = 0;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
	{
		printf("socket() failed .\n");
		return -1;
	}

	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8888);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	if (connect(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)))
	{
		printf("connect() failed .\n");
		return -1;
	}

	commun::Say msg;
	
	//基本类型参数的赋值
	msg.set_code(13);
	msg.set_data("this is the last chance, go ,do not hesitate .\n");

	//读一个音频文件
	readAudio("../audio/test.wav", &pcData,&fileSize);
	//非字符串型字符数据赋值（例如音频文件）
	//注意需要加size，不然传过去就是字符串
	msg.set_audio(pcData, fileSize);
	msg.set_filesize(fileSize);

	//数组类型参数的赋值
	msg.add_name("feng");
	msg.add_name("yu");
	msg.add_name("lei");
	msg.add_name("dian");

	//map类型参数的赋值
	using MapPair = google::protobuf::MapPair<google::protobuf::int32, string>;

	msg.mutable_projects()->insert(MapPair(0, "tom"));
	msg.mutable_projects()->insert(MapPair(1, "jack"));
	msg.mutable_projects()->insert(MapPair(2, "dong"));


	len = htonl(msg.ByteSize());

	char *buffer = (char *)malloc(msg.ByteSize()+10);
	memset(buffer, 0, msg.ByteSize() + 10);
	memcpy(buffer, &len, sizeof(int));
	msg.SerializeToArray(buffer + 4, msg.ByteSize());

	for (int i = 0; i < 10; i++)
	{
		printf("buf i[%d]  [%d]\n", i, buffer[i+4]);
	}

	send(fd, buffer, msg.ByteSize()+4, 0);

	free(buffer);

	sleep(30);

	close(fd);

	return 0;

}

int main()
{
	connect_server();
	return 0;
}


