#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <ctime>
#include <string>
#include <iostream>
#include <pthread.h>
#include <fcntl.h>

#define SERVER_PORT 5000
#define BUFFER_SIZE 5
#define QUEUE_SIZE 5

class Message {

    private:
     int timestamp;
     std::string lena;

    public:
     Message(std::string input_lena) {
        this->lena = input_lena;
        this->timestamp = std::time(0);
     }
     Message() {}
     int get_timestamp() {
        return this->timestamp;
     }
     std::string get_lena() {
        return this->lena;
     }
};

class History {
    private:
     int newest_index;
     Message* message_list;

    public:
     History() {
        this->newest_index = 0;
        this->message_list = new Message[100];
     }
     int get_newest_index() {
        return this->newest_index;
     }
     Message get_message(int index) {
        return this->message_list[index];
     }
     void insert_message(Message new_message) {
        this->newest_index += 1;
        this->newest_index = (this->newest_index%100);
        this->message_list[this->newest_index] = new_message;
     }
};

History *history;

     void *work(void* arg) {
        int pSocket, pClientSocket;
        int pBind, pListen;
        int pFoo = 1;
        socklen_t pTmp;
        struct sockaddr_in stAddr_p, stClientAddr_p;

        /* address structure */
        memset(&stAddr_p, 0, sizeof(struct sockaddr));
        stAddr_p.sin_family = AF_INET;
        stAddr_p.sin_addr.s_addr = htonl(INADDR_ANY);
        stAddr_p.sin_port = htons(SERVER_PORT + 1);

        /* create a socket */
        pSocket = socket(AF_INET, SOCK_STREAM, 0);
        setsockopt(pSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&pFoo, sizeof(pFoo));

        /* bind a name to a socket */
        pBind = bind(pSocket, (struct sockaddr*)&stAddr_p, sizeof(struct sockaddr));

        /* specify queue size */
        pListen = listen(pSocket, QUEUE_SIZE);

        while(1)
        {
            /* block for connection request */
            pTmp = sizeof(struct sockaddr);
            pClientSocket = accept(pSocket, (struct sockaddr*)&stClientAddr_p, &pTmp);

            printf("[history request from: %s]\n", inet_ntoa((struct in_addr)stClientAddr_p.sin_addr));

            for (int i = 0; i < 100; i++) {
                const void* buffer = history->get_message(i).get_lena().append("\n").c_str();
                write(pClientSocket, buffer, history->get_message(i).get_lena().append("\n").length());
            }

            close(pClientSocket);
        }

        close(pSocket);
     }

int main() {

   int nSocket, nClientSocket;
   int nBind, nListen;
   int nFoo = 1;
   socklen_t nTmp;
   struct sockaddr_in stAddr, stClientAddr;
   pthread_t t1;

   history = new History();
   for (int i = 0; i < 100; i++) {
           Message* new_lena = new Message("");
           history->insert_message(*new_lena);
   }

   pthread_create(&t1, NULL, work, (void*) "I am not important");

   /* address structure */
   memset(&stAddr, 0, sizeof(struct sockaddr));
   stAddr.sin_family = AF_INET;
   stAddr.sin_addr.s_addr = htonl(INADDR_ANY);
   stAddr.sin_port = htons(SERVER_PORT);

   /* create a socket */
   nSocket = socket(AF_INET, SOCK_STREAM, 0);
   setsockopt(nSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&nFoo, sizeof(nFoo));

   /* bind a name to a socket */
   nBind = bind(nSocket, (struct sockaddr*)&stAddr, sizeof(struct sockaddr));

   /* specify queue size */
   nListen = listen(nSocket, QUEUE_SIZE);

   while(1)
   {
       /* block for connection request */
       nTmp = sizeof(struct sockaddr);
       nClientSocket = accept(nSocket, (struct sockaddr*)&stClientAddr, &nTmp);

           printf("[connection from: %s]\n", inet_ntoa((struct in_addr)stClientAddr.sin_addr));
           std::string lena = "";
           char buffer[BUFFER_SIZE];
           bool keep_reading = true;
           while(keep_reading) {
                int n = read(nClientSocket, buffer, BUFFER_SIZE);
                lena.append(buffer, n);
                if (n < BUFFER_SIZE) {
                    keep_reading = false;
                }
           }
           Message* new_lena = new Message(lena);
           history->insert_message(*new_lena);
           close(nClientSocket);
    }

   close(nSocket);
   return(0);
}
