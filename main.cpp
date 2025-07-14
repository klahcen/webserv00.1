#include "include/Server.hpp"
#include "include/ConfigParser.hpp"

int main(int argc, char **argv)
{
    std::cout << "\033[2m";
    std::cout << "__        __  _____   ____    ____    _____   ____   __     __" << std::endl;
    std::cout << "\\ \\      / / | ____| | __ )  / ___|  | ____| |  _ \\  \\ \\   / /" << std::endl;
    std::cout << " \\ \\ /\\ / /  |  _|   |  _ \\  \\___ \\  |  _|   | |_) |  \\ \\ / / " << std::endl;
    std::cout << "  \\ V  V /   | |___  | |_) |  ___) | | |___  |  _ <    \\ V /  " << std::endl;
    std::cout << "   \\_/\\_/    |_____| |____/  |____/  |_____| |_| \\_\\    \\_/   " << std::endl;

    std::string configFilePath = "configs/default.conf";
    Server S;
    if (argc == 2)
        configFilePath = argv[1];
    else if (argc > 2)
    {
        std::cerr << "Usage: " << argv[0] << " [config_file]" << std::endl;
        return (1);
    }

    try
    {
        ConfigParser parser;
        parser.parse(configFilePath);
        std::vector<ServerConfig> &servers = parser.getServers();
        signal(SIGPIPE, SIG_IGN);
        S.start_server(servers);
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return (1);
    }
    std::cout << "\033[0m";
    return (0);
}