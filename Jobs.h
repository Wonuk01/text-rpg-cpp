#pragma once
#include "Character.h"

// =============================================
// Warrior : 근접 전사
// 스킬: Lv2 분노의 일격 (다음 공격 2배, 2회/전투)
// =============================================
class Warrior : public Character {
private:
    bool powerStrikeReady;
    bool rageStrikeActive;  // 분노의 일격 발동 중

protected:
    void onLevelUp() override;

public:
    Warrior(const std::string& name);

    int attack() override;
    int useSkill(int skillIndex) override;
    std::string getJobName() const override;
    void resetBattleSkills() override;
};

// =============================================
// Mage : 마법사
// 스킬: Lv2 마력 폭발 (마나 전소, 고데미지, 1회/전투)
// =============================================
class Mage : public Character {
private:
    int mana;
    int maxMana;

protected:
    void onLevelUp() override;

public:
    Mage(const std::string& name);

    int attack() override;
    int useSkill(int skillIndex) override;
    std::string getJobName() const override;
    void printStatus() const override;
};

// =============================================
// Healer : 힐러
// 스킬: Lv2 신성 치유 (HP 40% 즉시 회복, 2회/전투)
// =============================================
class Healer : public Character {
private:
    int healPerTurn;

protected:
    void onLevelUp() override;

public:
    Healer(const std::string& name);

    int attack() override;
    int useSkill(int skillIndex) override;
    std::string getJobName() const override;
    void printStatus() const override;
    void onTurnEnd() override;
};
