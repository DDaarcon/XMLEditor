#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <iostream>
#include <string>
#include <vector>
#include "units.hpp"

namespace Units {
	class Interface {
	public:
		std::string mainProlog;

		Unit* mainUnit = nullptr;
		std::vector<Unit*> memory;

	public:
		~Interface() {
			if (mainUnit != nullptr) delete mainUnit;
			for (auto it = memory.begin(); it != memory.end();) {
				delete *it;
				memory.erase(it);
			}
		}

		bool ReadCommand(std::string&);
		Unit* ReadAddress(std::string&);

	private:
		Unit* FindById(int, Unit* searchIn = nullptr) const;

		std::string GetSentence(std::string&, bool hideQuotesIfFound = false) const;

		bool CommandReadingFinished() const {
			std::cout << std::endl;
			return true;
		}

		void FillTemplate(Unit*, std::string&);


	};
}


#endif