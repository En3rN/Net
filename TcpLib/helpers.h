#pragma once
#include <vector>
#include <iostream>
#include <direct.h>
#include <algorithm>
#include <locale>
#include <cctype>
#include <string>



namespace En3rN
{
    namespace Net
    {
        namespace Helpers
        {
           
            std::vector<std::string> Split(std::string& line, char delimitter);

            std::string Filter(std::string& line, char delimitter);
            
            std::string Join(const std::vector<std::string>& vec, char delimitter);
            
            std::string getDir();
            
            // trim from start (in place)
            void ltrim(std::string& s);
            

            // trim from end (in place)
            void rtrim(std::string& s);
            

            // trim from both ends (in place)
            void trim(std::string& s);
            

            // trim from start (copying)
            std::string ltrim_copy(std::string s);
            

            // trim from end (copying)
            std::string rtrim_copy(std::string s);
            

            // trim from both ends (copying)
            std::string trim_copy(std::string s);
            
        }
    }
}

        