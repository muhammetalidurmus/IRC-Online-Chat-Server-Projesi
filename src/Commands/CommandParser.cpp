#include "../../include/Commands.hpp"

// String başındaki ve sonundaki boşlukları temizleyen fonksiyon
string strim(const string &str)
{
    size_t first = str.find_first_not_of(' ');
    if (string::npos == first)
    {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

// String başındaki ve sonundaki boşlukları temizleyen fonksiyon (diğer bir implementasyon)
string atrim(const std::string &str)
{
    const std::string whitespace = " \t\n\r\f\v";
    size_t start = str.find_first_not_of(whitespace);
    if (start == std::string::npos)
        return ""; // String sadece beyaz boşluktan oluşuyorsa
    size_t end = str.find_last_not_of(whitespace);
    return str.substr(start, end - start + 1);
}

// Vector<string> elemanlarını birleştiren fonksiyon
string mergeString(const vector<string> &parts, const string &delimiter)
{
    string result;
    for (size_t i = 0; i < parts.size(); ++i)
    {
        if (i > 0)
        {
            result += delimiter;
        }
        result += parts[i];
    }
    return result;
}

// String'i belirli bir ayraçla bölen ve parçalara ayıran fonksiyon
vector<string> splitString(string str, const string &delimiter)
{
    vector<string> parts;
    size_t pos = 0;
    string token;
    while ((pos = str.find(delimiter)) != string::npos)
    {
        token = str.substr(0, pos);
        parts.push_back(token);
        str.erase(0, pos + delimiter.length());
    }
    parts.push_back(str);
    return parts;
}

// CommandParser sınıfının komutları işleyen fonksiyonu
void CommandParser::commandParser(const char *command, Client *client, Server *srv)
{
    string commandString(command);
    if (commandString.empty())
    {
        return;
    }

    // Satır sonu karakterlerini temizle
    commandString.erase(std::remove(commandString.begin(), commandString.end(), '\n'), commandString.end());

    vector<string> commandParts = splitString(commandString, " ");
    if (commandParts.empty())
    {
        return;
    }

    // Komut adını al ve büyük harfe çevir
    for (size_t i = 0; i < commandParts[0].length(); ++i)
    {
        commandParts.at(0)[i] = toupper(commandParts.at(0)[i]);
    }

    handleCommand(client, commandParts, srv);
}

// CommandParser sınıfının komutları işleyen fonksiyonu
void CommandParser::handleCommand(Client *client, vector<string> commandParts, Server *srv)
{
    // Komutları işle
    if (commandParts.size() < 1)
    {
        client->sendMessage(":" + client->getHostName() + " 461 " + client->getNickName() + " Command :Not enough parameters");
        return;
    }

    if (commandParts.at(0) == "/CAP" || commandParts.at(0) == "CAP")
        Cap::cap(client, commandParts);
    else if (commandParts.at(0) == "/PASS" || commandParts.at(0) == "PASS")
        Pass::pass(client, commandParts, srv);
    else if (commandParts.at(0) == "/NICK" || commandParts.at(0) == "NICK")
        Nick::nick(client, commandParts, srv);
    else if ((commandParts.at(0) == "/USER" || commandParts.at(0) == "USER") && client->getIsPass())
        {
            if(!client->getNickName().empty())
            User::user(client, commandParts, srv);
            else
            client->sendMessage("firstly enter nickname");
               
        }

    else if (client->getUserAuth())
    {
        if (commandParts.at(0) == "/PRIVMSG" || commandParts.at(0) == "PRIVMSG")
            PrivMsg::privMsg(client, commandParts, srv);
        else if (commandParts.at(0) == "/NOTICE" || commandParts.at(0) == "NOTICE")
            Notice::notice(client, commandParts, srv);
        else if (commandParts.at(0) == "/JOIN" || commandParts.at(0) == "JOIN")
            Join::join(client, commandParts, srv);
        else if (commandParts.at(0) == "/QUIT" || commandParts.at(0) == "QUIT")
            Quit::quit(client, commandParts, srv);
        else if (commandParts.at(0) == "/PART" || commandParts.at(0) == "PART")
            Part::part(client, commandParts, srv);
        else if (commandParts.at(0) == "/MODE" || commandParts.at(0) == "MODE")
            Mode::mode(client, commandParts, srv);
        else if (commandParts.at(0) == "/KICK" || commandParts.at(0) == "KICK")
            Kick::kick(client, commandParts, srv);
        else
        {
            client->sendMessage("Unknown command");
            return;
        }
    }
    else
    {
        client->sendMessage(commandParts.at(0) + " Unknown command or invalid permission");
        return;
    }
}
