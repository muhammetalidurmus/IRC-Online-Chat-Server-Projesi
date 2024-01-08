#include "include/Server.hpp"
#include "include/Client.hpp"
#include "include/Bot.hpp"

int main(int argc, char *argv[])
{
	if (argc != 3) {
		write(2, "Usage: ./server <port> <password>\n", 34);
		return 1;
	}

	int port = std::atoi(argv[1]);
	if (port <= 0 || port > 65535) {
		write(2, "Invalid port\n", 13);
		return 1;
	}

	std::string password = argv[2];

	try {
		Server	srv(AF_INET, SOCK_STREAM, port, "Gariban İRC");
		srv.setSrvPass(password); // Şifreyi ayarlaC
		srv.serverRun();
	}
	catch (std::exception& e) {
		write(2, e.what(), strlen(e.what()));
		return 1;
	}
	return 0;
}
