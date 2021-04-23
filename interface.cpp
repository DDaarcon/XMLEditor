#include <fstream>
#include "interface.hpp"


namespace Units {


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

				if (unit->RemoveAttr(name))
					std::cout << "Attribute removed\n";
				else
					std::cout << "Couldn't find attribute\n";
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
					unit->SetTemplateValue();
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



			else if (action == "help") {
				std::cout 
				<< "Unit address explain (accessing units):\n" 
				<< "MAIN - main unit,\ti533 - unit with id 533,\n"
				<< "LiSt - child unit with name \"LiSt\",\t4 - 5th child unit\n"
				<< "\n"
				<< "Unit address example:\n"
				<< "MAIN>0>width\t(access main unit, 1st child unit, unit named \"width\")\n"
				<< "3>0\t(access 4th unit in memory, 1st child)\n"
				<< "i423>2\t(access unit with id 423, 3rd child unit)\n"
				<< "\n"
				<< "Open file:\topenf MAIN <source>\n"
				<< "Save file:\tsavef <unit> <destination>\n"
				<< "\n"
				<< "Display:\tdisp <unit>\tDisplay all:\tdisp\n"
				<< "Display as XML:\tdispx <unit>\n"
				<< "\n"
				<< "Change name\tname <unit> <name>\n"
				<< "Change value (int/float/string):\tvali/valf/vals <unit> <value>\n"
				<< "Set attribute (int/string):\tattri/attrs <unit> <name> <value>\n"
				<< "Delete attribute:\tdelattr <unit> <name>\n"
				<< "\n"
				<< "Copy:\tcopy <unit> <parent>\n"
				<< "Copy to memory:\tcopy <unit>\n"
				<< "Create:\tcreate <parent>\n"
				<< "Create in memory:\tcreate\n"
				<< "Delete:\tdel <unit>\n"
				<< "Move:\tmove <unit> <new parent> <new index>\n"
				<< "\n"
				<< "Working with templates:\n"
				<< "Suggested way to create and use templates:\tcreate/copy unit to memory, set template names, values, attributes, then recreate unit from template\n"
				<< "Set template name:\ttedit tname <unit>\n"
				<< "Set static name (non-template):\ttedit sname <unit>\n"
				<< "Set template value:\ttedit value <unit>\n"
				<< "Set template attribute:\ttedit attr <unit> <name>\n"
				<< "Recreating template:\n"
				<< "To somewhere in tree:\ttcreate <template unit> <parent> <values> <value> ... <value>\n"
				<< "To memory:\ttmemory <template unit> <value> <value> ... <value>\n"
				<< "When recreating template, values are interpred as:\t name (if template) > value (if template) > attrs (in order in which they were added) > childs name (if template) > ...\n";
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