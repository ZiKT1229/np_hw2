#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>

typedef struct sockaddr *sockaddrp;
int sockfd;

void *recv_other(void *arg) {
    char buf[255]= {};
    while (1) {
        int ret = recv(sockfd, buf, sizeof(buf), 0);
        if (ret < 0) {
            perror("recv");
            return;
        }
        printf("%s\n", buf);
    }
}

int main(int argc, char **argv) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in addr = {AF_INET};
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;

    socklen_t addr_len = sizeof(addr);

    int ret = connect(sockfd,(sockaddrp)&addr,addr_len);
    if (ret < 0) {
        perror("connect");
        return -1;
    }

    char buf[255] = {};
    char name[20] = {};
    printf("Please input your username： ");
    fgets(name, 20, stdin);
    if (name[strlen(name) - 1] == '\n') {
        name[strlen(name) - 1] = '\0'; 
    }

    ret = send(sockfd, name, strlen(name), 0);
    if (ret < 0) {
        perror("send");
        return -1;
    }

    pthread_t tid; //負責接收訊息
    ret = pthread_create(&tid,NULL,recv_other,NULL);
    if (ret < 0) {
        perror("pthread_create");
        return -1;
    }

    while (1) {
        fgets(buf, 255, stdin);
        if (buf[strlen(buf) - 1] == '\n') {
            buf[strlen(buf) - 1] = '\0'; 
        }
        
        ret = send(sockfd, buf, strlen(buf), 0);
        if (ret < 0) {
            perror("send");
            return -1;
        }

        if(strcmp("quit", buf) == 0) {
            printf("%s, you already leave\n", name);
            break;
        }
    }

    return 0;
}