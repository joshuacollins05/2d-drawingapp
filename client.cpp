#include <iostream>
#include <vector>
#include <sstream>
#include <pthread.h>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

// Struct to hold row decoding parameters
struct ThreadData {
    int row;
    int width;
    std::vector<int> headPos;
    std::vector<int> dataPos;
    std::vector<std::string> symbols;
    std::vector<std::vector<int>> ranges;
    char* output;
    std::string hostname;
    int port;
};

// Function to check if x is in dataPos
bool isDefined(int x, int low, int high, const std::vector<int>& dataPos) {
    while (low <= high) {
        int mid = (low + high) / 2;
        if (dataPos[mid] == x) return true;
        else if (dataPos[mid] > x) high = mid - 1;
        else low = mid + 1;
    }
    return false;
}

// Function to communicate with the server and decode the row
void* decodeRow(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);
    
    // Create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "ERROR opening socket\n";
        pthread_exit(nullptr);
    }

    // Get server address
    struct hostent* server = gethostbyname(data->hostname.c_str());
    if (!server) {
        std::cerr << "ERROR, no such host\n";
        pthread_exit(nullptr);
    }

    struct sockaddr_in serv_addr;
    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char*)server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(data->port);

    // Connect to server
    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "ERROR connecting to server\n";
        pthread_exit(nullptr);
    }

    // Send row number
    write(sockfd, &data->row, sizeof(int));

    // Send image width
    write(sockfd, &data->width, sizeof(int));

    // Send headPos array
    int headSize = data->headPos.size();
    write(sockfd, &headSize, sizeof(int));
    write(sockfd, data->headPos.data(), headSize * sizeof(int));

    // Send dataPos array
    int dataSize = data->dataPos.size();
    write(sockfd, &dataSize, sizeof(int));
    write(sockfd, data->dataPos.data(), dataSize * sizeof(int));

    // Receive decoded row
    read(sockfd, data->output, data->width);
    data->output[data->width] = '\0';

    close(sockfd);
    pthread_exit(nullptr);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <hostname> <port>\n";
        return 1;
    }

    std::string hostname = argv[1];
    int port = atoi(argv[2]);

    int width, height;
    std::cin >> width >> height;
    std::cin.ignore();

    std::vector<int> headPos, dataPos;
    std::vector<std::string> symbols;
    std::vector<std::vector<int>> ranges;

    // Read symbols and their ranges
    std::string line;
    std::getline(std::cin, line);
    std::stringstream ss(line);
    std::string part;

    while (std::getline(ss, part, ',')) {
        std::stringstream ps(part);
        std::string symbol;
        int start, end;
        ps >> symbol;
        std::vector<int> range;
        while (ps >> start >> end) {
            range.push_back(start);
            range.push_back(end);
        }
        symbols.push_back(symbol);
        ranges.push_back(range);
    }

    // Read headPos
    std::getline(std::cin, line);
    ss.clear();
    ss.str(line);
    int num;
    while (ss >> num) headPos.push_back(num);

    // Read dataPos
    std::getline(std::cin, line);
    ss.clear();
    ss.str(line);
    while (ss >> num) dataPos.push_back(num);

    pthread_t threads[height];
    ThreadData data[height];
    char output[height][width + 1];

    // Create threads
    for (int i = 0; i < height; i++) {
        data[i] = {i, width, headPos, dataPos, symbols, ranges, output[i], hostname, port};
        pthread_create(&threads[i], nullptr, decodeRow, &data[i]);
    }

    // Join threads and print output
    for (int i = 0; i < height; i++) {
        pthread_join(threads[i], nullptr);
        std::cout << output[i] << std::endl;
    }

    return 0;
}
