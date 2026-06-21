#pragma once
#include "Character.h"
#include <string>

// =============================================
// SaveManager : 게임 데이터 파일 저장/불러오기
// 파일 I/O 요건 담당
// =============================================
class SaveManager {
private:
    std::string saveFilePath;

public:
    SaveManager(const std::string& filePath = "savefile.txt");

    void save(const Character* player, int stage);  // 예외 처리 ④
    Character* load(int& stage);                    // 예외 처리 ⑤
    bool saveFileExists() const;
};
