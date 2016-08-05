#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <vector>
#include <list>

#define BUFSIZE 1024

int main()
{
	std::list<int> fd_list;
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	int new_fd;
	struct sockaddr_in server_addr, client_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8080);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	printf("bind:%d\n", bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)));
	printf("listen:%d\n", listen(server_fd, 20));
	
    fd_set fdsr;
	struct timeval tv;
	tv.tv_sec = 30;
	tv.tv_usec = 0;
    int maxfd = server_fd;
	char buf[BUFSIZE];
    socklen_t len = sizeof(client_addr);
    while (1) {
        FD_ZERO(&fdsr);
        FD_SET(server_fd, &fdsr);
        for (std::list<int>::iterator it = fd_list.begin(); it != fd_list.end(); it++) {
            if (*it != 0) {
                FD_SET(*it, &fdsr);
            }
        }    
        int ret = select(maxfd+1, &fdsr, NULL, NULL, &tv);
        if (ret < 0) {
            perror("select");
            break;
        } else if (ret == 0) {
            printf("timeout\n");
            continue;
        }
        for (std::list<int>::iterator it = fd_list.begin(); it != fd_list.end(); it++) {
            if (FD_ISSET(*it, &fdsr)) {
                int ret = recv(*it, buf, BUFSIZE, 0);
                if (ret <= 0) {
                    printf("client_fd:%d close\n", *it);
                    close(*it);
                    it = fd_list.erase(it);
                } else {
                    printf("client_fd:%s\n");
                }
            }
        }
        if (FD_ISSET(server_fd, &fdsr)) {
            memset(&client_addr, 0, sizeof(client_addr));
            new_fd = accept(server_fd, (struct sockaddr*)&client_addr, &len);
            fd_list.push_back(new_fd);
            if (maxfd < new_fd)
                maxfd = new_fd;
        }
    }

    close(server_fd);
    return 0;
}
