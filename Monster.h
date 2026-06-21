#pragma once
#include "Entity.h"
#include "Item.h"
#include <vector>
#include <string>

// =============================================
// Monster : 몬스터 추상 클래스
// =============================================
class Monster : public Entity {
protected:
    int expReward;
    int goldReward;

public:
    Monster(const std::string& name, int hp, int attackPower, int defense, int expReward, int goldReward);
    virtual ~Monster() = default;

    virtual int attack() = 0;
    virtual void printStatus() const override;
    virtual std::string getMonsterType() const = 0;
    virtual std::string getAsciiArt() const = 0;  // ASCII 아트

    int getExpReward() const;
    int getGoldReward() const;
};

// =============================================
// Goblin : 빠른 연속 공격, 낮은 HP
// =============================================
class Goblin : public Monster {
public:
    Goblin();
    int attack() override;
    std::string getMonsterType() const override;
    std::string getAsciiArt() const override;
};

// =============================================
// Wolf : 빠른 발톱 공격, 가끔 울부짖음
// =============================================
class Wolf : public Monster {
private:
    bool nextAttackBuffed;
public:
    Wolf();
    int attack() override;
    std::string getMonsterType() const override;
    std::string getAsciiArt() const override;
};

// =============================================
// Crocodile : 물기 + 데스롤 필살기
// =============================================
class Crocodile : public Monster {
private:
    int biteCount;
public:
    Crocodile();
    int attack() override;
    std::string getMonsterType() const override;
    std::string getAsciiArt() const override;
};

// =============================================
// Dragon : 보스급, 광역 브레스 공격
// =============================================
class Dragon : public Monster {
private:
    int breathCount;
public:
    Dragon();
    int attack() override;
    std::string getMonsterType() const override;
    std::string getAsciiArt() const override;
};
