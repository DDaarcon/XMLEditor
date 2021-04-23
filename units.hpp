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
   /**
    * @brief  Returns name (from constant objects)
    * @retval Name of the unit
    */
		std::string GetName() const {return name;}
   /**
    * @brief  Returns id (even from constant objects)
    * @retval Id if the unit
    */
		int GetId() const {return id;}
   /**
    * @brief  Sets value of unit (int, string, float)
    * @retval None
    */
		void SetValue(int); void SetValue(std::string); void SetValue(float);
   /**
    * @brief  Marks value as template
    * @retval None
    */
		void SetTemplateValue() {storeType = DataType::Template;}
   /**
    * @brief  Returns what type of value is stored
    * @retval Units::Unit::DataType - info about stored value
    */
		DataType GetDataType() const {return storeType;}
   /**
    * @brief  Gets int value
    * @param  value: Reference to variable where value of the unit should be passed
    * @retval True if value of the unit is int, false otherwise
    */
		bool GetValue(int& value) const; 
   /**
    * @brief  Gets string value
    * @param  value: Reference to variable where value of the unit should be passed
    * @retval True if value of the unit is string, false otherwise
    */
		bool GetValue(std::string& value) const; 
   /**
    * @brief  Gets float value
    * @param  value: Reference to variable where value of the unit should be passed
    * @retval True if value of the unit is float, false otherwise
    */
		bool GetValue(float& value) const;
   /**
    * @brief  Returns information if value is template
    * @retval True if value is template, false otherwise
    */
		bool IsValueTemp() const {return storeType == DataType::Template;}
   /**
    * @brief  Same as SetTemplateValue() (preferred method)
    * @retval None
    */
		void ValueIsTemp() {storeType = DataType::Template;}

   /**
    * @brief  Adds (or overrides) attribute
    * @param  name: Name of the attribute
    * @param  value: Int (long) value of the attribute
    * @retval None
    */
		void AddAttr(std::string name, long value);
   /**
    * @brief  Adds (or overrides) attribute
    * @param  name: Name of the attribute
    * @param  value: String value of the attribute
    * @retval None
    */
		void AddAttr(std::string name, std::string value);
   /**
    * @brief  Adds (or overrides existing) attribute and marks it as template
    * @param  name: Name of the template attribute
    * @retval None
    */
		void AddTemplateAttr(std::string name);
   /**
    * @brief  Next template attribute (in chronological order) is turned into normal attribute
    * @param  value: Value of the attribute
    * @retval True if attribute is added successfully, false if there are no template attributes
    */
		bool ImportTemplateAttr(int value); bool ImportTemplateAttr(std::string value);
   /**
    * @brief  Returns if there are any template attributes
    * @retval True if there are any template attributes, false otherwise
    */
		bool AnyTemplateAttrs() const {return attrsTemp.size() > 0;}
   /**
    * @brief  Gets value of the attribute
    * @param  name: Name of the attribute
    * @param  value: Reference to variable where value of the attribute should be passed
    * @retval True if attribute of specified type is present, false otherwise (try another type?)
    */
		bool GetAttr(std::string name, long& value) const; bool GetAttr(std::string name, std::string& value) const;
   /**
    * @brief  Checks if attribute is present
    * @param  name: Name of the attribute
    * @retval True if attribute is present, false otherwise
    */
		bool RemoveAttr(std::string name);

   /**
    * @brief  Adds new child unit
    * @retval Address to newly created unit
    */
		Unit* AddChild();
   /**
    * @brief  Adds new child unit
    * @param  unit: Unit to copy
    * @retval Address to newly created unit
    */
		Unit* AddChild(Unit* unit);
   /**
    * @brief  Gets child with specified id
    * @param  id: Id of the unit
    * @retval Address to the unit if was found, nullptr if wasn't
    */
		Unit* GetChild(int id) const;
   /**
    * @brief  Gets index (which in turn is unit with specified id)
    * @param  id: Id if the unit
    * @retval Index of the unit if was found, nullptr if wasn't
    */
		int GetChildIndex(int id) const;
   /**
    * @brief  Gets first child with specified name
    * @param  name: Name of unit
    * @retval Address to the unit if was found, false if wasn't
    */
		Unit* GetChild(std::string name) const;
   /**
    * @brief  Gets child at specified index
    * @param  index: Index in children
    * @retval Address to the unit, nullptr if index exceeded number of children
    */
		Unit* GetChildAt(int index) const;
   /**
    * @brief  Removes child with specified id
    * @param  id: Id of the child
    * @retval None
    */
		void RemoveChild(int id);
   /**
    * @brief  Removes child with specified name
    * @param  name: Name of the child
    * @retval None
    */
		void RemoveChild(std::string name);
   /**
    * @brief  Removes index-th child
    * @param  index: Index in children
    * @retval None
    */
		void RemoveChildAt(int index);
   /**
    * @brief  Moves specified unit below this unit, as pos-th child
    * @param  unit: Unit to move
    * @param  pos: Position in children
    * @retval True if moved successfully, false if pos exceeded children count
    */
		bool MoveChild(Unit* unit, int pos);

   /**
    * @brief  Prints data
    * @param  os: Printing target
    * @param  withChildrenCount: Print also indices of children
    * @param  withIds: Print also ids
    * @retval None
    */
		void Print(std::ostream& os, bool withChildrenCount, bool withIds) const {
			Print(os, withChildrenCount, withIds, 0, 0);
		}
   /**
    * @brief  Prints data as xml
	* @param  os: Printing target
    * @param  tabs: Tabs at start (should be 0)
    * @retval None
    */
		void PrintXML(std::ostream& os, int tabs = 0) const;

   /**
    * @brief  Reads data form file
    * @param  os: File to load from
    * @param  prolog: Pointer to string where prolog should be stored
    * @retval None
    */
		void ReadXML(std::istream& os, std::string* prolog);

   /**
    * @brief  Clones this unit and adds copy to parent
    * @retval Address to newly created unit, nullptr if you try to clone parentless unit
    */
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