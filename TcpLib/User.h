#pragma once
#include "helpers.h"
#include <iostream>
#include <fstream>
#include <time.h>
#include "logger.h"
#include <vector>
#include <filesystem>

namespace En3rN
{
	namespace Net
	{	
		enum class UserLvl { NotAuthed = 0, User = 1, PwrUser = 2, Admin = 3 };
		class User
		{

		public:

			std::string Name;
			std::string Key;
			std::string Telephone;
			UserLvl UserLevel;

			User(std::string name = "noname", std::string key = "", UserLvl lvl = UserLvl::NotAuthed) :
				Name(name), Key(key), Telephone("n/a"), UserLevel(lvl) {};
			~User();
			static std::vector<User>GetUsers();
			static User NewUser(std::string& name, UserLvl lvl);
			static std::string GenerateKey();
			static int SaveUsers(const std::vector<User>& Users);

		};
		void operator>>(std::istream& stream, User& other);
		std::ostream& operator<<(std::ostream& stream, const User& other);


	}	
}