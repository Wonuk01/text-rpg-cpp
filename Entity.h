#pragma once
#include <string>

// =============================================
// Entity : Character와 Monster의 공통 추상 클래스
// 다형성의 핵심 - 순수 가상함수 attack() 포함
// =============================================
class Entity {
protected:
    std::string name;
    int hp;
    int maxHp;
    int attackPower;
    int defense;

public:
    Entity(const std::string& name, int hp, int attackPower, int defense);
    virtual ~Entity() = default;

    // 순수 가상함수 - 파생 클래스에서 반드시 구현
    virtual int attack() = 0;
    virtual void printStatus() const = 0;

    // 공통 기능
    void takeDamage(int damage);
    bool isAlive() const;
    void heal(int amount);

    // Getter
    std::string getName() const;
    int getHp() const;
    int getMaxHp() const;
    int getAttackPower() const;
    int getDefense() const;

    // Setter
    void setAttackPower(int value);
    void setDefense(int value);
    void setHp(int value);
    void setMaxHp(int value);

    // UI 헬퍼 - 체력바 문자열 반환 (공통 사쫜)
    std::string makeHpBar(int barLen = 20) const;
};
