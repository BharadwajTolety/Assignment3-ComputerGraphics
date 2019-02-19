#ifndef FILEREADER_H
#define FILEREADER_H

#include <string>

class FileReader
{
public:
    static std::string ReadFile(const std::string& _path);
};

#endif // FILEREADER_H
