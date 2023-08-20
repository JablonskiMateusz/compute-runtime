/*
 * Copyright (C) 2021-2023 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "shared/source/built_ins/built_ins.h"
#include "shared/source/command_container/implicit_scaling.h"
#include "shared/source/compiler_interface/default_cache_config.h"
#include "shared/source/debug_settings/debug_settings_manager.h"
#include "shared/source/execution_environment/root_device_environment.h"
#include "shared/source/helpers/api_specific_config.h"
#include "shared/source/memory_manager/compression_selector.h"
#include "shared/source/page_fault_manager/cpu_page_fault_manager.h"
#include "shared/test/common/base_ult_config_listener.h"
#include "shared/test/common/helpers/test_files.h"
#include "shared/test/common/tests_configuration.h"

#include "test_files_setup.h"

namespace NEO {
namespace ImplicitScaling {
bool apiSupport = false;
} // namespace ImplicitScaling

const char *neoMockSettingsFileName = "neo_mock.config";
std::vector<const char *> validUltL0Prefixes = {"NEO_L0_", "NEO_", ""};
std::vector<NEO::DebugVarPrefix> validUltL0PrefixTypes = {DebugVarPrefix::Neo_L0, DebugVarPrefix::Neo, DebugVarPrefix::None};
std::vector<const char *> validUltOclPrefixes = {"NEO_OCL_", "NEO_", ""};
std::vector<NEO::DebugVarPrefix> validUltOclPrefixTypes = {DebugVarPrefix::Neo_Ocl, DebugVarPrefix::Neo, DebugVarPrefix::None};

bool CompressionSelector::preferCompressedAllocation(const AllocationProperties &properties) {
    return false;
}
void PageFaultManager::transferToCpu(void *ptr, size_t size, void *cmdQ) {
}
void PageFaultManager::transferToGpu(void *ptr, void *cmdQ) {
}
void PageFaultManager::allowCPUMemoryEviction(void *ptr, PageFaultData &pageFaultData) {
}
CompilerCacheConfig getDefaultCompilerCacheConfig() { return {}; }
const char *getAdditionalBuiltinAsString(EBuiltInOps::Type builtin) { return nullptr; }

void RootDeviceEnvironment::initApiGfxCoreHelper() {
}

ApiSpecificConfig::ApiType apiTypeForUlts = ApiSpecificConfig::OCL;
bool isStatelessCompressionSupportedForUlts = true;

bool ApiSpecificConfig::isStatelessCompressionSupported() {
    return isStatelessCompressionSupportedForUlts;
}
bool ApiSpecificConfig::getGlobalBindlessHeapConfiguration() {
    return DebugManager.flags.UseExternalAllocatorForSshAndDsh.get();
}
bool ApiSpecificConfig::getBindlessMode() {
    if (DebugManager.flags.UseBindlessMode.get() != -1) {
        return DebugManager.flags.UseBindlessMode.get();
    } else {
        return false;
    }
}

bool ApiSpecificConfig::isDeviceAllocationCacheEnabled() {
    return false;
}

bool ApiSpecificConfig::isDynamicPostSyncAllocLayoutEnabled() {
    return (NEO::DebugManager.flags.EnableDynamicPostSyncAllocLayout.get() == 1);
}

ApiSpecificConfig::ApiType ApiSpecificConfig::getApiType() {
    return apiTypeForUlts;
}

uint64_t ApiSpecificConfig::getReducedMaxAllocSize(uint64_t maxAllocSize) {
    return maxAllocSize / 2;
}

std::string ApiSpecificConfig::getName() {
    return "shared";
}
const char *ApiSpecificConfig::getRegistryPath() {
    return "";
}

void ApiSpecificConfig::initPrefixes() {
}

const std::vector<const char *> &ApiSpecificConfig::getPrefixStrings() {
    if (apiTypeForUlts == ApiSpecificConfig::L0) {
        return validUltL0Prefixes;
    } else {
        return validUltOclPrefixes;
    }
}

const std::vector<DebugVarPrefix> &ApiSpecificConfig::getPrefixTypes() {
    if (apiTypeForUlts == ApiSpecificConfig::L0) {
        return validUltL0PrefixTypes;
    } else {
        return validUltOclPrefixTypes;
    }
}
} // namespace NEO

using namespace NEO;
void cleanTestHelpers() {}

void applyWorkarounds() {
    const std::array<ConstStringRef, 11> builtinIntermediateNames{"copy_buffer_to_buffer.builtin_kernel.bc",
                                                                  "copy_buffer_rect.builtin_kernel.bc",
                                                                  "fill_buffer.builtin_kernel.bc",
                                                                  "copy_buffer_to_image3d.builtin_kernel.bc",
                                                                  "copy_image3d_to_buffer.builtin_kernel.bc",
                                                                  "copy_image_to_image1d.builtin_kernel.bc",
                                                                  "copy_image_to_image2d.builtin_kernel.bc",
                                                                  "copy_image_to_image3d.builtin_kernel.bc",
                                                                  "fill_image1d.builtin_kernel.bc",
                                                                  "fill_image2d.builtin_kernel.bc",
                                                                  "fill_image3d.builtin_kernel.bc"};
    auto &storageRegistry = EmbeddedStorageRegistry::getInstance();
    for (auto builtinIntermediateName : builtinIntermediateNames) {
        std::string resource = "__mock_spirv_resource";
        storageRegistry.store(builtinIntermediateName.str(), createBuiltinResource(resource.data(), resource.size() + 1));
    }
}

void initGTest(int &argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
}

bool isPlatformSupported(const HardwareInfo &hwInfoForTests) {
    return true;
}

void setupTestFiles(std::string testBinaryFiles, int32_t revId) {
    testBinaryFiles.append("/");
    testBinaryFiles.append(binaryNameSuffix);
    testBinaryFiles.append("/");
    testBinaryFiles.append(std::to_string(revId));
    testBinaryFiles.append("/");
    testBinaryFiles.append(testFiles);
    testFiles = testBinaryFiles;

    std::string nClFiles = NEO_SHARED_TEST_FILES_DIR;
    nClFiles.append("/");
    clFiles = nClFiles;
}

std::string getBaseExecutionDir() {
    if (testMode != TestMode::AubTests) {
        return "shared/";
    }
    return "";
}

void addUltListener(::testing::TestEventListeners &listeners) {
    listeners.Append(new BaseUltConfigListener);
}
