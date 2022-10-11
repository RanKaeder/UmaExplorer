#pragma once
#include <string>
#include <vector>
#include <utility>

namespace mdb
{
	void init();
	void unload();

	std::string find_text(int category, int index);
	std::vector<std::pair<int, std::string>> get_uma_all();
	int get_head_id_from_dress_id(int dressId);
	bool get_dress_have_mini(int dressId);
}
