#include "BattleManager.h"
#include <iostream>
#include <stdexcept>
#include <limits>
#include <string>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

BattleManager::BattleManager(Character* player, Monster* monster)
    : player(player), monster(monster), turnCount(0) {}

// ── 유틸리티 ──────────────────────────────────

void BattleManager::sleepMs(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

void BattleManager::addLog(const std::string& msg) const {
    battleLog.push_back(msg);
    if (battleLog.size() > 3) battleLog.erase(battleLog.begin());
}

// ── 애니메이션 ────────────────────────────────

void BattleManager::playBattleStartAnimation() const {
    std::string art = monster->getAsciiArt();
    std::vector<std::string> lines;
    std::string line;
    for (char c : art) {
        if (c == '\n') { lines.push_back(line); line.clear(); }
        else            line += c;
    }
    if (!line.empty()) lines.push_back(line);

    std::cout << "\033[2J\033[3J\033[H";
    std::cout << Color::BOLD_WHITE
              << "========================================\n"
              << "         \033[1;31m⚠  적이 나타났다!  ⚠\033[1;37m\n"
              << "========================================"
              << Color::RESET << "\n\n";

    std::cout << Color::BOLD_MAG;
    for (const auto& l : lines) {
        std::cout << "  " << l << "\n";
        std::cout.flush();
        sleepMs(110);
    }
    std::cout << Color::RESET;
    std::cout << "\n" << Color::BOLD_YEL
              << "  【 " << monster->getMonsterType() << " 】"
              << Color::RESET << " 이(가) 나타났다!\n\n";
    sleepMs(700);
}

void BattleManager::playAttackAnimation(bool critical, int dmg) const {
    const int TRACK = 26;
    const std::string pName = player->getName();
    const std::string mName = monster->getMonsterType();

    const char* flashColors[] = {
        Color::BOLD_YEL, Color::BOLD_RED, Color::BOLD_WHITE, Color::BOLD_YEL
    };

    for (int frame = 0; frame < 4; frame++) {
        std::cout << "\033[2J\033[3J\033[H";
        std::cout.flush();

        if (critical) {
            std::cout << flashColors[frame]
                      << "  ╔═══════════════════════════╗\n"
                      << "  ║   ★★  CRITICAL HIT!!  ★★  ║\n"
                      << "  ╚═══════════════════════════╝\n"
                      << Color::RESET << "\n";
        } else {
            std::cout << "\n  " << Color::YELLOW << "[ 공격! ]" << Color::RESET << "\n\n";
        }

        bool isHit = (frame == 3);
        int pos = isHit ? (TRACK - 1) : (frame * (TRACK - 1) / 3);
        std::string track(TRACK, '-');
        if (!isHit) track[pos] = '>';

        std::cout << "  " << Color::CYAN << "[" << pName << "]" << Color::RESET
                  << " " << Color::YELLOW << track << Color::RESET;

        if (isHit) {
            std::cout << Color::BOLD_RED << " [HIT!] " << Color::RESET
                      << Color::BOLD_MAG << "[" << mName << "]" << Color::RESET << "\n";
            std::cout << "\n  " << Color::BOLD_YEL << "  " << dmg << " 데미지!!" << Color::RESET << "\n";
        } else {
            std::cout << "   " << Color::BOLD_MAG << "[" << mName << "]" << Color::RESET << "\n";
        }

        sleepMs(130);
    }
    sleepMs(350);
}

void BattleManager::playVictoryAnimation() const {
    struct Frame { const char* color; const char* text; };
    Frame frames[] = {
        { Color::YELLOW,     "              \xE2\x98\x85              " },
        { Color::BOLD_YEL,   "          \xE2\x98\x85  \xE2\x98\x85  \xE2\x98\x85          " },
        { Color::BOLD_WHITE, "       \xE2\x98\x85  \xE2\x98\x85  \xE2\x98\x85  \xE2\x98\x85  \xE2\x98\x85       " },
        { Color::BOLD_YEL,   "     \xE2\x98\x85\xE2\x98\x85\xE2\x98\x85  전투 승리!  \xE2\x98\x85\xE2\x98\x85\xE2\x98\x85     " },
    };
    for (int i = 0; i < 4; i++) {
        std::cout << "\033[2J\033[3J\033[H";
        std::cout << "\n\n";
        std::cout << frames[i].color << "  " << frames[i].text << Color::RESET << "\n";
        std::cout.flush();
        sleepMs(200);
    }
    sleepMs(400);
}

// ── UI ───────────────────────────────────────

void BattleManager::printBattleStatus() const {
    std::cout << "\033[2J\033[3J\033[H";
    std::cout.flush();

    std::cout << Color::BOLD_WHITE
              << "========================================\n"
              << "  ⚔  Turn " << turnCount << "\n"
              << "========================================" << Color::RESET << "\n";

    player->printStatus();

    std::cout << "\n" << Color::BOLD_MAG << monster->getAsciiArt() << Color::RESET;
    monster->printStatus();

    if (!battleLog.empty()) {
        std::cout << "\n" << Color::CYAN << "  ─── 전투 로그 ─────────────────────\n" << Color::RESET;
        for (const auto& log : battleLog)
            std::cout << "  " << log << "\n";
    }
    std::cout << Color::BOLD_WHITE << "========================================" << Color::RESET << "\n";
}

void BattleManager::printTurnMenu() const {
    // 스킬 보유 여부 확인
    bool hasSkill = false;
    for (const auto& s : player->getActiveSkills())
        if (s.usesLeft > 0) { hasSkill = true; break; }

    std::cout << "\n행동을 선택하세요:\n";
    std::cout << "  " << Color::YELLOW << "1" << Color::RESET << ". 공격\n";
    std::cout << "  " << Color::YELLOW << "2" << Color::RESET << ". 아이템 사용\n";
    std::cout << "  " << Color::YELLOW << "3" << Color::RESET << ". 도망\n";
    if (hasSkill)
        std::cout << "  " << Color::BOLD_MAG << "4" << Color::RESET
                  << ". \033[35m스킬 사용\033[0m\n";
    std::cout << "> ";
}

int BattleManager::getPlayerChoice() const {
    int choice;
    std::cin >> choice;
    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        throw std::invalid_argument("숫자를 입력해주세요.");
    }
    bool hasSkill = false;
    for (const auto& s : player->getActiveSkills())
        if (s.usesLeft > 0) { hasSkill = true; break; }
    int maxChoice = hasSkill ? 4 : 3;
    if (choice < 1 || choice > maxChoice)
        throw std::out_of_range("1~" + std::to_string(maxChoice) + " 사이의 숫자를 입력해주세요.");
    return choice;
}

// ── 전투 루프 ─────────────────────────────────

BattleResult BattleManager::startBattle() {
    player->resetBattleSkills();  // 매 전투 시작 시 스킬 사용횟수 초기화
    playBattleStartAnimation();

    bool ran = false;

    while (player->isAlive() && monster->isAlive()) {
        turnCount++;
        bool actionTaken = false;
        while (!actionTaken) {
            printBattleStatus();
            printTurnMenu();

            int choice = -1;
            while (choice == -1) {
                try {
                    choice = getPlayerChoice();
                } catch (const std::exception& e) {
                    std::cout << Color::RED << "[오류] " << e.what() << Color::RESET << "\n> ";
                }
            }
            actionTaken = processTurn(choice, ran);
        }

        if (ran) return BattleResult::RUN;
        if (!monster->isAlive()) break;

        // 몬스터 반격
        int monsterDmg = monster->attack();
        if (monsterDmg > 0) {
            player->takeDamage(monsterDmg);
            addLog(std::string(Color::RED) + "[반격] " + monster->getMonsterType()
                   + "의 공격! " + std::to_string(monsterDmg) + " 데미지!" + Color::RESET);
        }

        // HP 0 → 힐러 회복 전에 즉시 사망 처리
        if (!player->isAlive()) {
            std::cout << "\n" << Color::BOLD_RED
                      << player->getName() << "이(가) 쓰러졌습니다...\n"
                      << Color::RESET;
            return BattleResult::LOSE;
        }

        // Healer 자동 회복 (살아있을 때만)
        Healer* healer = dynamic_cast<Healer*>(player);
        if (healer) healer->onTurnEnd();
    }

    if (!monster->isAlive()) {
        playVictoryAnimation();
        std::cout << "\n" << Color::BOLD_YEL << "★ 전투 승리! ★" << Color::RESET << "\n";
        player->gainExp(monster->getExpReward());
        player->addGold(monster->getGoldReward());
        return BattleResult::WIN;
    }

    return BattleResult::LOSE;
}

bool BattleManager::processTurn(int choice, bool& ran) {
    switch (choice) {
        case 1: {
            bool critical = (rand() % 100 < 15);
            int dmg = player->attack();
            if (critical) dmg = static_cast<int>(dmg * 1.5);

            playAttackAnimation(critical, dmg);
            monster->takeDamage(dmg);

            if (critical) {
                addLog(std::string(Color::BOLD_YEL) + "[크리티컬!!] "
                       + std::to_string(dmg) + " 데미지!" + Color::RESET);
            } else {
                addLog(std::string(Color::GREEN) + "[공격] " + player->getName()
                       + "의 공격! " + std::to_string(dmg) + " 데미지!" + Color::RESET);
            }
            return true;
        }
        case 2: {
            player->printInventory();
            const std::vector<Item>& inv = player->getInventoryRef();
            if (inv.empty()) {
                std::cout << "  사용할 아이템이 없습니다.\n";
                return false;  // 재선택
            }
            std::cout << "사용할 아이템 번호 (취소: 0): ";
            int itemIdx;
            std::cin >> itemIdx;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (itemIdx == 0) return false;  // 취소 → 재선택
            if (itemIdx < 1 || itemIdx > (int)inv.size()) {
                std::cout << Color::RED << "[오류] 잘못된 번호입니다.\n" << Color::RESET;
                return false;  // 재선택
            }
            std::string itemName = inv[itemIdx - 1].getName();
            try {
                player->useItem(itemName);
                addLog(std::string(Color::CYAN) + "[아이템] " + itemName + " 사용!" + Color::RESET);
            } catch (const std::exception& e) {
                std::cout << Color::RED << "[오류] " << e.what() << "\n" << Color::RESET;
                return false;  // 재선택
            }
            return true;
        }
        case 3: {
            if (rand() % 2 == 0) {
                std::cout << "\n  " << Color::BOLD_YEL << "✦ 도망쳤다!" << Color::RESET << "\n";
                ran = true;
            } else {
                std::cout << Color::YELLOW << "  도망에 실패했습니다! 몬스터의 공격을 받습니다.\n" << Color::RESET;
                addLog("[도망 실패] 몬스터의 공격을 받습니다!");
            }
            return true;
        }
        case 4: {
            player->printSkills();
            const std::vector<ActiveSkill>& skills = player->getActiveSkills();

            bool anyUsable = false;
            for (const auto& s : skills) if (s.usesLeft > 0) { anyUsable = true; break; }
            if (!anyUsable) {
                std::cout << "  이번 전투에서 모든 스킬을 소진했습니다.\n";
                return false;  // 재선택
            }

            std::cout << "스킬 번호 선택 (취소: 0): ";
            int si;
            std::cin >> si;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (si == 0) return false;  // 취소 → 재선택
            si--;  // 0-indexed

            if (si < 0 || si >= (int)skills.size() || skills[si].usesLeft <= 0) {
                std::cout << Color::RED << "[오류] 사용할 수 없는 스킬입니다.\n" << Color::RESET;
                return false;  // 재선택
            }

            std::string skillName = skills[si].name;
            int skillDmg = player->useSkill(si);
            if (skillDmg > 0) {
                monster->takeDamage(skillDmg);
                addLog(std::string(Color::BOLD_MAG) + "[스킬] " + skillName
                       + " " + std::to_string(skillDmg) + " 데미지!" + Color::RESET);
            } else {
                addLog(std::string(Color::BOLD_MAG) + "[스킬] " + skillName + " 사용!" + Color::RESET);
            }
            return true;
        }
    }
    return true;
}
