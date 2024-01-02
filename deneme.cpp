#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#define MAX_CLIENTS 10
#define PORT 6667

int main() {
    int server_socket, client_sockets[MAX_CLIENTS], kq, nevents;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    struct kevent change, event[MAX_CLIENTS];

    // Socket oluşturma
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Adres bilgilerini hazırlama
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Socket'i bağlama
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Socket'i dinleme moduna geçirme
    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("listen");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Kuyruk (kqueue) oluşturma
    kq = kqueue();
    if (kq == -1) {
        perror("kqueue");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Server socket'i kuyruğa ekleme (EV_SET)
    EV_SET(&change, server_socket, EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(kq, &change, 1, NULL, 0, NULL) == -1) {
        perror("kevent");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    std::cout << "IRC Sunucusu Başlatıldı. Port: " << PORT << std::endl;

    while (true) {
        // Kuyruktaki olayları bekleyerek kontrol etme
        nevents = kevent(kq, NULL, 0, event, MAX_CLIENTS, NULL);
        if (nevents == -1) {
            perror("kevent");
            break;
        }

        // Olayları kontrol etme
        for (int i = 0; i < nevents; ++i) {
            int current_socket = event[i].ident;

            // Yeni bir bağlantı var mı?
            if (current_socket == server_socket) {
                // Yeni bir bağlantı kabul etme
                int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
                if (client_socket == -1) {
                    perror("accept");
                    continue;
                }

                // Bağlantıyı non-blocking yapma
                fcntl(client_socket, F_SETFL, O_NONBLOCK);

                // Yeni bağlantıyı kuyruğa ekleme (EV_SET)
                EV_SET(&change, client_socket, EVFILT_READ, EV_ADD, 0, 0, NULL);
                if (kevent(kq, &change, 1, NULL, 0, NULL) == -1) {
                    perror("kevent");
                    close(client_socket);
                    continue;
                }

                std::cout << "Yeni bir bağlantı kabul edildi. Socket: " << client_socket << std::endl;
            }
            else {
                // Varolan bir bağlantıdan veri alınabilir mi?
                char buffer[1024];
                ssize_t bytes_received = recv(current_socket, buffer, sizeof(buffer), 0);
                if (bytes_received <= 0) {
                    // Bağlantı kapatıldı veya bir hata oluştu
                    std::cout << "Bağlantı kapandı. Socket: " << current_socket << std::endl;
                    close(current_socket);
                    EV_SET(&change, current_socket, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                    kevent(kq, &change, 1, NULL, 0, NULL);
                } else {
                    // Alınan veriyi işleme
                    buffer[bytes_received] = '\0';
                    std::cout << "Alınan veri (" << current_socket << "): " << buffer;
                }
            }
        }
    }

    // Kuyruk ve soketleri kapatma
    close(kq);
    close(server_socket);

    return 0;
}
