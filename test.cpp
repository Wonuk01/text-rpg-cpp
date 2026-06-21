#include <iostream>
#include <string>
#ifdef _WIN32
#include <windows.h>
#endif
#include "Jobs.h"

// ── 테스트 유틸 ──────────────────────────────────────────
static int passed = 0, failed = 0;

void check(const std::string& name, bool condition) {
    if (condition) {
        std::cout << "[PASS] " << name << "\n";
        ++passed;
    } else {
        std::cout << "[FAIL] " << name << "\n";
        ++failed;
    }
}

// ── 테스트 케이스 ────────────────────────────────────────

// 1. 초기 HP 확인
void test_initial_hp() {
    Warrior w("테스트");
    Mage    m("테스트");
    Healer  h("테스트");
    check("Warrior 초기 HP == 120", w.getHp() == 120);
    check("Mage 초기 HP == 80",     m.getHp() == 80);
    check("Healer 초기 HP == 100",  h.getHp() == 100);
}

// 2. takeDamage 정상 계산 (데미지 - 방어력, 최소 1)
void test_take_damage_normal() {
    Warrior w("테스트");   // 방어력 8
    int hpBefore = w.getHp();
    w.takeDamage(18);      // 18 - 8 = 10 데미지
    check("takeDamage 정상 계산 (18-8=10)", w.getHp() == hpBefore - 10);
}

// 3. takeDamage 경계: 0 데미지 → HP 변화 없음
void test_take_damage_zero() {
    Warrior w("테스트");
    int hpBefore = w.getHp();
    w.takeDamage(0);
    check("takeDamage(0) → HP 변화 없음 [경계]", w.getHp() == hpBefore);
}

// 4. takeDamage 경계: 방어력보다 낮은 공격 → 최소 1 데미지
void test_take_damage_min_one() {
    Warrior w("테스트");   // 방어력 8
    int hpBefore = w.getHp();
    w.takeDamage(3);       // 3 - 8 = -5 → 최소 1
    check("takeDamage(방어력 이하) → 최소 1 데미지 [경계]", w.getHp() == hpBefore - 1);
}

// 5. 치명적 데미지: HP 는 0 밑으로 내려가지 않음
void test_hp_not_negative() {
    Mage m("테스트");
    m.takeDamage(9999);
    check("치명 데미지 후 HP >= 0 [경계]", m.getHp() >= 0);
}

// 6. isAlive: HP > 0 이면 true
void test_is_alive_true() {
    Healer h("테스트");
    check("isAlive() → 살아있을 때 true", h.isAlive() == true);
}

// 7. isAlive: 치명 데미지 후 false
void test_is_alive_false() {
    Healer h("테스트");
    h.takeDamage(9999);
    check("isAlive() → 사망 후 false [실패 케이스]", h.isAlive() == false);
}

// 8. 초기 골드 == 0
void test_initial_gold() {
    Warrior w("테스트");
    check("Warrior 초기 골드 == 0", w.getGold() == 0);
}

// ── main ──────────────────────────────────────────────────
int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif
    std::cout << "===== 단위 테스트 시작 =====\n\n";

    test_initial_hp();
    test_take_damage_normal();
    test_take_damage_zero();
    test_take_damage_min_one();
    test_hp_not_negative();
    test_is_alive_true();
    test_is_alive_false();
    test_initial_gold();
    std::cout << "\n===== " << passed << " PASS / " << failed << " FAIL =====\n";
    std::cout << "\n  아무 키나 누르면 닫힙니다...";
    std::cin.get();
    return failed == 0 ? 0 : 1;
}
