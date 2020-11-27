#include "helpers.h"

namespace En3rN
{
    namespace Net
    {
        namespace Helpers
        {
            
            std::vector<std::string> Split(std::string& line, char delimitter)
            {
                /*size_t pos = line.find("\r\n", 0);
                if (pos != line.npos)
                    line.erase(pos, 2);*/

                std::vector<std::string> parsedBuf;
                size_t pos = 0;
                size_t pos2;
                do
                {
                    pos2 = line.find(delimitter, pos);
                    if (pos2 == line.npos)
                        pos2 = line.size();
                    parsedBuf.push_back(line.substr(pos, pos2 - pos));
                    pos = pos2 + 1;

                } while (pos != line.size() + 1);

                return parsedBuf;
            }

            std::string Filter(std::string& line, char delimitter)
            {
                std::string parsedBuf;
                size_t pos = 0;
                size_t pos2;
                do
                {
                    pos2 = line.find(delimitter, pos);
                    if (pos2 == line.npos)
                        pos2 = line.size();
                    parsedBuf.append(line.substr(pos, pos2 - pos));
                    pos = pos2 + 1;

                } while (pos != line.size() + 1);

                return parsedBuf;
            }

            std::string Join(const std::vector<std::string>& vec, char delimitter)
            {
                std::string s;
                for (auto item : vec)
                    s += item + delimitter;
                return s;
            }


            std::string getDir()
            {
                char* buf{};
                std::string s;
                if (_getcwd(buf, FILENAME_MAX) != 0)
                    buf == 0 ? s = "errFindingPath" : s = buf;
                else
                    s = "errFindingPath";
                return s;
            }
            // trim from start (in place)
            void ltrim(std::string& s)
            {
                s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch)
                    {
                        return !(std::isspace(ch) || ch == '\0');
                    }));
            }

            // trim from end (in place)
            void rtrim(std::string& s)
            {
                s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch)
                    {
                        return !(std::isspace(ch) || ch == '\0');
                    }).base(), s.end());
            }

            // trim from both ends (in place)
            void trim(std::string& s)
            {
                ltrim(s);
                rtrim(s);
            }

            // trim from start (copying)
            std::string ltrim_copy(std::string s)
            {
                ltrim(s);
                return s;
            }

            // trim from end (copying)
            std::string rtrim_copy(std::string s)
            {
                rtrim(s);
                return s;
            }

            // trim from both ends (copying)
            std::string trim_copy(std::string s)
            {
                trim(s);
                return s;
            }
            std::string GenerateKey()
            {
                std::string key;
                srand(time(NULL));
                for (int i = 0; i < 50; i++)
                {
                    char c = (unsigned char)(rand() % 94) + 32;
                    key += c;
                }
                return std::move(key);
            }
        }
    }
}

