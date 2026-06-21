#include "Item.h"

Item::Item(const std::string& name, const std::string& type, int effectValue, const std::string& desc)
    : name(name), type(type), effectValue(effectValue), description(desc) {}

std::string Item::getName() const { return name; }
std::string Item::getType() const { return type; }
int Item::getEffectValue() const { return effectValue; }
std::string Item::getDescription() const { return description; }

// 이름 기준 정렬 (STL sort에서 사용)
bool Item::operator<(const Item& other) const {
    return name < other.name;
}

// 이름 기준 동등 비교 (STL find에서 사용)
bool Item::operator==(const Item& other) const {
    return name == other.name;
}
