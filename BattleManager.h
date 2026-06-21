#pragma once
#include "Character.h"
#include "Monster.h"
#include "Jobs.h"
#include <vector>
#include <string>

// ANSI 색상 코드
namespace Color {
    constexpr const char* RESET      = "\033[0m";
    constexpr const char* RED        = "\033[31m";
    constexpr const char* GREEN      = "\033[32m";
    constexpr const char* YELLOW     = "\033[33m";
    constexpr const char* CYAN       = "\033[36m";
    constexpr const char* MAGENTA    = "\033[35m";
    constexpr const char* BOLD_WHITE = "\033[1;37m";
    constexpr const char* BOLD_YEL   = "\033[1;33m";
    constexpr const char* BOLD_RED   = "\033[1;31m";
    constexpr const char* BOLD_CYAN  = "\033[1;36m";
    constexpr const char* BOLD_MAG   = "\033[1;35m";
}

enum class BattleResult { WIN, LOSE, RUN };

class BattleManager {
private:
    Character* player;
    Monster*   monster;
    int turnCount;
    mutable std::vector<std::string> battleLog;

    void addLog(const std::string& msg) const;
    void printBattleStatus() const;
    void printTurnMenu() const;
    int  getPlayerChoice() const;

    // 애니메이션
    static void sleepMs(int ms);
    void playAttackAnimation(bool critical, int dmg) const;
    void playVictoryAnimation() const;
    void playBattleStartAnimation() const;

public:
    BattleManager(Character* player, Monster* monster);
    BattleResult startBattle();
    bool processTurn(int choice, bool& ran);  // true=행동완료, false=재선택
};
