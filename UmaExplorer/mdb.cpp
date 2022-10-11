#include <codecvt>
#include <iostream>
#include <Windows.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include <vector>
#include <utility>
#include <unordered_map>


namespace mdb
{
	std::unordered_map<int, int> umaDressHeadId{};
	std::unordered_map<int, int> umaDressHasMini{};

	std::string utf8_encode(const std::wstring& in)
	{
		if (in.empty()) return std::string();
		int size = WideCharToMultiByte(CP_UTF8, 0, &in[0], in.size(), NULL, 0, NULL, NULL);
		std::string dst(size, 0);
		WideCharToMultiByte(CP_UTF8, 0, &in[0], in.size(), &dst[0], size, NULL, NULL);
		return dst;
	}

	SQLite::Database* master;

	SQLite::Database* meta;

	void init()
	{
		try
		{
			if (master != nullptr)
			{
				return;
			}
			//std::cout << "master.mdb not opened." << std::endl;

			WCHAR buffer[MAX_PATH];
			int len = GetEnvironmentVariable(L"USERPROFILE", buffer, MAX_PATH);

			std::wstring path(buffer, len);
			path += L"\\AppData\\LocalLow\\Cygames\\umamusume\\master\\master.mdb";
			master = new SQLite::Database(utf8_encode(path), SQLite::OPEN_READONLY);

			std::cout << "master.mdb opened." << std::endl;
		}
		catch (std::exception& e)
		{
			std::cout << "Exception opening master.mdb: " << e.what() << std::endl;
		}

		try
		{
			if (meta != nullptr)
			{
				return;
			}
			//std::cout << "meta not opened." << std::endl;

			WCHAR buffer[MAX_PATH];
			int len = GetEnvironmentVariable(L"USERPROFILE", buffer, MAX_PATH);

			std::wstring path(buffer, len);
			path += L"\\AppData\\LocalLow\\Cygames\\umamusume\\meta";
			meta = new SQLite::Database(utf8_encode(path), SQLite::OPEN_READONLY);

			std::cout << "meta opened." << std::endl;
		}
		catch (std::exception & e)
		{
			std::cout << "Exception opening meta: " << e.what() << std::endl;
		}

	}

	void unload()
	{
		try
		{
			if (master == nullptr)
			{
				return;
			}

			delete master;
			master = nullptr;

			if (meta == nullptr)
			{
				return;
			}

			delete meta;
			meta = nullptr;
		}
		catch (std::exception& e)
		{
			std::cout << "Exception unloading meta: " << e.what() << std::endl;
		}
	}

	std::string find_text(int category, int index)
	{
		if (master == nullptr)
		{
			return "";
		}

		try
		{
			SQLite::Statement query(*master, "SELECT text FROM text_data WHERE category = ? AND \"index\" = ?");
			query.bind(1, category);
			query.bind(2, index);

			while (query.executeStep())
			{
				std::string s = query.getColumn(0);

				return s;
			}
		}
		catch (std::exception& e)
		{
			std::cout << "Exception querying master.mdb: " << e.what() << std::endl;
		}
		return "";
	}

	std::vector<std::pair<int, std::string>> get_uma_all() {
		std::vector<std::pair<int, std::string>> result;

		if (master == nullptr)
		{
			printf("Get Uma Failed!\n");
			return result;
		}

		try
		{
			SQLite::Statement query(*master, "SELECT \"index\",text FROM text_data WHERE category = 4");

			while (query.executeStep())
			{
				int id = query.getColumn(0);
				if (id >= 9000000) {
					continue;
				}

				std::string s = query.getColumn(1);

				std::pair<int, std::string> data(id, s);

				//printf("Uma name is %s\n", s.c_str());

				result.push_back(data);

			}
		}
		catch (std::exception & e)
		{
			std::cout << "Exception querying master.mdb: " << e.what() << std::endl;
		}
		return result;

	}

	void executeQueryDress() {
		if (master == nullptr) {
			init();
		}
		try
		{
			SQLite::Statement query(*master, "SELECT id, head_sub_id, have_mini FROM dress_data");

			while (query.executeStep())
			{
				int umaDressId = query.getColumn(0);
				int umaHeadId = query.getColumn(1);
				int haveMini = query.getColumn(2);
				umaDressHeadId.emplace(umaDressId, umaHeadId);
				umaDressHasMini.emplace(umaDressId, haveMini);
			}
		}
		catch (std::exception& e)
		{
			std::cout << "Exception querying master.mdb: " << e.what() << std::endl;
		}

	}

	int get_head_id_from_dress_id(int dressId) {
		if (umaDressHeadId.empty()) {
			executeQueryDress();
		}

		if (umaDressHeadId.find(dressId) != umaDressHeadId.end()) {
			return umaDressHeadId.at(dressId);
		}
		printf("uma dressId not found: %d\n", dressId);
		return 0;
	}

	bool get_dress_have_mini(int dressId) {
		if (umaDressHasMini.empty()) {
			executeQueryDress();
		}
		if (umaDressHasMini.find(dressId) != umaDressHasMini.end()) {
			return umaDressHasMini.at(dressId);
		}
		return false;
	}

}
