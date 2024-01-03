#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/event.h>

#define PORT 6667
#define MAX_CLIENTS 10
#define MAX_DATA_SIZE 1024

// Soket oluşturma işlevi
int create_socket() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Soket oluşturma başarısız");
        exit(EXIT_FAILURE);
    }
    return server_socket;
}

// Soketi bağlama işlevi
void bind_socket(int server_socket) {
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Bağlama başarısız");
        exit(EXIT_FAILURE);
    }
}

// İstemcileri dinleme işlevi
void listen_for_clients(int server_socket) {
    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("Dinleme başarısız");
        exit(EXIT_FAILURE);
    }
}

// İstemci kabul işlevi
int accept_client(int server_socket) {
    int client_socket = accept(server_socket, nullptr, nullptr);
    if (client_socket == -1) {
        perror("Kabul başarısız");
        exit(EXIT_FAILURE);
    }
    return client_socket;
}

// Soketi blok olmayan hale getirme işlevi
void set_non_blocking(int socket_fd) {
    int flags = fcntl(socket_fd, F_GETFL, 0);
    if (flags == -1) {
        perror("Fcntl başarısız");
        exit(EXIT_FAILURE);
    }

    if (fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("Fcntl başarısız");
        exit(EXIT_FAILURE);
    }
}

int main() {
    // Soket oluştur
    int server_socket = create_socket();
    // Soketi bağla
    bind_socket(server_socket);
    // İstemci bağlantılarını dinle
    listen_for_clients(server_socket);

    // Kuyruk oluştur
    int kq = kqueue();
    if (kq == -1) {
        perror("Kuyruk oluşturma başarısız");
        exit(EXIT_FAILURE);
    }

    // Sunucu soketini olay kuyruğuna ekle
    struct kevent evSet;
    EV_SET(&evSet, server_socket, EVFILT_READ, EV_ADD, 0, 0, nullptr);
    if (kevent(kq, &evSet, 1, nullptr, 0, nullptr) == -1) {
        perror("Kevent başarısız");
        exit(EXIT_FAILURE);
    }

    // Olaylar için bellekte yer ayır
    struct kevent events[MAX_CLIENTS];

    while (true) {
        // Olayları bekleyin
        int nev = kevent(kq, nullptr, 0, events, MAX_CLIENTS, nullptr);
        if (nev == -1) {
            perror("Kevent başarısız");
            exit(EXIT_FAILURE);
        }

        // Olayları işleyin
        for (int i = 0; i < nev; ++i) {
            if (events[i].ident == server_socket) {
                // Yeni istemci bağlantısı
                int client_socket = accept_client(server_socket);
                // İstemci soketini blok olmayan hale getir
                set_non_blocking(client_socket);

                // İstemci soketini olay kuyruğuna ekle
                struct kevent evSet;
                EV_SET(&evSet, client_socket, EVFILT_READ, EV_ADD, 0, 0, nullptr);
                if (kevent(kq, &evSet, 1, nullptr, 0, nullptr) == -1) {
                    perror("Kevent başarısız");
                    exit(EXIT_FAILURE);
                }

                std::cout << "Yeni istemci bağlandı: " << client_socket << std::endl;
            } else {
                // İstemciden gelen verileri işle
                char buffer[MAX_DATA_SIZE];
                int bytesRead = recv(events[i].ident, buffer, sizeof(buffer), 0);

                if (bytesRead <= 0) {
                    // Bağlantı kapandı veya hata
                    close(events[i].ident);
                    std::cout << "İstemci bağlantısı kesildi: " << events[i].ident << std::endl;
                } else {
                    // Alınan verileri işle (basitleştirilmiş, sadece yazdır)
                    buffer[bytesRead] = '\0';
                    std::cout << "İstemciden alındı " << events[i].ident << ": " << buffer << std::endl;
                }
            }
        }
    }

    // Sunucu soketini kapat
    close(server_socket);
    return 0;
}
