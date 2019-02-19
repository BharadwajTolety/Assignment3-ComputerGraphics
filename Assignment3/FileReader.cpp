#include "FileReader.h"
#include <sstream>
#include <fstream>

std::string FileReader::ReadFile(const std::string& _path)
{
    std::ifstream f(_path, std::ios::in);
    if (!f.is_open())
    {
        printf("[FAIL]: file loading failed.\n");
        return "";
    }
    std::string content;
    std::string line = "";
    while (!f.eof())
    {
        std::getline(f, line);
        content.append(line+"\n");
    }
    content.append("\0");
    f.close();
    return content;
}
