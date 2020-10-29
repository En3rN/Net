//#include "MyFDS.h"
//#include "logger.h"
//
//MyFDS& MyFDS::operator<<(SOCKET& socket)
//{
//    fd_array.push_back(socket);
//    fd_count++;
//    return *this;
//}
//
//MyFDS& MyFDS::operator=(MyFDS& other)
//{
//    this->fd_count = other.fd_count;
//    this->fd_array = other.fd_array;
//
//    return *this;
//}
//
//void MyFDS::Remove(SOCKET& socket)
//{
//    int pos = 0;
//    for (auto s : fd_array)
//    {
//        if (s == socket) 
//        {
//            if (closesocket(s) == 0)
//                logger(LogLvl::Info) << "Closing SOCKET: " << s << " [SUCCESS]";
//            else
//                logger(LogLvl::Error) << "Closing SOCKET: " << s << " [ERR:" << WSAGetLastError() << "]";
//            fd_array.erase(fd_array.begin() + pos);
//            fd_count--;
//            break;
//        }
//        pos++;
//    }
//}
//
//void MyFDS::CloseSockets()
//{
//    int pos =0 ;
//    for (auto s : fd_array)
//    { 
//        if (closesocket(s) == 0)
//            logger(LogLvl::Info) << "Closing SOCKET: " << s << " [SUCCESS]";
//        else
//            logger(LogLvl::Error) << "Closing SOCKET: " << s << " [ERR:" << WSAGetLastError() << "]";
//        fd_array.erase(fd_array.begin() + pos);
//        fd_count--;
//        pos++;
//    }
//}
