/*
 * Copyright (C) 2020-2023 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "shared/source/os_interface/debug_env_reader.h"

#include "shared/source/helpers/api_specific_config.h"
#include "shared/source/utilities/io_functions.h"

#include <vector>

namespace NEO {

const char *EnvironmentVariableReader::appSpecificLocation(const std::string &name) {
    return name.c_str();
}

bool EnvironmentVariableReader::getSetting(const char *settingName, bool defaultValue, DebugVarPrefix &type) {
    return getSetting(settingName, static_cast<int64_t>(defaultValue), type) ? true : false;
}

bool EnvironmentVariableReader::getSetting(const char *settingName, bool defaultValue) {
    return getSetting(settingName, static_cast<int64_t>(defaultValue)) ? true : false;
}

int32_t EnvironmentVariableReader::getSetting(const char *settingName, int32_t defaultValue, DebugVarPrefix &type) {
    return static_cast<int32_t>(getSetting(settingName, static_cast<int64_t>(defaultValue), type));
}

int32_t EnvironmentVariableReader::getSetting(const char *settingName, int32_t defaultValue) {
    return static_cast<int32_t>(getSetting(settingName, static_cast<int64_t>(defaultValue)));
}

int64_t EnvironmentVariableReader::getSetting(const char *settingName, int64_t defaultValue, DebugVarPrefix &type) {
    int64_t value = defaultValue;
    char *envValue;

    const std::vector<const char *> prefixString = ApiSpecificConfig::getPrefixStrings();
    const std::vector<NEO::DebugVarPrefix> prefixType = ApiSpecificConfig::getPrefixTypes();
    char neoFinal[MAX_NEO_KEY_LENGTH];
    uint32_t i = 0;

    for (const auto &prefix : prefixString) {
        strcpy_s(neoFinal, strlen(prefix) + 1, prefix);
        strcpy_s(neoFinal + strlen(prefix), strlen(settingName) + 1, settingName);
        envValue = IoFunctions::getenvPtr(neoFinal);
        if (envValue) {
            value = atoll(envValue);
            type = prefixType[i];
            return value;
        }
        i++;
    }
    type = DebugVarPrefix::None;
    return value;
}

int64_t EnvironmentVariableReader::getSetting(const char *settingName, int64_t defaultValue) {
    int64_t value = defaultValue;
    char *envValue;

    envValue = IoFunctions::getenvPtr(settingName);
    if (envValue) {
        value = atoll(envValue);
    }
    return value;
}

std::string EnvironmentVariableReader::getSetting(const char *settingName, const std::string &value, DebugVarPrefix &type) {
    char *envValue;
    std::string keyValue;
    keyValue.assign(value);

    const std::vector<const char *> prefixString = ApiSpecificConfig::getPrefixStrings();
    const std::vector<DebugVarPrefix> prefixType = ApiSpecificConfig::getPrefixTypes();

    char neoFinal[MAX_NEO_KEY_LENGTH];
    uint32_t i = 0;
    for (const auto &prefix : prefixString) {
        strcpy_s(neoFinal, strlen(prefix) + 1, prefix);
        strcpy_s(neoFinal + strlen(prefix), strlen(settingName) + 1, settingName);
        envValue = IoFunctions::getenvPtr(neoFinal);
        if (envValue) {
            keyValue.assign(envValue);
            type = prefixType[i];
            return keyValue;
        }
        i++;
    }
    type = DebugVarPrefix::None;
    return keyValue;
}

std::string EnvironmentVariableReader::getSetting(const char *settingName, const std::string &value) {
    char *envValue;
    std::string keyValue;
    keyValue.assign(value);

    envValue = IoFunctions::getenvPtr(settingName);
    if (envValue) {
        keyValue.assign(envValue);
    }
    return keyValue;
}
} // namespace NEO
