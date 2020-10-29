#include "User.h"
#include "helpers.h"

namespace En3rN
{
	namespace Net
	{

		std::ostream& operator <<(std::ostream& stream, const User& other)
		{
			stream << other.Name << "," << other.Key << "," << other.Telephone << "," << (int)other.UserLevel;
			return stream;
		}


		void operator>> (std::istream& stream, User& other)
		{
			std::string line;
			stream >> line;
			std::vector<std::string> v = Helpers::Split(line, ',');

			switch (v.size())
			{
			case 1:
				if (v[0] == "")
					break;
				other.Name = v[0];
				break;
			case 2:
				other.Name = v[0];
				other.Key = v[1];
				break;
			case 3:
				other.Name = v[0];
				other.Key = v[1];
				other.Telephone = v[2];
				break;
			case 4:
				other.Name = v[0];
				other.Key = v[1];
				other.Telephone = v[2];
				other.UserLevel = (UserLvl)stoi(v[3]);
				break;
			default:
				logger(LogLvl::Error) << "wrong size on input" << v.size();
				break;
			}

		}

		std::ostream& operator <<(std::ostream& stream, const UserLvl& other)
		{
			stream << (int)other;
			return stream;
		}

		std::istream& operator >> (std::istream& stream, UserLvl& other)
		{

			std::string s;
			stream >> s;
			other = (UserLvl)std::stoi(s);
			return stream;
		}

		User::~User() { logger(LogLvl::Debug) << "User destructed!"; }

		std::vector<User> User::GetUsers()
		{
			User u("noname");
			std::vector<User> v;
			std::fstream fsUsers;
			fsUsers.open("users.dat");
			logger(LogLvl::Info) << "Dir: " << Helpers::getDir();

			//TODO decrypt fsusers?

			while (!fsUsers.eof())
			{
				fsUsers >> u;
				v.push_back(u);
			}
			if (v.empty())
				v.push_back(u);
			fsUsers.close();
			return v;
		}
		User User::NewUser(std::string& name, UserLvl lvl = UserLvl::User)
		{
			std::string key = GenerateKey();
			User u(name, key, lvl);
			std::fstream fsUsers;
			fsUsers.open("users.dat", std::ios::app);
			fsUsers << "\n" << u;
			fsUsers.close();
			return u;
		}

		std::string User::GenerateKey()
		{
			std::string key;
			srand(time(NULL));
			for (int i = 0; i < 50; i++)
			{
				char c = (unsigned char)(rand() % 94) + 32;
				if (c == ' ' || c == ',')
				{
					i--;
					logger(LogLvl::Info) << "Unwanted CHAR: " << c;
					continue;
				}
				key += c;
			}
			return key;
		}
		int User::SaveUsers(const std::vector<User>& Users)
		{
			if (Users.size() == 0) return -1;
			std::fstream fsUsers;
			logger(LogLvl::Info) << "Dir: " << Helpers::getDir();
			fsUsers.open("users.dat", std::ios::out);			

			fsUsers << Users[0];
			for (size_t i = 1; i < Users.size(); i++)
				fsUsers << "\n" << Users[i];
			fsUsers.close();
			return 0;

		}
	}
}
