#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main()
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8080);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	printf("connect: %d\n", connect(fd, (struct sockaddr*)&addr, sizeof(addr)));
	char buf[1024] = {0};
	while (strcmp(buf, "exit")) {
		gets(buf);
		printf("%s\n", buf);
		send(fd, buf, sizeof(buf), 0);
	}
	close(fd);

	return 0;
}
