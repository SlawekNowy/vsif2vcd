#pragma once 
#include <pch.hpp>


namespace VSIF {
	class CStringPool {
	private:
		std::vector<std::string> pool;

	public:
		std::string getStringByID(unsigned short ID);
		unsigned short findOrAddString(std::string stringToPool);
		void prepareToMultiInsert(int howMuch);

	};
	inline std::string CStringPool::getStringByID(unsigned short ID)
	{
		if (ID >= pool.size()) {
			return std::to_string(ID);
		}
		return pool.at(ID); //THIS MIGHT CRASH!
	}

	inline unsigned short CStringPool::findOrAddString(std::string stringToPool)
	{
		//first check if we have that element
		std::vector<std::string>::iterator it = std::find(pool.begin(), pool.end(), stringToPool);

		if (it != pool.end())
			return std::distance(pool.begin(), it);

		//Okay. This isn't present in the pool. Add this.
		pool.push_back(stringToPool);
		it = std::find(pool.begin(), pool.end(), stringToPool);
		//no check for invalid ID since we just inserted it. Just get ID.
		return std::distance(pool.begin(), it);
	}

	inline void CStringPool::prepareToMultiInsert(int howMuch)
	{
		pool.reserve(howMuch);
	}
}
