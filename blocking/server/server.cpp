#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <vector>
#include <map>
#include <algorithm>
#include <list>
#include <stack>
#include <queue>
#include <string.h>

using namespace std;

struct message
{
	int *fd;
	pthread_t *thread;	
};

struct data
{
	char s[1024];	// 要发送的数据
	int id;			// 用户id
};

queue<data> dataList;
map<int, int> userList;			// key值为用户id，value为用户fd

void *read_(void *fd_)
{
	message *mes = static_cast<message*>(fd_);
	int *fd = ((message*)fd_)->fd;
	pthread_t *thread = mes->thread;
	int len = 1;
	printf("%d\n", *fd);
	char s[1024];
	char data_[1024];
	while (1) {
		len = recv(*fd, s, sizeof(s), 0);
		if (!(strcmp(s, "exit") && strcmp(s, "quit") && len))
			break;
		int dirId = s[0] - '0';
		strcpy(data_, s+1);
		struct data dataTmp;
		dataTmp.id = dirId;
		strcpy(dataTmp.s, data_);
		dataList.push(dataTmp);
		printf("dataListSize: %d\n", dataList.size());
		printf("len:%d id: %d dir:%d \tclient: %s\n", len, dirId, *fd, data_);
	}	

	close(*fd);
	delete fd;
	delete thread;
	pthread_exit(NULL);
}

void *write_(void *arg)
{
	data dataTmp;
	char s[1024];
	while (1) {
		if (dataList.size()) {
			dataTmp = dataList.front();
			strcpy(s, dataTmp.s);
			printf("send:%s\n", s);
			send(userList[dataTmp.id], s, sizeof(s), 0);
			dataList.pop();
		}
	}
	pthread_exit(NULL);
}

int main()
{
	userList.insert(pair<int, int>(1, 0));
	userList.insert(pair<int, int>(2, 0));
	userList.insert(pair<int, int>(3, 0));	
	
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in server_ad;
	server_ad.sin_family = AF_INET;
	server_ad.sin_port = htons(8080);
	server_ad.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	printf("%d\n", bind(server_fd, (struct  sockaddr*)&server_ad, sizeof(server_ad)));
	listen(server_fd, 10); 
	int *client_fd;
	sockaddr_in client_ad;
	socklen_t len = sizeof(client_ad);
	char s[1024];
	message mes;
	pthread_t *thread01;
	pthread_t thread02;
	pthread_create(&thread02, NULL, write_, NULL);
	while (1) {
		client_fd = new int;
		*client_fd = accept(server_fd, (struct sockaddr*)&client_ad, &len);
		recv(*client_fd, s, sizeof(s), 0);
		send(*client_fd, "test\0", 1024, 0);
		int id = s[0] - '0';
		printf("%s id = %d\n", s, id);
		userList[id] = *client_fd;
		thread01 = new pthread_t;
		mes.thread = thread01;
		mes.fd = client_fd;
		printf("%d\n", *client_fd);
		pthread_create(thread01, NULL, read_, &mes);
		for (map<int,int>::iterator it = userList.begin(); it != userList.end(); it++)
			cout << "id:" << it->first << ' ' << "fd:" << it->second << endl;
	}
		
	close(server_fd);

	return 0;
}
