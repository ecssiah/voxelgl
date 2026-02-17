#pragma once

#include <fstream>
#include <sstream>
#include <iostream>


namespace file_utils
{

static std::string load_text_file(const char* filename)
{
    std::ifstream file(filename, std::ios::in);

    if (!file.is_open())
    {
        std::cerr << "[FILE ERROR] Could not open " << filename << "\n";

        return {};
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

}