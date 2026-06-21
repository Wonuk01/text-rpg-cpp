#include "Monster.h"
#include "BattleManager.h"
#include <iostream>
#include <cstdlib>

// ── Monster 기반 ──────────────────────────────

Monster::Monster(const std::string& name, int hp, int attackPower, int defense, int expReward, int goldReward)
    : Entity(name, hp, attackPower, defense), expReward(expReward), goldReward(goldReward) {}

void Monster::printStatus() const {
    float ratio = (float)hp / maxHp;
    const char* hpColor;
    if      (ratio > 0.5f)  hpColor = Color::GREEN;
    else if (ratio > 0.25f) hpColor = Color::YELLOW;
    else                    hpColor = Color::BOLD_RED;

    std::cout << "  " << Color::BOLD_MAG << "[" << name << "]" << Color::RESET
              << " HP " << hpColor << makeHpBar(15) << Color::RESET
              << " " << hp << "/" << maxHp << "\n";
}

int Monster::getExpReward() const { return expReward; }
int Monster::getGoldReward() const { return goldReward; }

// ── Goblin ───────────────────────────────────

Goblin::Goblin() : Monster("고블린", 50, 12, 2, 30, 15) {
}

std::string Goblin::getAsciiArt() const {
    return
        "      ,      ,      \n"
        "       /(.-\"\"-.)\\   \n"
        "   |\\  \\/      \\/  /|  \n"
        "   | \\ / =.  .= \\ / |  \n"
        "   \\( \\   o\\/o   / )/  \n"
        "    \\_, '-/  \\-' ,_/   \n"
        "      /   \\__/   \\     \n"
        "      \\ \\__/\\__/ /    \n"
        "    ___\\ \\|--|/ /___   \n"
        "  /`    \\      /    `\\  \n"
        " /       '----'       \\  \n";
}

int Goblin::attack() {
    int dmg = attackPower + (rand() % 4);
    if (rand() % 10 < 7) {
        int dmg2 = (attackPower / 2) + (rand() % 3);
        std::cout << Color::MAGENTA << "[고블린] 연속 공격!"
                  << Color::RESET << " (" << dmg << " + " << dmg2 << " 데미지)\n";
        return dmg + dmg2;
    }
    std::cout << Color::MAGENTA << "[고블린] 공격!"
              << Color::RESET << " (" << dmg << " 데미지)\n";
    return dmg;
}

std::string Goblin::getMonsterType() const { return "고블린"; }

// ── Wolf ─────────────────────────────────────

Wolf::Wolf() : Monster("늑대", 80, 18, 4, 60, 30), nextAttackBuffed(false) {
}

std::string Wolf::getAsciiArt() const {
    return
        "           _        _\n"
        "          /\\\\     ,'/|\n"
        "        _|  |\\-'-'_/_/\n"
        "   __--'/`           \\\n"
        "       /              \\\n"
        "      /        \"o.  |o\"|\n"
        "      |              \\/\n"
        "       \\_          ___\\\n"
        "         `--._`.   \\;//\n"
        "              ;-.___,'\n"
        "             /\n"
        "           ,'\n"
        "        _-'\n";
}

int Wolf::attack() {
    int dmg = attackPower + (rand() % 6);
    if (nextAttackBuffed) {
        nextAttackBuffed = false;
        int buffedDmg = static_cast<int>(dmg * 1.8);
        std::cout << Color::BOLD_RED << "[늑대] 강화된 발톱 공격!!"
                  << Color::RESET << " (" << buffedDmg << " 데미지)\n";
        return buffedDmg;
    }
    if (rand() % 4 == 0) {
        nextAttackBuffed = true;
        std::cout << Color::BOLD_YEL << "[늑대] 울부짖음!! (다음 공격 1.8배 강화)"
                  << Color::RESET << "\n";
        return 0;
    }
    std::cout << Color::MAGENTA << "[늑대] 발톱 공격!"
              << Color::RESET << " (" << dmg << " 데미지)\n";
    return dmg;
}

std::string Wolf::getMonsterType() const { return "늑대"; }

// ── Crocodile ────────────────────────────────

Crocodile::Crocodile() : Monster("악어", 120, 20, 7, 90, 50), biteCount(0) {
}

std::string Crocodile::getAsciiArt() const {
    return
        "   .-._   _ _ _ _ _\n"
        ".-''-.__.-'00  '-' ' ' ' '-.   \n"
        "'.___ '    .   .--_'-' '-'_'-' '._\n"
        " V: V 'vv-'   '_   '.       .'  _..' '.'\n"
        "   '=.____.=_.--'   :_.__.__:_   '.   : :\n"
        "           (((____.-'        '-.  /   : :\n"
        "                             (((-'\\ .' /\n"
        "                          _____..'  .'\n"
        "                         '-._____.-'\n";
}

int Crocodile::attack() {
    biteCount++;
    // 3턴마다 데스롤 (강력한 회전 공격)
    if (biteCount % 3 == 0) {
        int dmg = static_cast<int>(attackPower * 1.8) + (rand() % 8);
        std::cout << Color::BOLD_RED << "[악어] 데스롤!!"
                  << Color::RESET <<  " (" << dmg << " 데미지)\n";
        return dmg;
    }
    int dmg = attackPower + (rand() % 6);
    std::cout << Color::MAGENTA << "[악어] 물기!"
              << Color::RESET << " (" << dmg << " 데미지)\n";
    return dmg;
}

std::string Crocodile::getMonsterType() const { return "악어"; }

// ── Dragon ────────────────────────────────────────────

Dragon::Dragon() : Monster("드래곤", 200, 28, 10, 150, 200), breathCount(0) {
}

std::string Dragon::getAsciiArt() const {
    return
        "                                                       ____________\n"
        "                                 (`-..________....---''  ____..._.-`\n"
        "                                  \\`._______.._,.---'''     ,'\n"
        "                                  ; )`.      __..-'`-.      /\n"
        "                                 / /     _.-' _,.;;._ `-._,'\n"
        "                                / /   ,-' _.-'  //   ``--._``._\n"
        "                              ,','_.-' ,-' _.- (( =-    -. `-._`-._____\n"
        "                            ,;.''__..-'   _..--.\\\\.--'````--.._``-.`-._`.\n"
        "             _          |\\,' .-''        ```-'`---'`-...__,._  ``-.`-.`-.`.\n"
        "  _     _.-,'(__)\\__)\\-'' `     ___  .          `     \\      `--._\n"
        ",',)---' /|)          `     `      ``-.   `     /     /     `     `-.\n"
        "\\_____--.  '`  `               __..-.  \\     . (   < _...-----..._   `.\n"
        " \\_,--..__. \\\\ .-`.\\----'';``,..-.__ \\  \\      ,`_. `.,-'`--'`---''`.  )\n"
        "           `.\\`.\\  `_.-..' ,'   _,-..'  /..,-''(, ,' ; ( _______`___..'__\n"
        "                   ((,(,__(    ((,(,__,'  ``'-- `'`.(\\  `.,..______   SSt\n"
        "                                                      ``--------..._``--.__\n";
}

int Dragon::attack() {
    breathCount++;
    if (breathCount % 3 == 0) {
        int dmg = attackPower * 2 + (rand() % 10);
        std::cout << Color::BOLD_RED << "[드래곤] 불꽃 브레스!!"
                  << Color::RESET << " (" << dmg << " 데미지)\n";
        return dmg;
    }
    int dmg = attackPower + (rand() % 8);
    std::cout << Color::MAGENTA << "[드래곤] 발톱 공격!"
              << Color::RESET << " (" << dmg << " 데미지)\n";
    return dmg;
}

std::string Dragon::getMonsterType() const { return "드래곤"; }
