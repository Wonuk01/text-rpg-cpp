#pragma once
#include <string>

// =============================================
// Item : 아이템 종류 및 효과 정의
// type: "heal" = HP 회복, "attack" = 공격력 증가
// =============================================
class Item {
private:
    std::string name;
    std::string type;   // "heal" | "attack"
    int effectValue;
    std::string description;

public:
    Item(const std::string& name, const std::string& type, int effectValue, const std::string& desc);

    std::string getName() const;
    std::string getType() const;
    int getEffectValue() const;
    std::string getDescription() const;

    // 연산자 오버로딩 - 이름 기준 정렬용
    bool operator<(const Item& other) const;
    bool operator==(const Item& other) const;
};
