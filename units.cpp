#include <iostream>

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
	bool Unit::RemoveAttr(std::string name) {
		for (std::vector<Attribute<long>>::iterator it = attrsInt.begin(); it != attrsInt.end() ; it++) {
			if (it->name == name) {
				attrsInt.erase(it);
				return true;
			}
		}
		for (std::vector<Attribute<std::string>>::iterator it = attrsStr.begin(); it != attrsStr.end() ; it++) {
			if (it->name == name) {
				attrsStr.erase(it);
				return true;
			}
		}
		return false;
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
		if (index >= children.size() || index < 0) return nullptr;
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
		if (unit->parent == this) {
			if (pos >= children.size()) return false;

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
			if (pos > children.size()) return false;
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

	void Unit::ReadXML(std::istream& is, bool forceString, std::string* prolog) {
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
						
						continue;
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

					// closing detected
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
								insideValueField = false;
								break;
							}
							if (ch == '/') {
								is.unget();
								insideUnit = true;
								insideValueField = false;
								break;
							}
							if (ch == '>') {
								insideUnit = false;
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

					// close detected
					if (ch == '>') {
						insideUnit = false;
						insideValueField = true;
						continue;
					}

					// unit closing detected
					if (ch == '/') {
						is.get(ch);
						while (ch != '>') is.get(ch);
						insideUnit = false;
						insideValueField = false;
						break;
					}

					std::string attrName;
					std::string attrValue;


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

					if (forceString) {
						AddAttr(attrName, attrValue);
					} else {
						try {
							int iV = std::stoi(attrValue);
							AddAttr(attrName, iV);
						} catch (const std::exception& ia) {
							AddAttr(attrName, attrValue);
						}
					}


				}


				if (insideValueField) {
					// first child or unit close
					if (ch == '<') {
						is.get(ch);

						// unit closing detected
						if (ch == '/') {
							while (ch != '>') {
								is.get(ch);
							}
							insideUnit = false;
							insideValueField = false;
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
							unit->ReadXML(is, forceString,nullptr);
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

						if (forceString) {
							SetValue(valueStr);
						} else {
							try {
								int iV = std::stoi(valueStr);
								SetValue(iV);
							} catch (const std::exception& ia) {
								try {
									int fV = std::stof(valueStr);
									SetValue(fV);
								} catch (const std::exception& ia) {
									SetValue(valueStr);
								}
							}
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
		if (parent == nullptr) return nullptr;
		Unit* unit = new Unit(*this);
		parent->AddChild(unit);
		return unit;
	}


}