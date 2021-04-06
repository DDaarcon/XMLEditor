#ifndef UNIT_HPP
#define UNIT_HPP

#include <string>
#include <vector>

namespace Units {

	class Unit {

		
	public:


		enum DataType {
			Int, String, Float, Template
		};

		union StoreValue {
			int iValue;
			float fValue;
			StoreValue() {}
			~StoreValue() {}
		};

		template<typename T>
		struct Attribute {
			std::string name;
			T value;
		};


		int id;
		std::string name;
		bool templateName = false;

		std::vector<Unit*> children;
		Unit* parent = nullptr;

	private:
		static int idCounter;

		DataType storeType;
		StoreValue storeValue;
		std::string sValue;

		std::vector<Attribute<long>> attrsInt;
		std::vector<Attribute<std::string>> attrsStr;
		std::vector<std::string> attrsTemp;

	public:

		Unit() {
			id = idCounter++;
		}

		Unit(Unit* parent) {
			id = idCounter++;

			this->parent = parent;
		}

		Unit(const Unit& copy) {
			id = idCounter++;
			name = copy.name;
			templateName = copy.templateName;
			attrsInt = copy.attrsInt;
			attrsStr = copy.attrsStr;
			attrsTemp = copy.attrsTemp;

			switch (copy.GetDataType()) {
				default:
				case DataType::String:
				{
					std::string str;
					copy.GetValue(str);
					SetValue(str);
					break;
				}
				case DataType::Int:
					int i;
					copy.GetValue(i);
					SetValue(i);
					break;
				case DataType::Float:
					float f;
					copy.GetValue(f);
					SetValue(f);
					break;
				case DataType::Template:
					storeType = DataType::Template;
			}

			for (auto it = copy.children.begin(); it != copy.children.end(); it++) {
				Unit* unit = new Unit(**it);
				AddChild(unit);
			}
		}

		~Unit() {
			EraseChildren();
		}

		std::string GetName() const {return name;}
		int GetId() const {return id;}

		void SetValue(int); void SetValue(std::string); void SetValue(float);
		void SetTemplateValue() {storeType = DataType::Template;}
		DataType GetDataType() const {return storeType;}
		bool GetValue(int&) const; bool GetValue(std::string&) const; bool GetValue(float&) const;
		bool IsValueTemp() const {return storeType == DataType::Template;}
		void ValueIsTemp() {storeType = DataType::Template;}

		void AddAttr(std::string, long);
		void AddAttr(std::string, std::string);
		void AddTemplateAttr(std::string);
		bool ImportTemplateAttr(int); bool ImportTemplateAttr(std::string);
		bool AnyTemplateAttrs() const {return attrsTemp.size() > 0;}
		bool GetAttr(std::string, long&) const;
		bool GetAttr(std::string, std::string&) const;
		bool RemoveAttr(std::string);

		Unit* AddChild();
		Unit* AddChild(Unit*);
		Unit* GetChild(int) const;
		int GetChildIndex(int) const;
		Unit* GetChild(std::string) const;
		Unit* GetChildAt(int) const;
		void RemoveChild(int);
		void RemoveChild(std::string);
		void RemoveChildAt(int);
		bool MoveChild(Unit*, int);

		void Print(std::ostream& os, bool withChildrenCount, bool withIds) const {
			Print(os, withChildrenCount, withIds, 0, 0);
		}
		void PrintXML(std::ostream&, int tabs = 0) const;

		void ReadXML(std::istream&, std::string*);

		Unit* CloneAsSibling() const;

	private:

		void Print(std::ostream&, bool, bool, int, int) const;

		bool SearchForAttrName(std::string& name, bool remove) {
			for (auto it = attrsInt.begin(); it != attrsInt.end(); it++) {
				if (it->name == name) {
					if (remove) attrsInt.erase(it);
					return true;
				}
			}
			for (auto it = attrsStr.begin(); it != attrsStr.end(); it++) {
				if (it->name == name) {
					if (remove) attrsStr.erase(it);
					return true;
				}
			}
			for (auto it = attrsTemp.begin(); it != attrsTemp.end(); it++) {
				if (*it == name) {
					if (remove) attrsTemp.erase(it);
					return true;
				}
			}
			return false;
		}

		void EraseChildren() {
			for (auto it = children.begin(); it != children.end();) {
				delete *it;
				children.erase(it);
			}
		}
	};

}
#endif