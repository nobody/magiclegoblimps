#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>

#include <boost/asio.hpp>

#include "../protocol.h"

int main() {
    // let's create some robots to send to the server
    robotInit* robots = new robotInit[3];

    robots[0].RID = 0;
    robots[0].x = 0;
    robots[0].y = 9;
    robots[0].VideoURL = new std::string("http://10.10.1.100:7500");

    robots[1].RID = 1;
    robots[1].x = 1;
    robots[1].y = 8;
    robots[1].VideoURL = new std::string("http://10.10.1.101:7500");

    robots[2].RID = 2;
    robots[2].x = 2;
    robots[2].y = 7;
    robots[2].VideoURL = new std::string("http://10.10.1.102:7500");

    byteArray arr;
    if (write_data(P_ROBOT_INIT, robots, 3, &arr) < 0) {
        std::cerr << "Failed to create write array\n";
        return -1;
    }

    robotUpdate* update = new robotUpdate[3];

    update[0].RID = 0;
    update[0].x = 1;
    update[0].y = 9;
    update[0].listSize = 1;
    update[0].objects = new int[1];
    update[0].objects[0] = 15;
    update[0].qualities = new int[1];
    update[0].qualities[0] = 1;

    update[1].RID = 1;
    update[1].x = 2;
    update[1].y = 8;
    update[1].listSize = 2;
    update[1].objects = new int[2];
    update[1].objects[0] = 15;
    update[1].objects[1] = 9;
    update[1].qualities = new int[2];
    update[1].qualities[0] = 2;
    update[1].qualities[1] = 3;

    update[2].RID = 2;
    update[2].x = 3;
    update[2].y = 7;
    update[2].listSize = 3;
    update[2].objects = new int[3];
    update[2].objects[0] = 15;
    update[2].objects[1] = 9;
    update[2].objects[2] = 5;
    update[2].qualities = new int[3];
    update[2].qualities[0] = 1;
    update[2].qualities[1] = 2;
    update[2].qualities[2] = 5;

    byteArray updArr;
    if (write_data(P_ROBOT_UPDATE, update, 3, &updArr)) {
        std::cerr << "Failed to create update array\n";
        return -1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock == -1) {
        std::cerr << "Failed to create socket\n";
        return -1;
    }

    struct hostent *hostPtr = NULL;
    struct sockaddr_in serverName = { 0 };

    //in_addr_t ip = inet_addr("10.176.14.94");
    //hostPtr = gethostbyaddr(&ip, 4, AF_INET);
    hostPtr = gethostbyname("localhost");


    serverName.sin_family = AF_INET;
    serverName.sin_port = htons(9999);
    memcpy(&serverName.sin_addr, hostPtr->h_addr, hostPtr->h_length);

    int conn = connect(sock, (struct sockaddr*)&serverName, sizeof(serverName));

    if (conn == -1) {
        std::cerr << "Failed to connect()\n";
        return -1;
    }

    conn = write(sock, arr.array, arr.size);
    std::cout << "Wrote robot data to socket:\n";
    for (int i = 0; i < arr.size; ++i) {
        printf("%02X ", arr.array[i]);
    }
    std::cout << std::endl;

    sleep(1);

    conn = write(sock, updArr.array, updArr.size);
    std::cout << "Wrote update data to socket:\n";
    for (int i = 0; i < updArr.size; ++i) {
        printf("%02X ", updArr.array[i]);
    }
    std::cout << std::endl;
    char buffer[1];

    while (0 < (conn = read(sock, buffer, 1))) {
        printf("%02X ", (unsigned)*buffer);
        fflush(stdout);
    }
    std::cout << std::endl;

    if (-1 == conn)
            perror("read()");

    close(sock);
    return 0;
}
