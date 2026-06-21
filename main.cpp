#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <stdexcept>
#include <limits>
#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#include <cstdio>
#pragma comment(lib, "winmm.lib")
#endif
#include "Jobs.h"
#include "Monster.h"
#include "BattleManager.h"
#include "SaveManager.h"

// ── 오디오 ───────────────────────────────────
namespace Audio {
    static int  volume = 70;   // 0~100
    static bool muted  = false;
    static bool opened = false;

    void play(const char* file) {
#ifdef _WIN32
        if (opened) {
            mciSendStringA("stop bgm", NULL, 0, NULL);
            mciSendStringA("close bgm", NULL, 0, NULL);
        }
        char cmd[512];
        std::snprintf(cmd, sizeof(cmd), "open \"%s\" type mpegvideo alias bgm", file);
        if (mciSendStringA(cmd, NULL, 0, NULL) == 0) {
            mciSendStringA("play bgm repeat", NULL, 0, NULL);
            std::snprintf(cmd, sizeof(cmd), "setaudio bgm volume to %d", volume * 10);
            mciSendStringA(cmd, NULL, 0, NULL);
            opened = true;
        }
#endif
    }

    void stop() {
#ifdef _WIN32
        if (opened) {
            mciSendStringA("stop bgm", NULL, 0, NULL);
            mciSendStringA("close bgm", NULL, 0, NULL);
            opened = false;
        }
#endif
    }

    void applyVolume() {
#ifdef _WIN32
        if (!opened) return;
        char cmd[64];
        int v = muted ? 0 : volume * 10;
        std::snprintf(cmd, sizeof(cmd), "setaudio bgm volume to %d", v);
        mciSendStringA(cmd, NULL, 0, NULL);
#endif
    }

    void setVolume(int v) {
        volume = v < 0 ? 0 : (v > 100 ? 100 : v);
        applyVolume();
    }

    void toggleMute() {
        muted = !muted;
        applyVolume();
    }
}

void clearScreen() {
    std::cout << "\033[2J\033[3J\033[H";
    std::cout.flush();
}

void pauseScreen() {
    std::cout << "\n  \033[90m아무 키나 누르면 계속...\033[0m";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (std::cin.peek() == '\n') std::cin.ignore();
}

void printTitle() {
    std::cout << "\n";
    std::cout << "  +===============================+\n";
    std::cout << "  |    텍스트 RPG 어드벤처        |\n";
    std::cout << "  +===============================+\n\n";
}

static std::vector<std::string> splitArt(const char* art) {
    std::vector<std::string> lines;
    const char* p = art;
    const char* start = p;
    while (*p) {
        if (*p == '\n') {
            lines.emplace_back(start, p);
            start = p + 1;
        }
        p++;
    }
    if (start != p) lines.emplace_back(start, p);
    return lines;
}

static void printJobCols(
    const std::vector<std::string>& a,
    const std::vector<std::string>& b,
    const std::vector<std::string>& c,
    int wa, int wb, int gap)
{
    size_t maxH = std::max(a.size(), std::max(b.size(), c.size()));
    size_t padA = (maxH - a.size()) / 2;
    size_t padB = (maxH - b.size()) / 2;
    size_t padC = (maxH - c.size()) / 2;
    std::string sp(gap, ' ');
    for (size_t i = 0; i < maxH; i++) {
        const std::string& la = (i >= padA && i < padA + a.size()) ? a[i - padA] : "";
        const std::string& lb = (i >= padB && i < padB + b.size()) ? b[i - padB] : "";
        const std::string& lc = (i >= padC && i < padC + c.size()) ? c[i - padC] : "";
        std::cout << la << std::string(std::max(0, wa - (int)la.size()), ' ') << sp
                  << lb << std::string(std::max(0, wb - (int)lb.size()), ' ') << sp
                  << lc << "\n";
    }
}

Character* createCharacter() {
    clearScreen();
    std::cout << "\n\033[1;37m  ========================================================\n";
    std::cout << "              캐릭터를 생성합니다\n";
    std::cout << "  ========================================================\033[0m\n\n";
    std::string name;
    std::cout << "  캐릭터 이름을 입력하세요: ";
    std::getline(std::cin, name);
    if (name.empty()) name = "용사";

    clearScreen();
    std::cout << "\n\033[1;37m  ========================================================\n";
    std::cout << "                   직업을 선택하세요\n";
    std::cout << "  ========================================================\033[0m\n\n";

    std::string h1 = "\033[1;33m  [1] 전사\033[0m";
    std::string h2 = "\033[1;35m  [2] 마법사\033[0m";
    std::string h3 = "\033[1;32m  [3] 힐러\033[0m";
    int gap = 4;
    std::cout << h1 << std::string(37 - 8, ' ') << std::string(gap, ' ')
              << h2 << std::string(43 - 10, ' ') << std::string(gap, ' ')
              << h3 << "\n";
    std::cout << std::string(37, '-') << std::string(gap, ' ')
              << std::string(43, '-') << std::string(gap, ' ')
              << std::string(37, '-') << "\n";

    const char* wArt =
        "  ,   A           {}\n"
        " / \\, | ,        .---.\n"
        "|    =|= >      /.--.\\\n"
        " \\ /` | `       |====|\n"
        "  `   |         |`::` |\n"
        "      |     .-;`\\..../`;_.-^-._\n"
        "     /\\\\/  /  |...::..|`   :   `|\n"
        "     |:'\\ |   /'''::''|   .:.   |\n"
        "      \\ /\\;-,/\\   ::  |..:::::.. |\n"
        "      |\\ <` >  >._::_.| ':::::' |\n"
        "      | `\"\"`  /   ^^  |   ':'   |\n"
        "      |       |       \\    :    /\n"
        "      |       |        \\   :   /\n"
        "      |       |___/\\___|`-.:.-`\n"
        "      |        \\_ || _/    `\n"
        "      |        <_ >< _>\n"
        "      |        |  ||  |\n"
        "      |        |  ||  |\n"
        "      |       _\\.:||:./_\n"
        "      | jgs  /____/\\____\\\n";
    const char* mArt =
        "                          ,---.\n"
        "                         /    |\n"
        "                        /     |\n"
        "                       /      |\n"
        "                      /       |\n"
        "                 ___,'        |\n"
        "               <  -'          :\n"
        "                `-.__..--'``-,_\\_\n"
        "                   |o/ ` :,.)_`>\n"
        "                   :/ `     ||/)\n"
        "                   (_.).__,-` |\\\n"
        "                   /( `.``   `| :\n"
        "                   \\'`-.)  `  ; ;\n"
        "                   | `       /-<\n"
        "                   |     `  /   `.\n"
        "   ,-_-..____     /|  `    :__..-'\\\n"
        "  /,'-.__\\\\  ``-./ :`      ;       \\\n"
        "  `\\ `\\  `\\\\  \\ :  (   `  /  ,   `. \\\n"
        "    \\` \\   \\\\   |  | `   :  :     .\\ \\\n"
        "     \\ `\\_  ))  :  ;     |  |      ): :\n"
        "    (`-.-'\\ ||  |\\ \\   ` ;  ;       | |\n"
        "     \\-_   `;;._   ( `  /  /_       | |\n"
        "      `-.-.// ,'`-._\\__/_,'         ; |\n"
        "         \\:: :     /     `     ,   /  |\n"
        "          || |    (        ,' /   /   |\n"
        "          ||                ,'   / SSt|\n";
    const char* hArt =
        "                    ,-----.\n"
        "                   #,-. ,-.#\n"
        "                  () a   e ()\n"
        "                  (   (_)   )\n"
        "                  #\\_  -  _/#\n"
        "                ,'   `\"\"\"` `.\n"
        "              ,'      \\X/      `.\n"
        "             /         X     ____\\\n"
        "            /          v   ,`  v  `,\n"
        "           /    /         ( <==+==>) \n"
        "           `-._/|__________\\   ^   /\n"
        "          (\\\\)  |______@____\\  ^  /\n"
        "            \\\\  |     ( )    \\ ^ /\n"
        "             )  |             \\^/\n"
        "            (   |             |v\n"
        "           <(^)>|             |\n"
        "             v  |             |\n"
        "                |             |\n"
        "        ZOT     |_.--.__ .--._|\n"
        "                  `==='  `==='\n";

    printJobCols(splitArt(wArt), splitArt(mArt), splitArt(hArt), 37, 43, gap);

    std::cout << std::string(37, '-') << std::string(gap, ' ')
              << std::string(43, '-') << std::string(gap, ' ')
              << std::string(37, '-') << "\n";

    std::string s1 = "  HP:\033[32m120\033[0m ATK:\033[31m18\033[0m DEF:\033[36m8\033[0m";
    std::string s2 = "  HP:\033[32m80\033[0m  ATK:\033[31m25\033[0m DEF:\033[36m3\033[0m";
    std::string s3 = "  HP:\033[32m100\033[0m ATK:\033[31m14\033[0m DEF:\033[36m5\033[0m";
    std::cout << s1 << std::string(37 - 22, ' ') << std::string(gap, ' ')
              << s2 << std::string(43 - 22, ' ') << std::string(gap, ' ')
              << s3 << "\n";

    std::string t1 = "  2턴마다 강공격";
    std::string t2 = "  마나 소모 마법 공격";
    std::string t3 = "  매 턴 자동 HP 회복";
    std::cout << t1 << std::string(37 - (int)t1.size(), ' ') << std::string(gap, ' ')
              << t2 << std::string(43 - (int)t2.size(), ' ') << std::string(gap, ' ')
              << t3 << "\n\n";

    // 커서를 74행에 고정해서 입력해도 위 아트가 스크롤되지 않음
    std::cout << "\033[74;1H\033[1;37m  직업 번호 입력 > \033[0m";
    int choice;
    while (true) {
        std::cin >> choice;
        std::cin.ignore();
        if (choice >= 1 && choice <= 3) break;
        std::cout << "\033[74;1H\033[31m  잘못된 입력. 1~3 중 선택:           \033[0m";
        std::cout << "\033[74;30H";
    }

    Character* player = nullptr;
    if (choice == 1) player = new Warrior(name);
    else if (choice == 2) player = new Mage(name);
    else player = new Healer(name);

    clearScreen();
    std::cout << "\n\033[1;32m  환영합니다, " << name << " 님!\033[0m\n";
    player->printStatus();
    pauseScreen();
    return player;
}

Monster* selectMonster(int stage) {
    if (stage <= 2) return new Goblin();
    if (stage <= 3) return new Wolf();
    if (stage <= 5) return new Crocodile();
    return new Dragon();
}

bool isBossStage(int stage) {
    return stage >= 7;
}

void visitShop(Character* player) {
    clearScreen();
    std::cout << "\n\033[1;33m  =============================================\n";
    std::cout << "                   마을 상점\n";
    std::cout << "  =============================================\033[0m\n";
    std::cout << "  현재 골드: \033[33m" << player->getGold() << "G\033[0m\n\n";

    while (true) {
        std::cout << "  [1] HP 포션      (\033[33m30G\033[0m) - HP +50 회복\n";
        std::cout << "  [2] 공격 포션    (\033[33m50G\033[0m) - ATK +8 (전투 중 사용)\n";
        std::cout << "  [3] 방어 강화    (\033[33m50G\033[0m) - DEF +3 (즉시 적용)\n";
        std::cout << "  [0] 떠나기\n\n";
        std::cout << "  > ";
        int ch;
        std::cin >> ch;
        std::cin.ignore();
        if (ch == 0) break;
        if (ch == 1) {
            if (player->spendGold(30)) {
                player->addItem(Item("HP 포션", "heal", 50, "HP +50 회복"));
                std::cout << "\033[32m  HP 포션을 인벤토리에 추가했습니다.\033[0m\n\n";
            } else {
                std::cout << "\033[31m  골드가 부족합니다.\033[0m\n\n";
            }
        } else if (ch == 2) {
            if (player->spendGold(50)) {
                player->addItem(Item("공격 포션", "attack", 8, "ATK +8 강화"));
                std::cout << "\033[32m  공격 포션을 인벤토리에 추가했습니다.\033[0m\n\n";
            } else {
                std::cout << "\033[31m  골드가 부족합니다.\033[0m\n\n";
            }
        } else if (ch == 3) {
            if (player->spendGold(50)) {
                player->setDefense(player->getDefense() + 3);
                std::cout << "\033[32m  방어력 +3 (즉시 적용)\033[0m\n\n";
            } else {
                std::cout << "\033[31m  골드가 부족합니다.\033[0m\n\n";
            }
        }
        std::cout << "  현재 골드: \033[33m" << player->getGold() << "G\033[0m\n\n";
    }
}

void visitInventory(Character* player) {
    clearScreen();
    std::cout << "\n\033[1;36m  === 인벤토리 ===\033[0m\n";
    player->printInventory();
    pauseScreen();
}

void showAudioSettings() {
    while (true) {
        clearScreen();
        std::cout << "\n\033[1;37m  =============================================\n";
        std::cout << "                   음악 설정\n";
        std::cout << "  =============================================\033[0m\n\n";

        int bars = Audio::volume / 5;
        std::cout << "  음량: \033[1;33m[";
        for (int i = 0; i < 20; i++)
            std::cout << (i < bars ? "\xe2\x96\x88" : "-");
        std::cout << "]\033[0m " << Audio::volume << "%";
        if (Audio::muted) std::cout << "  \033[31m[뮤트]\033[0m";
        std::cout << "\n\n";
        std::cout << "  [1] 음량 올리기  (+10)\n";
        std::cout << "  [2] 음량 내리기  (-10)\n";
        std::cout << "  [3] 뮤트 / 해제\n";
        std::cout << "  [0] 돌아가기\n\n";
        std::cout << "  > ";
        int ch; std::cin >> ch; std::cin.ignore();
        if (ch == 0) break;
        else if (ch == 1) Audio::setVolume(Audio::volume + 10);
        else if (ch == 2) Audio::setVolume(Audio::volume - 10);
        else if (ch == 3) Audio::toggleMute();
    }
}

void showEnding(Character* player) {
    // ── 엔딩 연출 ──
    clearScreen();
    std::cout << "\n\n\n";
    std::cout << "\033[1;33m";
    std::cout << "         ╔══════════════════════════════════════════╗\n";
    std::cout << "         ║           드래곤이 쓰러졌다...           ║\n";
    std::cout << "         ╚══════════════════════════════════════════╝\n";
    std::cout << "\033[0m\n\n";
    std::cout << "  \033[37m" << player->getName() << " 은(는) 긴 싸움 끝에 드래곤을 물리쳤다.\033[0m\n";
    std::cout << "  \033[37m왕국에는 오랜만에 평화가 찾아왔고,\033[0m\n";
    std::cout << "  \033[37m사람들은 그 이름을 영웅이라 불렀다.\033[0m\n\n";
    pauseScreen();

    // ── 크레딧 ──
    clearScreen();
    std::cout << "\n\n\n";
    std::cout << "\033[1;37m";
    std::cout << "         ╔══════════════════════════════════════════╗\n";
    std::cout << "         ║                  F I N                   ║\n";
    std::cout << "         ║           텍스트 RPG 어드벤처            ║\n";
    std::cout << "         ╚══════════════════════════════════════════╝\n";
    std::cout << "\033[0m\n\n";

    std::cout << "  \033[90m──────────────────────────────────────────────\033[0m\n\n";

    std::cout << "  \033[1;33m  기획 · 개발\033[0m\n";
    std::cout << "  \033[37m      원욱\033[0m\n\n";

    std::cout << "  \033[1;33m  등장 몬스터\033[0m\n";
    std::cout << "  \033[37m      고블린  ·  늑대  ·  악어  ·  드래곤\033[0m\n\n";

    std::cout << "  \033[1;33m  플레이어 직업\033[0m\n";
    std::cout << "  \033[37m      전사  ·  마법사  ·  힐러\033[0m\n\n";

    std::cout << "  \033[1;33m  ASCII Art\033[0m\n";
    std::cout << "  \033[37m      jgs  ·  SSt  ·  ZOT\033[0m\n\n";

    std::cout << "  \033[1;33m  클리어 스테이지\033[0m\n";
    std::cout << "  \033[37m      " << player->getName() << "  (";
    std::cout << player->getJobName() << " Lv." << player->getLevel() << ")\033[0m\n\n";

    std::cout << "  \033[90m──────────────────────────────────────────────\033[0m\n\n";
    std::cout << "  \033[1;32m         플레이해주셔서 감사합니다!\033[0m\n\n\n";
    pauseScreen();
}

int showMainMenu() {
    while (true) {
        clearScreen();
        printTitle();
        std::cout << "  [1] 새 게임\n";
        std::cout << "  [2] 불러오기\n";
        std::cout << "  [3] 설정 (음악: "
                  << Audio::volume << "%" << (Audio::muted ? " 뮤트" : "") << ")\n";
        std::cout << "  [4] 종료\n\n";
        std::cout << "  > ";
        int ch; std::cin >> ch; std::cin.ignore();
        if (ch == 3) { showAudioSettings(); continue; }
        return ch;
    }
}

void gameLoop(Character* player, int startStage = 1) {
    SaveManager save;
    int stage = startStage;

    while (true) {
        if (stage > 1) {
            bool goNext = false;
            while (!goNext) {
                clearScreen();
                std::cout << "\n\033[1;32m  === 마을 ===\033[0m\n";
                std::cout << "  현재 스테이지: " << stage << "\n";
                std::cout << "  [1] 상점\n";
                std::cout << "  [2] 인벤토리\n";
                std::cout << "  [3] 상태 확인\n";
                std::cout << "  [4] 다음 스테이지로\n";
                std::cout << "  [5] 저장\n\n";
                std::cout << "  > ";
                int ch; std::cin >> ch; std::cin.ignore();
                if (ch == 1) visitShop(player);
                else if (ch == 2) visitInventory(player);
                else if (ch == 3) {
                    clearScreen();
                    player->printStatus();
                    pauseScreen();
                }
                else if (ch == 4) goNext = true;
                else if (ch == 5) {
                    try {
                        save.save(player, stage);
                        pauseScreen();
                    } catch (const std::exception& e) {
                        std::cout << "\033[31m  저장 실패: " << e.what() << "\033[0m\n";
                        pauseScreen();
                    }
                }
            }
        }

        if (isBossStage(stage)) {
            showEnding(player);
            break;
        }

        Monster* monster = selectMonster(stage);
        BattleManager bm(player, monster);
        BattleResult result = bm.startBattle();
        delete monster;

        if (result == BattleResult::LOSE) {
            clearScreen();
            std::cout << "\n\n  \033[1;31m==============================\033[0m\n";
            std::cout << "  \033[1;31m       GAME  OVER\033[0m\n";
            std::cout << "  \033[1;31m==============================\033[0m\n\n";
            pauseScreen();
            break;
        }
        if (result == BattleResult::RUN) continue;
        stage++;
    }
}

int main() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    SMALL_RECT minRect = {0, 0, 1, 1};
    SetConsoleWindowInfo(hOut, TRUE, &minRect);
    COORD bufSize = {220, 80};
    SetConsoleScreenBufferSize(hOut, bufSize);
    SMALL_RECT winRect = {0, 0, 219, 79};
    SetConsoleWindowInfo(hOut, TRUE, &winRect);
#endif
    srand(static_cast<unsigned>(time(nullptr)));
    Audio::play("bgm.mp3");
    while (true) {
        int menu = showMainMenu();
        if (menu == 4) { Audio::stop(); break; }

        Character* player = nullptr;
        if (menu == 1) {
            player = createCharacter();
        } else if (menu == 2) {
            SaveManager save;
            int stage = 1;
            try {
                player = save.load(stage);
            } catch (const std::exception& e) {
                clearScreen();
                std::cout << "  \033[31m" << e.what() << "\033[0m\n";
                pauseScreen();
                continue;
            }
            if (!player) {
                clearScreen();
                std::cout << "  저장 데이터가 없습니다.\n";
                pauseScreen();
                continue;
            }
            pauseScreen();
            gameLoop(player, stage);
            delete player;
            continue;
        } else {
            continue;
        }

        if (player) {
            gameLoop(player);
            delete player;
        }
    }
    return 0;
}
