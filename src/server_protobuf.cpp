
#include <iostream>
#include <string>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "test.pb.h"
#include <errno.h>

#include "include/google/protobuf/map.h"

using namespace std;

void dosome(int client_fd)
{
	ssize_t len = 0;
	int datelen = 0;
	//char buf[2048] = { 0 };
	//接收消息
	len = recv(client_fd, (void *)&datelen, sizeof(int), 0);
	if (len <= 0)
	{
		printf("recv() failed and len[%d] errno[%s]!\n",(int)len,strerror(errno));
		return;
	}
	if (len != sizeof(int))
	{
		printf("data error !\n");
		return;
	}
	datelen = ntohl(datelen);
	printf("date len is[%d]\n",datelen);
	char *buf = (char *)malloc(datelen);
	memset(buf, 0, datelen);
	char *pcIndex = buf;
	int reallen = 0;
	char gcTemp[1024] = { 0 };
	//获取消息长度
	while (true)
	{
		memset(gcTemp, 0, 1024);
		len = recv(client_fd, gcTemp, 1024, 0);
		if (len <= 0)
		{
			printf("peer is closed !\n");
			return;
		}
		memcpy(pcIndex + reallen, gcTemp, len);
		reallen += len;

		if (datelen <= reallen)
		{
			printf("recv real data is [%d] \n", reallen);
			break;
		}
	}
	
	commun::Say msg;
	msg.ParseFromArray(buf, datelen);
	cout << msg.data() << endl;

	//获取文件数据
	char *pcFileData = (char *)malloc(msg.filesize());
	if (pcFileData)
	{
		memcpy(pcFileData, msg.audio().c_str(), msg.filesize());
		FILE *pfw = fopen("11.wav", "w");
		fwrite(pcFileData, msg.filesize(), 1, pfw);
		fclose(pfw);
		cout << "file size is " << msg.filesize() << endl;
		cout << "write file ok" << endl;
	}

	cout << "show name" << endl;

	//数组类型变量的迭代器遍历--当前数组类型也可以下标遍历，但是怕出现越界错误，不使用
	google::protobuf::internal::RepeatedPtrIterator<const string> it2; 
	it2 = msg.name().begin();
	for (it2 = msg.name().begin(); it2 != msg.name().end(); ++it2)
	{
		cout << "name is " << *it2 << endl;
	}

	cout << "show map" << endl;

	//map类型变量的迭代器遍历
	google::protobuf::Map<google::protobuf::int32, string>::const_iterator it;

	for (it = msg.projects().begin(); it != msg.projects().end(); ++it)
	{
		cout << "key is " << it->first << " and value is " << it->second << endl;
	}
	
}

int server_listen()
{
	int fd = 0;
	struct sockaddr_in addr;
	struct sockaddr_in peer_addr;
	int peer_fd = 0;
	socklen_t peeer_len = sizeof(struct sockaddr_in);

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
	{
		printf("socket() failed \n");
		return -1;
	}

	{
		int on = 1;
		if (setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))
		{
			printf("setsockopt() failed \n");
			return -1;
		}
	}
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8888);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)))
	{
		printf("bind() failed \n");
		return -1;
	}

	if (listen(fd,SOMAXCONN))
	{
		printf("listen() failed \n");
		return -1;
	}

	peer_fd = accept(fd, (struct sockaddr *)&peer_addr, &peeer_len);
	if (peer_fd < 0)
	{
		printf("listen() failed \n");
		return -1;
	}

	while (true)
	{
		dosome(peer_fd);
		sleep(3);
	}

	close(fd);
	close(peer_fd);

	return 0;

}

int main()
{
	server_listen();
	return 0;
}

