#include "skillparser.h"
#include "skill.h"
#include "animation.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>

SkillParser::SkillParser(QString filepath)
{
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw "[SEVERE] SkillParser: Unable to open Json document";

    QByteArray data = file.readAll();

    // Remove Comments <- u wot m8, fine, I'll play your game this time ~WapaMario63
    while (true)
    {
        int start=data.indexOf("/*");
        if (start == -1)    break;
        int end = data.indexOf("*/");
        if (end < start+2)  break; // We would loop forever otherwise
        if (end != -1)      data.remove(start, end-start+2);
    }

    QJsonParseError jsonError;
    QJsonArray json = QJsonDocument::fromJson(data,&jsonError).array();
    if (jsonError.error != QJsonParseError::NoError)
        throw QString("[SEVERE] SkillParser Error: "+jsonError.errorString()+QString(" at %1").arg(jsonError.offset));

    for (QJsonValue skillValue : json)
    {
        if (!skillValue.isObject()) throw "[SEVERE] SkillParser: Invalid Format (not an object)";

        QJsonObject skillObject = skillValue.toObject();
        Skill skill = parseSkill(skillObject);
        Skill::skills[skill.id] = skill;
    }
}

Skill SkillParser::parseSkill(QJsonObject &skillObject)
{
    Skill skill;
    if (skillObject.contains("ID")) skill.id = skillObject["ID"].toDouble();
    if (skillObject.contains("MaxLevel")) skill.maxLevel = skillObject["MaxLevel"].toDouble();
    if (skillObject.contains("Races")) skill.races = parseRaces(skillObject["Races"].toArray());
    if (skillObject.contains("DamageType")) skill.damageType = parseDamageType(skillObject["DamageType"].toArray());
    if (skillObject.contains("Upgrades")) skill.upgrades = parseSkillUpgrades(skillObject["Upgrades"].toArray());
    return skill;
}

Skill::SkillRace SkillParser::parseRaces(QJsonArray jsonRaces)
{
    Skill::SkillRace races = Skill::None;
    for (QJsonValue raceValue : jsonRaces)
    {
        if (raceValue.toString() == "Earth") races = (Skill::SkillRace)(races|Skill::EarthPony);
        else if (raceValue.toString() == "Unicorn") races = (Skill::SkillRace)(races|Skill::Unicorn);
        else if (raceValue.toString() == "Pegasus") races = (Skill::SkillRace)(races|Skill::Pegasus);
        else throw "[SEVERE] SkillParser::parseRaces: Invalid Race (must be EarthPony, Unicorn or Pegasus";
    }
    return races;
}

Skill::SkillDamageType SkillParser::parseDamageType(QJsonArray jsonDT)
{
    if (jsonDT.size() > 1) throw "[SEVERE] SkillParser::parseDamageType multiple damage types!";
    Skill::SkillDamageType dt = Skill::SkillDamageType::Physical;
    for (QJsonValue dtValue : jsonDT)
    {
        if (dtValue.toString() == "Physical") dt = Skill::SkillDamageType::Physical;
        else if (dtValue.toString() == "Magical") dt = Skill::SkillDamageType::Magical;
        else throw "[SEVERE] SkillParser::parseDamageType: Invalid Damage Type (must be Physical or Magical)";
    }
    return dt;
}

QMap<unsigned, SkillUpgrade> SkillParser::parseSkillUpgrades(QJsonArray jsonUpgrades)
{
    QMap<unsigned,SkillUpgrade> skillUpgrades;
    for (QJsonValue jsonUpgrade : jsonUpgrades)
    {
        QJsonObject upgradeObject = jsonUpgrade.toObject();
        SkillUpgrade upgrade;
        if (upgradeObject.contains("ID")) upgrade.id = upgradeObject["ID"].toDouble();
        if (upgradeObject.contains("Tier")) upgrade.tier = upgradeObject["Tier"].toDouble();
        if (upgradeObject.contains("EnergyCost")) upgrade.energyCost = upgradeObject["EnergyCost"].toDouble();
        if (upgradeObject.contains("TargetDistance")) upgrade.targetDistance = upgradeObject["TargetDistance"].toDouble();
        if (upgradeObject.contains("Cooldown")) upgrade.cooldown = upgradeObject["Cooldown"].toDouble();
        if (upgradeObject.contains("CastTime")) upgrade.castTime = upgradeObject["CastTime"].toDouble();
        if (upgradeObject.contains("GlobalCooldown")) upgrade.globalCooldown = upgradeObject["GlobalCooldown"].toDouble();
        if (upgradeObject.contains("ParentID")) upgrade.parentId = upgradeObject["ParentID"].toDouble();
        if (upgradeObject.contains("Aggression")) upgrade.aggression = upgradeObject["Aggression"].toDouble();
        if (upgradeObject.contains("ConeAngle")) upgrade.coneAngle = upgradeObject["ConeAngle"].toDouble();
        if (upgradeObject.contains("AoERadius")) upgrade.AoERadius = upgradeObject["AoERadius"].toDouble();
        if (upgradeObject.contains("AoEDuration")) upgrade.AoEDuration = upgradeObject["AoEDuration"].toDouble();
        if (upgradeObject.contains("MaxSplashCount")) upgrade.maxSplashCount = upgradeObject["MaxSplashCount"].toDouble();
        if (upgradeObject.contains("TrainingPointCost")) upgrade.trainingPointCost = upgradeObject["TrainingPointCost"].toDouble();
        if (upgradeObject.contains("MinimumLevel")) upgrade.minimunLevel = upgradeObject["MinimumLevel"].toDouble();
        if (upgradeObject.contains("TargetShapes")) upgrade.targetShape = parseTargetShapes(upgradeObject["TargetShapes"].toString());
        if (upgradeObject.contains("TargetEffects")) upgrade.targetEffects = parseTargetEffects(upgradeObject["TargetEffects"].toArray());
        if (upgradeObject.contains("SplashEffects")) upgrade.splashEffects = parseTargetEffects(upgradeObject["SplashEffects"].toArray());
        if (upgradeObject.contains("CasterAnimation")) upgrade.casterAnimation = &Animation::animations[upgradeObject["CasterAnimation"].toString()];
        skillUpgrades[upgrade.id]= upgrade;
    }
    return skillUpgrades;
}

SkillUpgrade::TargetShape SkillParser::parseTargetShapes(QString jsonShapes)
{
    SkillUpgrade::TargetShape shapes = SkillUpgrade::TargetShape::None;
    if (jsonShapes.contains("Sphere")) shapes = (SkillUpgrade::TargetShape)(shapes|SkillUpgrade::Sphere);
    if (jsonShapes.contains("Frustum")) shapes = (SkillUpgrade::TargetShape)(shapes|SkillUpgrade::Frustum);
    if (jsonShapes.contains("Target")) shapes = (SkillUpgrade::TargetShape) (shapes|SkillUpgrade::Target);
    if (jsonShapes.contains("Ray")) shapes = (SkillUpgrade::TargetShape) (shapes|SkillUpgrade::Ray);
    if (shapes == SkillUpgrade::TargetShape::None) throw "[SEVERE] SkillParser::parseTargetShapes: No Target!";

    return shapes;
}

QVector<SkillTargetEffect> SkillParser::parseTargetEffects(QJsonArray jsonEffects)
{
    QVector<SkillTargetEffect> effects;
    for (QJsonValue jsonEffect : jsonEffects)
    {
        QJsonObject effectObject = jsonEffect.toObject();
        SkillTargetEffect effect;
        if (effectObject.contains("Stat")) effect.stat = parseStat(effectObject["Stat"].toArray());
        if (effectObject.contains("Targets")) effect.targets = parseTargets(effectObject["Targets"].toArray());
        if (effectObject.contains("Amount")) effect.amount = effectObject["Amount"].toDouble();
        if (effectObject.contains("isMultiplier")) effect.isMultiplier = effectObject["isMultiplier"].toBool();
        if (effectObject.contains("Chance")) effect.chance = effectObject["Chance"].toDouble();
        if (effectObject.contains("IsDPS")) effect.isDPS = effectObject["isDPS"].toBool();
        if (effectObject.contains("SkillId")) effect.skillId = effectObject["SkillId"].toDouble();
        if (effectObject.contains("Duration")) effect.duration = effectObject["Duration"].toDouble();
        effects.append(effect);
    }
    return effects;
}

SkillTarget SkillParser::parseTargets(QJsonArray jsonTargets)
{
    SkillTarget targets = SkillTarget::None;
    for (QJsonValue targetValue : jsonTargets)
    {
        if (targetValue.toString() == "None") continue;
        else if (targetValue.toString() == "Enemy") targets = (SkillTarget)((int)targets|(int)SkillTarget::Enemy);
        else if (targetValue.toString() == "Friendly") targets = (SkillTarget)((int)targets|(int)SkillTarget::Friendly);
        else if (targetValue.toString() == "Self") targets = (SkillTarget)((int)targets|(int)SkillTarget::Self);
        else throw "[SEVERE] SkillParser::parseRargets: Invalid Target (must be Enemy, Friendly, Self or just None)";
    }
    return targets;
}

SkillTargetStat SkillParser::parseStat(QJsonArray jsonStat)
{
    if (jsonStat.size() > 1) throw "[SEVERE] SkillParser::parseStat: multiple stats!";
    SkillTargetStat stat = SkillTargetStat::Health;
    for (QJsonValue statValue : jsonStat)
    {
        if (statValue.toString() == "Health") stat = SkillTargetStat::Health;
        else if (statValue.toString() == "Tension") stat = SkillTargetStat::Tension;
        else throw "[SEVERE] SkillParser::parseStat: Invalid stat (must be Health or Tension)";
    }
    return stat;
}
