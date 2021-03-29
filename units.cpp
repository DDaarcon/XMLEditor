#include <iostream>
#include <fstream>

#include "units.hpp"

namespace Units {

	int Unit::idCounter = 0;

	void Unit::SetValue(int value) {
		storeType = DataType::Int;
		storeValue.iValue = value;
	}
	void Unit::SetValue(std::string value) {
		storeType = DataType::String;
		sValue = value;
	}
	void Unit::SetValue(float value) {
		storeType = DataType::Float;
		storeValue.fValue = value;
	}
	bool Unit::GetValue(int& dest) const {
		if (storeType == DataType::Int) {
			dest = storeValue.iValue;
			return true;
		}
		return false;
	}
	bool Unit::GetValue(std::string& dest) const {
		if (storeType == DataType::String) {
			dest = sValue;
			return true;
		}
		return false;
	}
	bool Unit::GetValue(float& dest) const {
		if (storeType == DataType::Float) {
			dest = storeValue.fValue;
			return true;
		}
		return false;
	}

	void Unit::AddAttr(std::string name, long value) {
		// if (SearchForAttrName(name)) return;
		SearchForAttrName(name, true);
		attrsInt.push_back({name, value});
	}
	void Unit::AddAttr(std::string name, std::string value) {
		// if (SearchForAttrName(name)) return;
		SearchForAttrName(name, true);
		attrsStr.push_back({name, value});
	}
	void Unit::AddTemplateAttr(std::string name) {
		SearchForAttrName(name, true);
		attrsTemp.push_back(name);
	}
	bool Unit::ImportTemplateAttr(int value) {
		auto firstInQueue = attrsTemp.begin();
		if (firstInQueue == attrsTemp.end()) return false;
		AddAttr(*firstInQueue, value);
		return true;
	}
	bool Unit::ImportTemplateAttr(std::string value) {
		auto firstInQueue = attrsTemp.begin();
		if (firstInQueue == attrsTemp.end()) return false;
		AddAttr(*firstInQueue, value);
		return true;
	}
	bool Unit::GetAttr(std::string name, long& dest) const {
		for (Attribute<long> attr : attrsInt) {
			if (attr.name == name) {
				dest = attr.value;
				return true;
			}
		}
		return false;
	}
	bool Unit::GetAttr(std::string name, std::string& dest) const {
		for (Attribute<std::string> attr : attrsStr) {
			if (attr.name == name) {
				dest = attr.value;
				return true;
			}
		}
		return false;
	}
	void Unit::RemoveAttr(std::string name) {
		for (std::vector<Attribute<long>>::iterator it = attrsInt.begin(); it != attrsInt.end() ; it++) {
			if (it->name == name) {
				attrsInt.erase(it);
				return;
			}
		}
		for (std::vector<Attribute<std::string>>::iterator it = attrsStr.begin(); it != attrsStr.end() ; it++) {
			if (it->name == name) {
				attrsStr.erase(it);
				return;
			}
		}
	}

	Unit* Unit::AddChild() {
		Unit* unit = new Unit();
		return AddChild(unit);
	}
	Unit* Unit::AddChild(Unit* unit) {
		unit->parent = this;
		children.push_back(unit);

		return unit;
	}
	Unit* Unit::GetChild(int id) const {
		for (auto it = children.begin(); it != children.end(); it++) {
			if ((*it)->id == id) return *it;
		}
		return nullptr;
	}
	int Unit::GetChildIndex(int id) const {
		for (auto it = children.begin(); it != children.end(); it++) {
			if ((*it)->id == id) return std::distance(children.begin(), it);
		}
		return -1;
	}
	Unit* Unit::GetChild(std::string name) const {
		for (auto it = children.begin(); it != children.end(); it++) {
			if ((*it)->name == name) return *it;
		}
		return nullptr;
	}
	Unit* Unit::GetChildAt(int index) const {
		if (index >= children.size()) return nullptr;
		else return children[index];
	}
	void Unit::RemoveChild(int id) {
		for (auto it = children.begin(); it != children.end(); it++) {
			if ((*it)->id == id) {
				delete *it;
				children.erase(it);
				return;
			}
		}
	}
	void Unit::RemoveChild(std::string name) {
		for (auto it = children.begin(); it != children.end(); it++) {
			if ((*it)->name == name) {
				delete *it;
				children.erase(it);
				return;
			}
		}
	}
	void Unit::RemoveChildAt(int index) {
		if (index >= children.size()) return;
		else {
			delete *(children.begin() + index);
			children.erase(children.begin() + index);
		}
	}
	bool Unit::MoveChild(Unit* unit, int pos) {
		if (pos >= children.size()) return false;

		if (unit->parent == this) {
			int prevPos;

			for (auto it = children.begin(); it != children.end(); it++) {
				if (unit == *it) {
					prevPos = distance(children.begin(), it);
				}
			}

			if (pos > prevPos) pos--;

			children.erase(children.begin() + prevPos);
			children.insert(children.begin() + pos, unit);

		} else {
			unit->parent->children.erase(unit->parent->children.begin() + GetChildIndex(unit->GetId()));

			unit->parent = this;
			children.insert(children.begin() + pos, unit);
		}

		return true;
	}

	void Unit::Print(std::ostream& os, bool withChildrenCount, bool withIds, int tabs, int childCount) const {
		int tabsLeft = tabs * 5;
		if (withChildrenCount) {tabsLeft -= 4;}
		for (int i = 0; i < tabsLeft ; i++) {
			os << ' ';
		}
		int indexDisplayed = childCount;
		if (indexDisplayed == 0) {
			if (parent != nullptr) {
				indexDisplayed = parent->GetChildIndex(id);
			}
		}
		if (withChildrenCount) {os /* << '[' */ << indexDisplayed << "] ";}

		os << (templateName ? "<" : "") << name << (templateName ? "> " : " ");
		if (withIds) os << "(id:" << id << ") ";

		if (children.size() == 0) {
			int iV;
			std::string sV;
			float fV;
			if (GetValue(iV)) os << "=" << iV << "  ";
			if (GetValue(sV)) os << "=" << sV << "  ";
			if (GetValue(fV)) os << "=" << fV << "  ";
			if (IsValueTemp()) os << "=<value>  ";
		}

		for (int i = 0; i < attrsInt.size(); i++) {
			os << attrsInt[i].name << ":" << attrsInt[i].value << " ";
		}
		for (int i = 0; i < attrsStr.size(); i++) {
			os << attrsStr[i].name << ":\"" << attrsStr[i].value << "\" ";
		}
		for (int i = 0; i < attrsTemp.size(); i++) {
			os << "<" << attrsTemp[i] << "> ";
		}

		os << std::endl;
		for (auto it = children.begin(); it != children.end(); it++) {
			(*it)->Print(os, children.size() > 1, withIds, tabs + 1, std::distance(children.begin(), it));
		}
	}
	void Unit::PrintXML(std::ostream& os, int tabs) const {
		for (int i = 0; i < tabs; i++) {
			os << '\t';
		}
		os << '<' << name;

		if (attrsInt.size() + attrsStr.size() > 0) {
			for (int i = 0; i < attrsInt.size(); i++) {
				os << ' ' << attrsInt[i].name << "=\"" << attrsInt[i].value << "\"";
			}
			for (int i = 0; i < attrsStr.size(); i++) {
				os << ' ' << attrsStr[i].name << "=\"" << attrsStr[i].value << "\" ";
			}
		}
		os << '>';

		if (children.size() == 0) {
			int iV;
			std::string sV;
			float fV;
			if (GetValue(iV)) os << iV;
			if (GetValue(sV)) os << sV;
			if (GetValue(fV)) os << fV;

		} else {
			os << std::endl;
			for (auto it = children.begin(); it != children.end(); it++) {
				(*it)->PrintXML(os, tabs + 1);
			}
			for (int i = 0; i < tabs; i++) {
				os << '\t';
			}
		}

		os << "</" << name << '>' << std::endl;

	}

	void Unit::ReadXML(std::istream& is, std::string* prolog) {
		char ch;
		bool insideUnit = false;
		bool insideValueField = false;

		// if (prolog != nullptr) prolog = new std::string();

		is.exceptions(std::ios_base::eofbit | std::ios_base::failbit);

		try {
			
			while (is.get(ch)) {

				if (ch == ' ' || ch == '\n') continue;

				// opening detected
				if (ch == '<' && !insideValueField && !insideUnit) {
					is.get(ch);

					// prolog detected
					if (ch == '?') {

						if (prolog != nullptr) *prolog += "<?";
						do {
							is.get(ch);
							if (prolog != nullptr) *prolog += ch;
						} while (ch != '>');
						// std::cout << *prolog;
					}

					// comment detected
					else if (ch == '!') {
						while (true) {
							is.get(ch);
							if (ch == '-') {
								is.get(ch);
								if (ch == '-') {
									is.get(ch);
									if (ch == '>') {
										break;
									}
								}
							}
						}
						continue;
					}

					else if (ch == '/') {
						is.unget();
						is.unget();
						insideValueField = true;
						continue;
					} 
					
					// unit detected
					else {
						name = ch;

						while (true) {
							is.get(ch);
							if (ch == ' ') {
								insideUnit = true;
								break;
							}
							if (ch == '>') {
								insideValueField = true;
								break;
							}
							name += ch;
						}

						continue;
					}

				}

				// read attributes
				if (insideUnit) {
					if (ch == ' ') continue;

					std::string attrName;
					std::string attrValue;

					// close detected
					if (ch == '>') {
						insideUnit = false;
						insideValueField = true;
						continue;
					}

					// read attribute name
					attrName = "";
					while (ch != '=') {
						attrName += ch;
						is.get(ch);
					}

					is.get(ch); // quote mark "
					is.get(ch); // value first character

					attrValue = "";
					while (ch != '\"') {
						attrValue += ch;
						is.get(ch);
					}

					try {
						int iV = std::stoi(attrValue);
						AddAttr(attrName, iV);
					} catch (const std::invalid_argument& ia) {
						AddAttr(attrName, attrValue);
					} catch (const std::out_of_range) {
						std::cout << id << std::endl;
						break;
					}


				}


				if (insideValueField) {
					// first child or unit close
					if (ch == '<') {
						is.get(ch);
						if (ch == '/') {
							while (ch != '>') {
								is.get(ch);
							}
							break;
						} 
						
						// comment detected
						else if (ch == '!') {
							while (ch != '-') {
								is.get(ch);
								if (ch == '-') {
									is.get(ch);
									if (ch == '-') {
										is.get(ch);
										if (ch == '>') {
											break;
										}
									}
								}
							}
							continue;
						}
						
						else {
							is.unget(); // '<'
							is.unget(); // '>'

							Unit* unit = new Unit();
							AddChild(unit);
							unit->ReadXML(is, nullptr);
							continue;
						}
					} 
					
					// value detected
					else {

						std::string valueStr;
						valueStr = ch;

						is.get(ch);
						while (ch != '<') {
							valueStr += ch;
							is.get(ch);
						}
						is.unget();

						try {
							int iV = std::stoi(valueStr);
							SetValue(iV);
						} catch (const std::invalid_argument& ia) {
							try {
								int fV = std::stof(valueStr);
								SetValue(fV);
							} catch (const std::invalid_argument& ia) {
								SetValue(valueStr);
							}
						} catch (const std::out_of_range) {
							std::cout << id << std::endl;
							break;
						}

						continue;
					}
				}
			}
			// reading finished







		} catch (std::ios_base::failure e) {
			std::cout << "fail reading " << e.what() << "\n";
			// throw e;
		}

	}

	Unit* Unit::CloneAsSibling() const {
		Unit* unit = new Unit(*this);
		parent->AddChild(unit);
		return unit;
	}



	bool Interface::ReadCommand(std::string& command) {
		std::string action = GetSentence(command);

		try {

			if (action == "disp") {
				std::string address = GetSentence(command);
				if (address == "") {
					if (mainUnit != nullptr) {
						mainUnit->Print(std::cout, true, true);
					}
					for (auto memUnit : memory) {
						memUnit->Print(std::cout, true, true);
					}
					return CommandReadingFinished();
				}
				
				Unit* unit = ReadAddress(address);
				if (unit == nullptr) return CommandReadingFinished();

				unit->Print(std::cout, true, true);

			}
			else if (action == "dispx") {
				std::string address = GetSentence(command);
				Unit* unit = ReadAddress(address);
				if (unit == nullptr) return CommandReadingFinished();

				unit->PrintXML(std::cout);
			}



			else if (action == "copy") {
				std::string address = GetSentence(command);
				Unit* unit = ReadAddress(address);
				if (unit == nullptr) return CommandReadingFinished();

				Unit* copy = new Unit(*unit);
				address = GetSentence(command);
				// copy to memory
				if (address == "") {
					memory.push_back(copy);
					std::cout << "Unit added to memory at index: " << memory.size() - 1 << std::endl;
				} 
				// copy to somewhere in tree
				else {
					Unit* parent = ReadAddress(address);
					parent->AddChild(copy);

					std::cout << "Unit added in tree\n";
				}
			}

			else if (action == "create") {
				std::string address = GetSentence(command);
				Unit* unit = new Unit();

				// create in memory
				if (address == "") {

					memory.push_back(unit);
					std::cout << "Unit created in memory at index: " << memory.size() - 1 << std::endl;
				} 
				// create as child
				else {
					Unit* parent = ReadAddress(address);
					parent->AddChild(unit);

					std::cout << "Unit created in tree as child\n";
				}
			}

			else if (action == "del") {
				std::string address = GetSentence(command);
				Unit* unit = ReadAddress(address);
				if (unit == nullptr) return CommandReadingFinished();

				if (unit->parent != nullptr) {
					unit->parent->RemoveChild(unit->GetId());

					std::cout << "Unit removed from tree\n";
				} else {
					if (unit == mainUnit) {
						delete unit;
						mainUnit = nullptr;

						std::cout << "Main unit removed\n";
					} else {
						for (auto it = memory.begin(); it != memory.end(); it++) {
							if ((*it) == unit) {
								delete unit;
								memory.erase(it);

								std::cout << "Unit removed from memory\n";
								break;
							}
						}
					}
				}

			} 

			else if (action == "move") {
				std::string address = GetSentence(command);
				Unit* unit = ReadAddress(address);
				if (unit == nullptr) return CommandReadingFinished();

				address = GetSentence(command);
				Unit* newParent = ReadAddress(address);
				if (unit == nullptr) return CommandReadingFinished();

				std::string newIndex = GetSentence(command);
				int index = newParent->children.size();

				try {
					index = std::stoi(newIndex);
					std::cout << "Unit moved\n";
				} catch (const std::invalid_argument& ia) {
					std::cout << "Cannot resolve index, moved to back\n";
				}
				

				newParent->MoveChild(unit, index);
			}


			else if (action == "name") {
				std::string address = GetSentence(command);
				Unit* unit = ReadAddress(address);
				if (unit == nullptr) return CommandReadingFinished();

				std::string name = GetSentence(command);
				unit->name = name;

				std::cout << "Unit name changed\n";
			}

			else if (action == "attri" || action == "attrs") {
				std::string address = GetSentence(command);
				Unit* unit = ReadAddress(address);
				if (unit == nullptr) return CommandReadingFinished();

				std::string name = GetSentence(command);
				std::string value = GetSentence(command, true);

				if (action == "attri") {
					try {
						unit->AddAttr(name, std::stoi(value));
					} catch (const std::invalid_argument& ia) {
						std::cout << "Invalid int number, string attribute set\n";
						action = "attrs";
					}
				}
				if (action == "attrs") {
					unit->AddAttr(name, value);
				}

				std::cout << "Added attribute to unit\n";
			}
			else if (action == "delattr") {
				std::string address = GetSentence(command);
				Unit* unit = ReadAddress(address);
				if (unit == nullptr) return CommandReadingFinished();

				std::string name = GetSentence(command);

				unit->RemoveAttr(name);
				std::cout << "Attribute removed\n";
			}

			else if (action == "vali" || action == "vals" || action == "valf") {
				std::string address = GetSentence(command);
				Unit* unit = ReadAddress(address);
				if (unit == nullptr) return CommandReadingFinished();

				std::string value = GetSentence(command, true);

				if (action == "vali"){
					try {
						unit->SetValue(std::stoi(value));
					} catch (const std::invalid_argument& ia) {
						std::cout << "Invalid int number, set string value\n";
						action = "vals";
					}
				}
				if (action == "valf"){
					try {
						unit->SetValue(std::stof(value));
					} catch (const std::invalid_argument& ia) {
						std::cout << "Invalid float number, set string value\n";
						action = "vals";
					}
				}
				if (action == "vals"){
					unit->SetValue(value);
				}
				std::cout << "Unit value changed\n";
			}



			else if (action == "tedit") {
				std::string action2 = GetSentence(command);

				std::string address = GetSentence(command);
				Unit* unit = ReadAddress(address);
				if (unit == nullptr) return CommandReadingFinished();

				if (action2 == "tname") {
					unit->templateName = true;
					std::cout << "Unit name set as variable\n";
				} else if (action2 == "sname") {
					unit->templateName = false;
					std::cout << "Unit name set as static\n";
				} else if (action2 == "attr") {
					std::string name = GetSentence(command);
					if (name != "") {
						unit->AddTemplateAttr(name);
						std::cout << "Added variable attribute to unit template\n";
					}
				} else if (action2 == "value") {
					unit->ValueIsTemp();
					std::cout << "Unit value set as variable\n";
				}
			}
			else if (action == "tcreate") {
				std::string address = GetSentence(command);
				Unit* unit = ReadAddress(address);
				if (unit == nullptr) return CommandReadingFinished();

				address = GetSentence(command);
				Unit* parent = ReadAddress(address);
				if (parent == nullptr) return CommandReadingFinished();

				Unit* copy = new Unit(*unit);
				parent->AddChild(copy);

				FillTemplate(copy, command);

				std::cout << "Unit created from template in tree/n";
			}
			else if (action == "tmemory") {
				std::string address = GetSentence(command);
				Unit* unit = ReadAddress(address);
				if (unit == nullptr) return CommandReadingFinished();

				Unit* copy = new Unit(*unit);
				memory.push_back(copy);

				FillTemplate(copy, command);

				std::cout << "Unit created from template in memory at index: " << memory.size() - 1 << std::endl;
			}


			else if (action == "exit") {
				std::cout << "Bye :)\n";
				return false;
			}



			else if (action == "openf") {
				std::string address = GetSentence(command);
				Unit* unit = ReadAddress(address);
				if (unit == nullptr) return true;

				address = GetSentence(command);

				std::ifstream inFile;
				inFile.open(address);

				if (inFile.is_open()) {
					std::string prolog;
					unit->ReadXML(inFile, &prolog);

					if (unit == mainUnit) mainProlog = prolog;

					std::cout << "File \"" << address << "\" loaded\n";
				} else {
					std::cout << "Couldn't open file\n";
				}

				inFile.close();

			}
			else if (action == "savef") {
				std::string address = GetSentence(command);
				Unit* unit = ReadAddress(address);
				if (unit == nullptr) return true;

				address = GetSentence(command);

				std::ofstream outFile;
				outFile.open(address);

				if (outFile.is_open()) {
					if (unit == mainUnit) outFile << mainProlog << std::endl;
					unit->PrintXML(outFile);

					std::cout << "Saved to \"" << address << "\"\n";
				} else {
					std::cout << "Couldn't save file\n";
				}
				outFile.close();
			}



			else {
				std::cout << "action \"" << action << "\"\n"; 
			}

		}


		catch (const std::exception& e) {
			std::cout << e.what() << std::endl;
		}

		return CommandReadingFinished();
	}

	Unit* Interface::ReadAddress(std::string& address) {
		std::string element;
		int nextEleMark = address.find('>');
		element = address.substr(0, nextEleMark);
		if (nextEleMark > -1) nextEleMark += 1;
		address.erase(0, nextEleMark);
		
		Units::Unit* unit = nullptr;

		while (!(element == "" || element.empty())) {
			Unit* prevUnit = unit;

			try {
				// look for id
				if (element[0] == 'i') {
					unit = FindById(std::stoi(element.substr(1)), unit);
				}

				// look for index
				else {
					int num = std::stoi(element);
					// look in memory
					if (unit == nullptr) {
						if (memory.size() > num) {
							unit = memory[num];
						} else return nullptr;
					}
					else {
						if (unit->children.size() > num) {
							unit = unit->GetChildAt(num);
						} else return unit;
					}
				}
			} 
			// look for name
			catch (const std::invalid_argument& ia) {
				// look in mainUnit and memory
				if (unit == nullptr) {
					// create mainUnit
					if (mainUnit == nullptr && element == "MAIN") {
						unit = mainUnit = new Unit();
					}
					// look in mainUnit
					if (mainUnit != nullptr) {
						if (element == "MAIN" || element == mainUnit->GetName()) {
							unit = mainUnit;
						}
					}
					// look in memory
					if (unit == nullptr) {
						for (auto unit2 : memory) {
							if (element == unit2->GetName()) {
								unit = unit2;
								break;
							}
						}
					}
					// haven't found
					if (unit == nullptr) {
						break;
					}
				} 
				// look in children
				else {
					for (auto unit2 : unit->children) {
						if (element == unit2->GetName()) {
							unit = unit2;
							break;
						}
					}
				}
			}

			if (prevUnit == unit) break;

			nextEleMark = address.find('>');
			element = address.substr(0, nextEleMark);
			if (nextEleMark > -1) nextEleMark += 1;
			address.erase(0, nextEleMark);

		} 

		return unit;
	}

	Unit* Interface::FindById(int id, Unit* searchIn) const {
		// look in mainUnit and memory
		if (searchIn == nullptr) {
			if (mainUnit != nullptr && id == mainUnit->GetId()) {
				return mainUnit;
			}
			for (auto unit : memory) {
				if (id == unit->GetId()) {
					return unit;
				}
			}
			for (auto unit : memory) {
				Unit* rtn = FindById(id, unit);
				if (rtn != unit) return rtn;
			}
			if (mainUnit != nullptr) {
				Unit* rtn = FindById(id, mainUnit);
				if (rtn != mainUnit) return rtn;
			}
		}

		// look up children
		else {
			for (auto unit : searchIn->children) {
				if (id == unit->GetId()) {
					return unit;
				}
			}
			for (auto unit : searchIn->children) {
				Unit* rtn = FindById(id, unit);
				if (rtn != unit) return rtn;
			}
		}

		return searchIn;
	}

	std::string Interface::GetSentence(std::string& command, bool hideQuotesIfFound) const {
		std::string element;

		int sentenceLen = command.find(' ');
		int sentenceBegin = 0;
		while (sentenceLen == 0) {
			sentenceBegin++;
			sentenceLen = command.find(' ', sentenceBegin) - sentenceBegin;
		}
		if (hideQuotesIfFound && command[sentenceBegin] == '\"') {
			sentenceBegin++;
			sentenceLen = command.find('\"', sentenceBegin) - sentenceBegin;
		}

		element = command.substr(sentenceBegin, sentenceLen);
		// if (element == "") throw std::runtime_error("invalid command"); 
		if (sentenceLen > -1) {
			sentenceLen += 1;
		}
		command.erase(0, sentenceLen + sentenceBegin);

		return element;
	}

	void Interface::FillTemplate(Unit* unit, std::string& command) {
		std::string sentence;

		if (unit->templateName == true) {
			sentence = GetSentence(command);
			if (sentence == "") return;

			unit->name = sentence;
		}

		if (unit->IsValueTemp()) {
			sentence = GetSentence(command, true);
			if (sentence == "") return;

			try {
				unit->SetValue(std::stoi(sentence));
			} catch (const std::invalid_argument& ia) {
				try {
					unit->SetValue(std::stof(sentence));
				} catch (const std::invalid_argument& ia2) {
					unit->SetValue(sentence);
				}
			}
		}

		while (unit->AnyTemplateAttrs()) {
			sentence = GetSentence(command, true);
			if (sentence == "") return;

			try {
				unit->ImportTemplateAttr(std::stoi(sentence));
			} catch (const std::invalid_argument& ia) {
				unit->ImportTemplateAttr(sentence);
			}
		}

		for (auto it = unit->children.begin(); it != unit->children.end(); it++) {
			FillTemplate(*it, command);
			if (sentence == "") return;
		}
	}
}