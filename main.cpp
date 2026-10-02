#include <iostream>
#include <string>
#include <sstream>

#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

int main() {

    // 1.set socket
    int server_fd = socket(
        AF_INET, //ipv4
        SOCK_STREAM, //tcp
        0
    );
    if(server_fd < 0) {
        perror("socket error");
        return 1;
    }

    // 2.get addr
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; //このマシンが持っているすべてのipv4アドレス
    serverAddr.sin_port = htons(8080);

    if(bind(
            server_fd,
            reinterpret_cast<sockaddr*>(&serverAddr),
            sizeof(serverAddr)
        ) < 0
    ) {
        perror("bind error");
        close(server_fd);
        return 1;
    }

    if(listen(server_fd, 10) < 0) {
        perror("listen error");
        close(server_fd);
        return 1;
    }

    cout << "Server started on port 8080" << endl;

    int client_fd = accept(
        server_fd,
        nullptr,
        nullptr
    );

    if(client_fd < 0) {
        perror("client accpet error");
        close(server_fd);
        close(client_fd);
        return 1;
    }

    char buffer[4096];

    int n = recv(
        client_fd,
        buffer,
        sizeof(buffer),
        0
    );

    if(n < 0) {
        perror("recv error");
        close(server_fd);
        close(client_fd);
        return 1;
    }

    string request(buffer, n);

    cout << "----- Request -----\n";
    cout << request;
    cout << "-------------------\n";

    istringstream iss(request);

    string method;
    string path;
    string version;

    iss >> method >> path >> version;

    cout << "Method: " << method << endl;
    cout << "Path: " << path << endl;
    cout << "Version: " << version << endl;

    string body = "hello world!\n";

    string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: " +
        std::to_string(body.size()) +
        "\r\n"
        "Connection: close\r\n"
        "\r\n" +
        body;

    send(
        client_fd,
        response.c_str(),
        response.size(),
        0
    );

    close(server_fd);
    close(client_fd);

    return 0;

}