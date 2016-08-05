#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

using namespace std;

int fd;
bool flag = true;

void *read_(void *arg)
{
	char s[1024];
	do {
		recv(fd, s, sizeof(s), 0);
		printf("other: %s\n", s);
	}while (flag);
	pthread_exit(NULL);
}

void *write_(void *arg)
{
	char s[1024] = {0};
	while (strcmp(s, "quit") && strcmp(s, "exit")) {
		scanf("%s", s);
		send(fd, s, sizeof(s), 0);
	}
	flag = false;
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	printf("%c\n", argv[1][0]);
	fd = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8080);
	addr.sin_addr.s_addr = inet_addr("127.0.01");

	connect(fd, (struct sockaddr*)&addr, sizeof(addr));
	char s[1024] = {0};
	s[0] = argv[1][0];
	send(fd, s, sizeof(s), 0);
	pthread_t thread01;
	pthread_t thread02;
	pthread_create(&thread01, NULL, write_, NULL);
	pthread_create(&thread02, NULL, read_, NULL);
	pthread_join(thread01, NULL);
	pthread_join(thread02, NULL);
	getchar();
	close(fd);

	return 0;
}
