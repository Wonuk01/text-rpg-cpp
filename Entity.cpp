#include "Entity.h"
#include <iostream>
#include <algorithm>

Entity::Entity(const std::string& name, int hp, int attackPower, int defense)
    : name(name), hp(hp), maxHp(hp), attackPower(attackPower), defense(defense) {}

void Entity::takeDamage(int damage) {
    if (damage <= 0) return;  // 0 데미지(울부짖음 등)는 무효
    int actualDamage = std::max(1, damage - defense);
    hp = std::max(0, hp - actualDamage);
}

bool Entity::isAlive() const {
    return hp > 0;
}

void Entity::heal(int amount) {
    hp = std::min(maxHp, hp + amount);
}

std::string Entity::getName() const { return name; }
int Entity::getHp() const { return hp; }
int Entity::getMaxHp() const { return maxHp; }
int Entity::getAttackPower() const { return attackPower; }
int Entity::getDefense() const { return defense; }
void Entity::setAttackPower(int value) { attackPower = value; }

std::string Entity::makeHpBar(int barLen) const {
    int filled = maxHp > 0 ? (hp * barLen) / maxHp : 0;
    std::string bar = "[";
    for (int i = 0; i < barLen; i++)
        bar += (i < filled) ? "\xE2\x96\x88" : "\xE2\x96\x91"; // █ / ░
    bar += "]";
    return bar;
}

void Entity::setDefense(int value) { defense = value; }

void Entity::setHp(int value) { hp = value; }
void Entity::setMaxHp(int value) { maxHp = value; }
