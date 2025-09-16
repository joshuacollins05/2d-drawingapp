#include <iostream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <csignal>
#include <wait.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void fireman(int) {
    while (waitpid(-1, nullptr, WNOHANG) > 0);
}

void drawCanvas(int width, int height, const std::vector<std::pair<char, std::vector<int>>>& shapes) {
    std::vector<std::vector<char>> canvas(height, std::vector<char>(width, ' '));
    for (const auto& shape : shapes) {
        char symbol = shape.first;
        for (int pos : shape.second) {
            int row = pos / width;
            int col = pos % width;
            if (row < height && col < width) {
                canvas[row][col] = symbol;
            }
        }
    }
    for (const auto& row : canvas) {
        for (char c : row) {
            std::cout << c;
        }
        std::cout << std::endl;
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 5);

    signal(SIGCHLD, fireman);

    while (true) {
        new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
        if (fork() == 0) {
            close(server_fd);
            char buffer[BUFFER_SIZE] = {0};
            read(new_socket, buffer, BUFFER_SIZE);
            std::vector<std::pair<char, std::vector<int>>> shapes;
            int width, height;
            sscanf(buffer, "%d %d", &width, &height);
            char *token = strtok(buffer, "\n");
            while ((token = strtok(nullptr, ",")) != nullptr) {
                char symbol;
                int start, end;
                sscanf(token, " %c %d %d", &symbol, &start, &end);
                std::vector<int> positions;
                for (int i = start; i <= end; ++i) {
                    positions.push_back(i);
                }
                shapes.emplace_back(symbol, positions);
            }
            drawCanvas(width, height, shapes);
            close(new_socket);
            exit(0);
        }
        close(new_socket);
    }
    return 0;
}
