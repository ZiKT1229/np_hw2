#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct sockaddr *sockaddrp;

struct sockaddr_in src_addr[10];
socklen_t src_len = sizeof(src_addr[0]);

int confd[10] = {};
int count = 0; //連線人數
char people[10][20] = {};

void *broadcast(void *indexp) {
    int index = *(int *)indexp;
    char buf_rcv[255] = {};
    char buf_snd[255] = {};

    char name[20] = {}; //username
    int ret = recv(confd[index], name, sizeof(name), 0); //接收訊息
    if(ret < 0) {
        perror("recv");
        close(confd[index]);
        return;
    }

    strcpy(people[index], name); //存放進來的username

    while (1) {
        bzero(buf_rcv, sizeof(buf_rcv));
        recv(confd[index], buf_rcv, sizeof(buf_rcv),0);

        if(strcmp("quit", buf_rcv) == 0) { //離開聊天室
            sprintf(buf_snd, "%s leave the chatroom", name);
            for(int i = 0; i <= count; i++) {
                if(index == i || confd[i] == 0) {
                    continue;
                }
                send(confd[i], buf_snd, strlen(buf_snd), 0);
            }
            confd[index] = -1;
            pthread_exit(0);
        } else if (strcmp("who", buf_rcv) == 0) { //查看線上user
            char allName[200] = {};
            for(int i = 0; i < count; i++) {
                if (i > 0) {
                    strcat(allName, ", ");
                }
                strcat(allName, people[i]);
            }
            sprintf(buf_snd, "Current user: %s", allName);
            send(confd[index], buf_snd, strlen(buf_snd), 0);
        } else if (strncmp("to", buf_rcv, 2) == 0) { //傳送訊息給指定對象
            char * pch;
            pch = strtok (buf_rcv," ");
            pch = strtok (NULL, " ");
            char target[20] = {};
            strcpy(target, pch);
            pch = strtok (NULL, " ");
            char msg[255] = {};
            while (pch != NULL) {
                strcat(msg, pch);
                strcat(msg, " ");
                pch = strtok (NULL, " ");
            }
            sprintf(buf_snd, "%s:%s", name, msg);
            printf("to %s:%s\n", target, msg);
            for(int i = 0; i < count; i++) {
                if (strcmp(target, people[i]) == 0) {
                    send(confd[i], buf_snd, strlen(buf_snd), 0);
                    break;
                }
            }
        } else { //傳送訊息給所有人
            sprintf(buf_snd, "%s:%s", name, buf_rcv);
            printf("%s\n", buf_snd);

            for(int i = 0; i <= count; i++) {
                if(index == i || confd[i] == 0) {
                    continue;
                }
                send(confd[i], buf_snd, sizeof(buf_snd), 0);
            }
        }
        memset(buf_snd, '\0', 255);
    }
}

int main(int argc, char **argv) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in addr = {AF_INET};
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;

    socklen_t addr_len = sizeof(addr);

    int ret = bind(sockfd, (sockaddrp)&addr, addr_len);
    if (ret < 0) {
        perror("bind");
        return -1;
    }

    listen(sockfd, 10);

    int index = 0;

    printf("Chat room is running~\n");
    while (count <= 10) {
        confd[count] = accept(sockfd, (sockaddrp)&src_addr[count], &src_len);
        ++count;
        printf("current count: %d\n", count);
        index = count-1;

        pthread_t tid;
        ret = pthread_create(&tid, NULL, broadcast, &index);
        if (ret < 0) {
            perror("pthread_create");
            return -1;
        }
    }
}