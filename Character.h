#pragma once
#include "Entity.h"
#include "Item.h"
#include <vector>
#include <string>
#include <limits>

// 전투 중 사용 가능한 액티브 스킬
struct ActiveSkill {
    std::string name;
    std::string description;
    int usesLeft;
    int maxUses;
    ActiveSkill(const std::string& n, const std::string& d, int uses)
        : name(n), description(d), usesLeft(uses), maxUses(uses) {}
};

// =============================================
// Character : 플레이어 캐릭터 추상 클래스
// =============================================
class Character : public Entity {
protected:
    std::string job;
    int level;
    int exp;
    int expToNextLevel;
    int gold;
    std::vector<Item> inventory;
    std::vector<ActiveSkill> activeSkills;

    virtual void onLevelUp() {}  // 서브클래스에서 스킬 언락

public:
    Character(const std::string& name, const std::string& job,
              int hp, int attackPower, int defense);
    virtual ~Character() = default;

    // 순수 가상
    virtual int attack() = 0;
    virtual void printStatus() const override;
    virtual std::string getJobName() const = 0;

    // 스킬 (서브클래스에서 오버라이드, 반환값 = 몬스터 데미지)
    virtual int useSkill(int skillIndex);

    // 인벤토리
    void addItem(const Item& item);
    void removeItem(const std::string& itemName);
    Item* findItem(const std::string& itemName);
    void sortInventory();
    void printInventory() const;
    void useItem(const std::string& itemName);

    // 경험치 / 레벨업
    void gainExp(int amount);
    void levelUp();

    // 스킬 관련
    void printSkills() const;
    virtual void resetBattleSkills();
    virtual void onTurnEnd() {}  // 턴 종료 훅 (Healer 자동 회복 등)
    const std::vector<ActiveSkill>& getActiveSkills() const;

    // Getter
    std::string getJob() const;
    int getLevel() const;
    int getExp() const;
    int getGold() const;
    void addGold(int amount);
    bool spendGold(int amount);
    std::vector<Item> getInventory() const;
    const std::vector<Item>& getInventoryRef() const;

      // Setter
    void setLevel(int value);
    void setExp(int value);
    void setExpToNextLevel(int value);
};
