//IRC üzerinde dosya transferi gerçekleştirmek için C++ programı yazmak oldukça karmaşık olabilir, 
//çünkü bu, IRC'nin DCC protokolünü anlamayı ve kullanmayı gerektirir. Ancak, aşağıda basit bir örnek C++ kodu bulunmaktadır.
// Bu örnek, bir DCC sunucu oluşturur ve bir dosyayı karşı tarafa gönderir.

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 12345

void sendFile(const std::string& filename, int clientSocket) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Dosya açılamadı: " << filename << std::endl;
        return;
    }

    char buffer[1024];
    while (!file.eof()) {
        file.read(buffer, sizeof(buffer));
        send(clientSocket, buffer, file.gcount(), 0);
    }

    file.close();
}

int main() {
    srand(time(NULL));

    // DCC sunucu için soket oluştur
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Soket oluşturulamadı." << std::endl;
        return -1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    // Soketi bağla
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Soket bağlanamadı." << std::endl;
        close(serverSocket);
        return -1;
    }

    // Bağlantıları dinle
    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Bağlantı dinlenemiyor." << std::endl;
        close(serverSocket);
        return -1;
    }

    std::cout << "DCC sunucu başlatıldı, port: " << PORT << std::endl;

    while (true) {
        // Bağlantıyı kabul et
        int clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == -1) {
            std::cerr << "Bağlantı kabul edilemedi." << std::endl;
            continue;
        }

        // Dosya gönder
        std::string filename = "gonderilecek_dosya.txt";  // Gönderilecek dosyanın adını değiştirebilirsiniz
        sendFile(filename, clientSocket);

        // Bağlantıyı kapat
        close(clientSocket);
    }

    // Sunucu soketini kapat
    close(serverSocket);

    return 0;
}
