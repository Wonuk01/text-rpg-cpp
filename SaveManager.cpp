#include "SaveManager.h"
#include "Jobs.h"
#include <fstream>
#include <iostream>
#include <stdexcept>

SaveManager::SaveManager(const std::string& filePath)
    : saveFilePath(filePath) {}

bool SaveManager::saveFileExists() const {
    std::ifstream f(saveFilePath);
    return f.good();
}

void SaveManager::save(const Character* player, int stage) {
    std::ofstream file(saveFilePath);

    // 예외 처리 ④ - 파일 열기 실패
    if (!file.is_open()) {
        throw std::runtime_error("저장 파일을 열 수 없습니다: " + saveFilePath);
    }

    file << player->getName() << "\n";
    file << player->getJob() << "\n";
    file << player->getLevel() << "\n";
    file << player->getExp() << "\n";
    file << player->getHp() << "\n";
    file << player->getMaxHp() << "\n";
    file << player->getAttackPower() << "\n";
    file << player->getDefense() << "\n";
    file << stage << "\n";  // 스테이지 저장
    file << player->getGold() << "\n";  // 골드 저장

    // 인벤토리 저장
    std::vector<Item> inv = player->getInventory();
    file << inv.size() << "\n";
    for (const Item& item : inv) {
        file << item.getName() << "\n";
        file << item.getType() << "\n";
        file << item.getEffectValue() << "\n";
        file << item.getDescription() << "\n";
    }

    file.close();
    std::cout << "[저장 완료] " << saveFilePath << " (스테이지 " << stage << ")\n";
}

Character* SaveManager::load(int& stage) {
    std::ifstream file(saveFilePath);

    // 예외 처리 ⑤ - 저장 파일 없음
    if (!file.is_open()) {
        throw std::runtime_error("저장 파일이 없습니다. 새 게임을 시작해주세요.");
    }

    std::string name, job;
    int level, exp, hp, maxHp, attackPower, defense, gold;

    std::getline(file, name);
    std::getline(file, job);
    file >> level >> exp >> hp >> maxHp >> attackPower >> defense >> stage >> gold;

    // 직업에 따라 파생 클래스 생성
    Character* player = nullptr;
    if (job == "전사") {
        player = new Warrior(name);
    } else if (job == "마법사") {
        player = new Mage(name);
    } else if (job == "힐러") {
        player = new Healer(name);
    } else {
        throw std::runtime_error("알 수 없는 직업 데이터: " + job);
    }

    // 스탯 복원 (저장된 값으로 덮어쓰기)
    player->setMaxHp(maxHp);
    player->setHp(hp);
    player->setAttackPower(attackPower);
    player->setDefense(defense);
    player->setLevel(level);
    player->setExp(exp);
    // expToNextLevel: level에 맞게 재계산 (100 * 1.5^(level-1))
    int etl = 100;
    for (int i = 1; i < level; i++) etl = static_cast<int>(etl * 1.5);
    player->setExpToNextLevel(etl);

    // 골드 복원 (메시지 없이)
    player->addGold(gold);

    // 인벤토리 불러오기
    int itemCount;
    file >> itemCount;
    file.ignore();

    for (int i = 0; i < itemCount; i++) {
        std::string iName, iType, iDesc;
        int iVal;
        std::getline(file, iName);
        std::getline(file, iType);
        file >> iVal;
        file.ignore();
        std::getline(file, iDesc);
        player->addItem(Item(iName, iType, iVal, iDesc));
    }

    file.close();
    std::cout << "[불러오기 완료] " << name << " (" << job << ") Lv." << level << " / 스테이지 " << stage << "\n";
    return player;
}
