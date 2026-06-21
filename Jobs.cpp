#include "Jobs.h"
#include <iostream>
#include <cstdlib>
#include <algorithm>

// ── Warrior ───────────────────────────────────

Warrior::Warrior(const std::string& name)
    : Character(name, "전사", 120, 18, 8),
      powerStrikeReady(false), rageStrikeActive(false) {}

int Warrior::attack() {
    bool raged = rageStrikeActive;
    rageStrikeActive = false;

    int dmg;
    if (powerStrikeReady) {
        powerStrikeReady = false;
        dmg = static_cast<int>(attackPower * 1.5) + (rand() % 5);
        std::cout << "  [" << name << "] ⚔  강공격! (" << dmg << " 데미지)\n";
    } else {
        powerStrikeReady = true;
        dmg = attackPower + (rand() % 5);
        std::cout << "  [" << name << "] 공격! (" << dmg << " 데미지)\n";
    }
    if (raged) {
        dmg *= 2;
        std::cout << "  \033[1;33m[분노의 일격 발동!] 데미지 2배!\033[0m\n";
    }
    return dmg;
}

void Warrior::onLevelUp() {
    if (level == 2) {
        activeSkills.push_back(ActiveSkill("분노의 일격",
            "다음 공격 데미지 2배! (2회/전투)", 2));
        std::cout << "  \033[1;33m[스킬 획득]\033[0m 분노의 일격을 배웠습니다!\n";
    }
}

int Warrior::useSkill(int idx) {
    if (idx < 0 || idx >= (int)activeSkills.size()) return 0;
    if (activeSkills[idx].usesLeft <= 0) {
        std::cout << "  \033[90m이 스킬은 이미 소진됐습니다.\033[0m\n";
        return 0;
    }
    activeSkills[idx].usesLeft--;
    if (idx == 0) {  // 분노의 일격
        rageStrikeActive = true;
        std::cout << "\033[1;33m  [분노의 일격] 다음 공격이 2배로 강해집니다!\033[0m\n";
    }
    return 0;
}

std::string Warrior::getJobName() const { return "전사"; }

void Warrior::resetBattleSkills() {
    Character::resetBattleSkills();  // 스킬 사용횟수 초기화
    powerStrikeReady = false;        // 강공격 상태 초기화
    rageStrikeActive = false;        // 분노 버프 초기화
}

// ── Mage ─────────────────────────────────────

Mage::Mage(const std::string& name)
    : Character(name, "마법사", 80, 25, 3),
      mana(60), maxMana(60) {}

int Mage::attack() {
    if (mana >= 20) {
        mana -= 20;
        int dmg = static_cast<int>(attackPower * 1.8) + (rand() % 8);
        std::cout << "  [" << name << "] ✦ 마법 공격! (" << dmg
                  << " 데미지) [MP: " << mana << "/" << maxMana << "]\n";
        return dmg;
    } else {
        int dmg = static_cast<int>(attackPower * 0.6) + (rand() % 3);
        std::cout << "  [" << name << "] 마나 부족... 일반 공격 (" << dmg << " 데미지)\n";
        return dmg;
    }
}

void Mage::onLevelUp() {
    if (level == 2) {
        activeSkills.push_back(ActiveSkill("마력 폭발",
            "현재 마나 전소, 강력한 마법 데미지! (1회/전투)", 1));
        std::cout << "  \033[1;33m[스킬 획득]\033[0m 마력 폭발을 배웠습니다!\n";
    }
}

int Mage::useSkill(int idx) {
    if (idx < 0 || idx >= (int)activeSkills.size()) return 0;
    if (activeSkills[idx].usesLeft <= 0) {
        std::cout << "  \033[90m이 스킬은 이미 소진됐습니다.\033[0m\n";
        return 0;
    }
    activeSkills[idx].usesLeft--;
    if (idx == 0) {  // 마력 폭발
        int burstDmg = std::max(1, mana * 2 + attackPower);
        std::cout << "\033[1;35m  [마력 폭발] 마나 " << mana
                  << " 전소! " << burstDmg << " 데미지!\033[0m\n";
        mana = 0;
        return burstDmg;
    }
    return 0;
}

void Mage::printStatus() const {
    float ratio = (float)hp / maxHp;
    const char* hpColor = (ratio > 0.5f) ? "\033[32m" : (ratio > 0.25f) ? "\033[33m" : "\033[1;31m";

    int barLen = 15;
    int mpFilled = maxMana > 0 ? (mana * barLen) / maxMana : 0;
    std::string mpBar = "[";
    for (int i = 0; i < barLen; i++)
        mpBar += (i < mpFilled) ? "\xE2\x96\x88" : "\xE2\x96\x91";
    mpBar += "]";

    std::cout << "\n  \033[1;37m╔══════════════════════════════╗\033[0m\n";
    std::cout << "    \033[1;36m[" << name << "]\033[0m  마법사  Lv." << level << "\n";
    std::cout << "    HP " << hpColor << makeHpBar(15) << "\033[0m " << hp << "/" << maxHp << "\n";
    std::cout << "    MP \033[34m" << mpBar << "\033[0m " << mana << "/" << maxMana << "\n";
    std::cout << "    ATK: \033[33m" << attackPower << "\033[0m  DEF: \033[36m" << defense << "\033[0m\n";
    std::cout << "    EXP: " << exp << " / " << expToNextLevel << "\n";
    std::cout << "    GOLD: \033[33m" << gold << "G\033[0m\n";
    if (!activeSkills.empty()) {
        std::cout << "    스킬: ";
        for (const auto& s : activeSkills)
            std::cout << "\033[35m" << s.name << "\033[0m(" << s.usesLeft << "/" << s.maxUses << ") ";
        std::cout << "\n";
    }
    std::cout << "\n    \033[90m※ 데미지 = 공격력 - 적 방어력 (최소 1)\033[0m\n";
    std::cout << "    \033[90m※ 크리티컬(15%) = 데미지 × 1.5\033[0m\n";
    std::cout << "  \033[1;37m╚══════════════════════════════╝\033[0m\n";
}

std::string Mage::getJobName() const { return "마법사"; }

// ── Healer ───────────────────────────────────

Healer::Healer(const std::string& name)
    : Character(name, "힐러", 100, 14, 5),
      healPerTurn(5) {}

int Healer::attack() {
    int dmg = attackPower + (rand() % 4);
    std::cout << "  [" << name << "] ✜ 신성 공격! (" << dmg << " 데미지)\n";
    return dmg;
}

void Healer::onLevelUp() {
    if (level == 2) {
        activeSkills.push_back(ActiveSkill("신성 치유",
            "HP 40% 즉시 회복! (2회/전투)", 2));
        std::cout << "  \033[1;33m[스킬 획득]\033[0m 신성 치유를 배웠습니다!\n";
    }
}

int Healer::useSkill(int idx) {
    if (idx < 0 || idx >= (int)activeSkills.size()) return 0;
    if (activeSkills[idx].usesLeft <= 0) {
        std::cout << "  \033[90m이 스킬은 이미 소진됐습니다.\033[0m\n";
        return 0;
    }
    activeSkills[idx].usesLeft--;
    if (idx == 0) {  // 신성 치유
        int healAmt = maxHp * 2 / 5;  // 40%
        heal(healAmt);
        std::cout << "\033[1;32m  [신성 치유] HP +" << healAmt
                  << " 회복! (현재: " << hp << "/" << maxHp << ")\033[0m\n";
    }
    return 0;
}

void Healer::onTurnEnd() {
    if (hp < maxHp) {
        heal(healPerTurn);
        std::cout << "  [" << name << "] 자동 회복 +" << healPerTurn
                  << " HP (현재: " << hp << "/" << maxHp << ")\n";
    }
}


void Healer::printStatus() const {
    float ratio = (float)hp / maxHp;
    const char* hpColor = (ratio > 0.5f) ? "\033[32m" : (ratio > 0.25f) ? "\033[33m" : "\033[1;31m";

    std::cout << "\n  \033[1;37m╔══════════════════════════════╗\033[0m\n";
    std::cout << "    \033[1;36m[" << name << "]\033[0m  힐러  Lv." << level << "\n";
    std::cout << "    HP " << hpColor << makeHpBar(15) << "\033[0m " << hp << "/" << maxHp << "\n";
    std::cout << "    자동회복: \033[32m+" << healPerTurn << "/턴\033[0m\n";
    std::cout << "    ATK: \033[33m" << attackPower << "\033[0m  DEF: \033[36m" << defense << "\033[0m\n";
    std::cout << "    EXP: " << exp << " / " << expToNextLevel << "\n";
    std::cout << "    GOLD: \033[33m" << gold << "G\033[0m\n";
    if (!activeSkills.empty()) {
        std::cout << "    스킬: ";
        for (const auto& s : activeSkills)
            std::cout << "\033[35m" << s.name << "\033[0m(" << s.usesLeft << "/" << s.maxUses << ") ";
        std::cout << "\n";
    }
    std::cout << "\n    \033[90m※ 데미지 = 공격력 - 적 방어력 (최소 1)\033[0m\n";
    std::cout << "    \033[90m※ 크리티컬(15%) = 데미지 × 1.5\033[0m\n";
    std::cout << "  \033[1;37m╚══════════════════════════════╝\033[0m\n";
}

std::string Healer::getJobName() const { return "힐러"; }
