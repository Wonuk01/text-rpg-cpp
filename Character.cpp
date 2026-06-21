#include "Character.h"
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <limits>

Character::Character(const std::string& name, const std::string& job,
                     int hp, int attackPower, int defense)
    : Entity(name, hp, attackPower, defense),
      job(job), level(1), exp(0), expToNextLevel(100), gold(0) {}

// ── 상태 출력 ─────────────────────────────────

void Character::printStatus() const {
    float ratio = (float)hp / maxHp;
    const char* hpColor;
    if      (ratio > 0.5f)  hpColor = "\033[32m";
    else if (ratio > 0.25f) hpColor = "\033[33m";
    else                    hpColor = "\033[1;31m";

    std::cout << "\n  \033[1;37m╔══════════════════════════════╗\033[0m\n";
    std::cout << "    \033[1;36m[" << name << "]\033[0m  " << job << "  Lv." << level << "\n";
    std::cout << "    HP " << hpColor << makeHpBar(15) << "\033[0m " << hp << "/" << maxHp << "\n";
    std::cout << "    ATK: \033[33m" << attackPower << "\033[0m  DEF: \033[36m" << defense << "\033[0m\n";
    std::cout << "    EXP: " << exp << " / " << expToNextLevel << "\n";
    std::cout << "    GOLD: \033[33m" << gold << "G\033[0m\n";
    // 스킬 보유 시 표시
    if (!activeSkills.empty()) {
        std::cout << "    스킬: ";
        for (const auto& s : activeSkills)
            std::cout << "\033[35m" << s.name << "\033[0m(" << s.usesLeft << "/" << s.maxUses << ") ";
        std::cout << "\n";
    }
    // 데미지 공식 안내
    std::cout << "\n    \033[90m※ 데미지 = 공격력 - 적 방어력 (최소 1)\033[0m\n";
    std::cout << "    \033[90m※ 크리티컬(15%) = 데미지 × 1.5\033[0m\n";
    std::cout << "  \033[1;37m╚══════════════════════════════╝\033[0m\n";
}

// ── 인벤토리 ──────────────────────────────────

void Character::addItem(const Item& item) {
    inventory.push_back(item);
    std::cout << "[아이템 획득] " << item.getName() << "\n";
}

void Character::removeItem(const std::string& itemName) {
    auto it = std::find_if(inventory.begin(), inventory.end(),
        [&itemName](const Item& i) { return i.getName() == itemName; });
    if (it == inventory.end())
        throw std::runtime_error("인벤토리에 [" + itemName + "] 아이템이 없습니다.");
    inventory.erase(it);
}

Item* Character::findItem(const std::string& itemName) {
    auto it = std::find_if(inventory.begin(), inventory.end(),
        [&itemName](const Item& i) { return i.getName() == itemName; });
    return (it != inventory.end()) ? &(*it) : nullptr;
}

void Character::sortInventory() {
    std::sort(inventory.begin(), inventory.end(),
        [](const Item& a, const Item& b) {
            if (a.getType() != b.getType()) return a.getType() > b.getType();
            return a.getName() < b.getName();
        });
}

void Character::printInventory() const {
    std::cout << "\n== 인벤토리 ==\n";
    if (inventory.empty()) { std::cout << "  (비어 있음)\n"; return; }
    for (int i = 0; i < (int)inventory.size(); i++)
        std::cout << "  " << i+1 << ". " << inventory[i].getName()
                  << " - " << inventory[i].getDescription() << "\n";
}

void Character::useItem(const std::string& itemName) {
    Item* item = findItem(itemName);
    if (!item)
        throw std::runtime_error("사용할 수 없습니다. [" + itemName + "] 아이템이 없습니다.");
    if (item->getType() == "heal") {
        heal(item->getEffectValue());
        std::cout << "[" << itemName << "] 사용! HP +" << item->getEffectValue()
                  << " (현재: " << hp << ")\n";
    } else if (item->getType() == "attack") {
        setAttackPower(attackPower + item->getEffectValue());
        std::cout << "[" << itemName << "] 사용! ATK +" << item->getEffectValue()
                  << " (현재: " << attackPower << ")\n";
    }
    removeItem(itemName);
}

// ── 경험치 / 레벨업 ───────────────────────────

void Character::gainExp(int amount) {
    exp += amount;
    std::cout << "경험치 +" << amount << " (현재: " << exp << "/" << expToNextLevel << ")\n";
    if (exp >= expToNextLevel) levelUp();
}

void Character::levelUp() {
    level++;
    exp -= expToNextLevel;
    expToNextLevel = static_cast<int>(expToNextLevel * 1.5);

    std::cout << "\n\033[1;33m";
    std::cout << "  ╔═══════════════════════════════╗\n";
    std::cout << "  ║    ★  LEVEL UP!  Lv." << level << "  ★      ║\n";
    std::cout << "  ╚═══════════════════════════════╝\033[0m\n\n";

    std::cout << "  능력치를 선택하세요:\n";
    std::cout << "  \033[33m1\033[0m. 체력 강화    (MaxHP \033[32m+40\033[0m)\n";
    std::cout << "  \033[33m2\033[0m. 공격력 강화  (ATK \033[31m+10\033[0m, MaxHP +10)\n";
    std::cout << "  \033[33m3\033[0m. 방어 강화    (DEF \033[36m+5\033[0m,  MaxHP +10)\n";
    std::cout << "> ";

    int choice = 1;
    std::cin >> choice;
    if (std::cin.fail()) { std::cin.clear(); choice = 1; }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    switch (choice) {
        case 2:
            attackPower += 10; maxHp += 10;
            std::cout << "  \033[31mATK +10\033[0m, MaxHP +10 강화!\n";
            break;
        case 3:
            defense += 5; maxHp += 10;
            std::cout << "  \033[36mDEF +5\033[0m, MaxHP +10 강화!\n";
            break;
        default:
            maxHp += 40;
            std::cout << "  \033[32mMaxHP +40\033[0m 강화!\n";
            break;
    }
    hp = maxHp;  // 레벨업 시 HP 완전 회복

    onLevelUp();  // 스킬 언락 (서브클래스 오버라이드)
}

// ── 스킬 관련 ─────────────────────────────────

int Character::useSkill(int /*idx*/) {
    std::cout << "  사용 가능한 스킬이 없습니다.\n";
    return 0;
}

void Character::printSkills() const {
    std::cout << "\n  ╔══════════════════════════════╗\n";
    std::cout << "         ★ 스킬 목록 ★\n";
    if (activeSkills.empty()) {
        std::cout << "  (Lv.2 달성 시 스킬을 배웁니다)\n";
    } else {
        for (int i = 0; i < (int)activeSkills.size(); i++) {
            const auto& s = activeSkills[i];
            const char* col = s.usesLeft > 0 ? "\033[33m" : "\033[90m";
            std::cout << "  " << col << (i+1) << ". " << s.name
                      << "  [" << s.usesLeft << "/" << s.maxUses << "]\033[0m"
                      << "  - " << s.description << "\n";
        }
    }
    std::cout << "  ╚══════════════════════════════╝\n";
}

void Character::resetBattleSkills() {
    for (auto& s : activeSkills) s.usesLeft = s.maxUses;
}

const std::vector<ActiveSkill>& Character::getActiveSkills() const { return activeSkills; }

// ── Getter ────────────────────────────────────

std::string Character::getJob() const { return job; }
int Character::getLevel() const { return level; }
int Character::getExp() const { return exp; }
int Character::getGold() const { return gold; }
void Character::addGold(int amount) {
    gold += amount;
    std::cout << "\033[33m[골드 획득] +" << amount << "G (현재: " << gold << "G)\033[0m\n";
}
bool Character::spendGold(int amount) {
    if (gold < amount) return false;
    gold -= amount;
    return true;
}
std::vector<Item> Character::getInventory() const { return inventory; }
const std::vector<Item>& Character::getInventoryRef() const { return inventory; }

void Character::setLevel(int v) { level = v; }
void Character::setExp(int v) { exp = v; }
void Character::setExpToNextLevel(int v) { expToNextLevel = v; }
