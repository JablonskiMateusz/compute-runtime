/*
 * Copyright (C) 2023-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "shared/source/os_interface/linux/drm_wrappers.h"
#include "shared/source/os_interface/linux/sys_calls.h"

#include "level_zero/core/source/device/device.h"
#include "level_zero/core/source/device/device_imp.h"
#include "level_zero/tools/source/debug/debug_session.h"
#include "level_zero/tools/source/debug/debug_session_imp.h"

namespace L0 {
struct DebugSessionLinux : DebugSessionImp {

    DebugSessionLinux(const zet_debug_config_t &config, Device *device, int fd) : DebugSessionImp(config, device), fd(fd){};
    static ze_result_t translateDebuggerOpenErrno(int error);
    bool closeFd();
    void closeAsyncThread();

    int fd = 0;
    std::atomic<bool> internalThreadHasStarted{false};
    static void *readInternalEventsThreadFunction(void *arg);

    MOCKABLE_VIRTUAL void startInternalEventsThread() {
        internalEventThread.thread = NEO::Thread::create(readInternalEventsThreadFunction, reinterpret_cast<void *>(this));
    }
    void closeInternalEventsThread() {
        internalEventThread.close();
    }

    virtual void readInternalEventsAsync() = 0;
    MOCKABLE_VIRTUAL std::unique_ptr<uint64_t[]> getInternalEvent();
    MOCKABLE_VIRTUAL float getThreadStartLimitTime() {
        return 0.5;
    }
    virtual int openVmFd(uint64_t vmHandle, bool readOnly) = 0;
    virtual int flushVmCache(int vmfd) { return 0; };
    ze_result_t readGpuMemory(uint64_t memoryHandle, char *output, size_t size, uint64_t gpuVa) override;
    ze_result_t writeGpuMemory(uint64_t memoryHandle, const char *input, size_t size, uint64_t gpuVa) override;

    ThreadHelper internalEventThread;
    std::mutex internalEventThreadMutex;
    std::condition_variable internalEventCondition;
    std::queue<std::unique_ptr<uint64_t[]>> internalEventQueue;
    constexpr static uint64_t invalidClientHandle = std::numeric_limits<uint64_t>::max();
    constexpr static uint64_t invalidHandle = std::numeric_limits<uint64_t>::max();
    uint64_t clientHandle = invalidClientHandle;
    uint64_t clientHandleClosed = invalidClientHandle;
    static constexpr size_t maxEventSize = 4096;

    struct IoctlHandler {
        virtual ~IoctlHandler() = default;
        virtual int ioctl(int fd, unsigned long request, void *arg) {
            return 0;
        };
        MOCKABLE_VIRTUAL int fsync(int fd) {
            return NEO::SysCalls::fsync(fd);
        }
        MOCKABLE_VIRTUAL int poll(pollfd *pollFd, unsigned long int numberOfFds, int timeout) {
            return NEO::SysCalls::poll(pollFd, numberOfFds, timeout);
        }

        MOCKABLE_VIRTUAL int64_t pread(int fd, void *buf, size_t count, off_t offset) {
            return NEO::SysCalls::pread(fd, buf, count, offset);
        }

        MOCKABLE_VIRTUAL int64_t pwrite(int fd, const void *buf, size_t count, off_t offset) {
            return NEO::SysCalls::pwrite(fd, buf, count, offset);
        }

        MOCKABLE_VIRTUAL void *mmap(void *addr, size_t size, int prot, int flags, int fd, off_t off) {
            return NEO::SysCalls::mmap(addr, size, prot, flags, fd, off);
        }

        MOCKABLE_VIRTUAL int munmap(void *addr, size_t size) {
            return NEO::SysCalls::munmap(addr, size);
        }
    };

    ze_result_t readMemory(ze_device_thread_t thread, const zet_debug_memory_space_desc_t *desc, size_t size, void *buffer) override;
    ze_result_t writeMemory(ze_device_thread_t thread, const zet_debug_memory_space_desc_t *desc, size_t size, const void *buffer) override;
    struct BindInfo {
        uint64_t gpuVa = 0;
        uint64_t size = 0;
    };

    struct EventToAck {
        EventToAck(uint64_t seqno, uint32_t type) : seqno(seqno), type(type){};
        uint64_t seqno;
        uint32_t type;
    };
    struct IsaAllocation {
        BindInfo bindInfo;
        uint64_t elfUuidHandle;
        uint64_t vmHandle;
        bool tileInstanced = false;
        bool perKernelModule = true;
        NEO::DeviceBitfield deviceBitfield;

        uint64_t moduleBegin;
        uint64_t moduleEnd;

        std::unordered_set<uint64_t> cookies;
        int vmBindCounter = 0;
        bool moduleLoadEventAck = false;
        std::vector<EventToAck> ackEvents;
    };

    struct ClientConnection {
        virtual ~ClientConnection() = default;
        virtual size_t getElfSize(uint64_t elfHandle) = 0;
        virtual char *getElfData(uint64_t elfHandle) = 0;

        std::unordered_set<uint64_t> vmIds;

        std::unordered_map<uint64_t, BindInfo> vmToModuleDebugAreaBindInfo;
        std::unordered_map<uint64_t, BindInfo> vmToContextStateSaveAreaBindInfo;
        std::unordered_map<uint64_t, BindInfo> vmToStateBaseAreaBindInfo;
        std::unordered_map<uint64_t, uint32_t> vmToTile;

        std::unordered_map<uint64_t, uint64_t> elfMap;
        std::unordered_map<uint64_t, std::unique_ptr<IsaAllocation>> isaMap[NEO::EngineLimits::maxHandleCount];

        uint64_t moduleDebugAreaGpuVa = 0;
        uint64_t contextStateSaveAreaGpuVa = 0;
        uint64_t stateBaseAreaGpuVa = 0;

        size_t contextStateSaveAreaSize = 0;
    };

  protected:
    virtual std::shared_ptr<ClientConnection> getClientConnection(uint64_t clientHandle) = 0;

    enum class ThreadControlCmd {
        interrupt,
        resume,
        stopped,
        interruptAll
    };

    struct AttentionEventFields {
        uint64_t clientHandle;
        uint64_t contextHandle;
        uint64_t lrcHandle;
        uint32_t bitmaskSize;
        uint8_t *bitmask;
    };

    void updateStoppedThreadsAndCheckTriggerEvents(AttentionEventFields &attention, uint32_t tileIndex);
    virtual uint64_t getVmHandleFromClientAndlrcHandle(uint64_t clientHandle, uint64_t lrcHandle) = 0;
    virtual std::unique_lock<std::mutex> getThreadStateMutexForTileSession(uint32_t tileIndex) = 0;
    virtual void checkTriggerEventsForAttentionForTileSession(uint32_t tileIndex) = 0;
    virtual void addThreadToNewlyStoppedFromRaisedAttentionForTileSession(EuThread::ThreadId threadId,
                                                                          uint64_t memoryHandle,
                                                                          const void *stateSaveArea,
                                                                          uint32_t tileIndex) = 0;

    virtual int euControlIoctl(ThreadControlCmd threadCmd,
                               const NEO::EngineClassInstance *classInstance,
                               std::unique_ptr<uint8_t[]> &bitmask,
                               size_t bitmaskSize, uint64_t &seqnoOut, uint64_t &bitmaskSizeOut) = 0;
    MOCKABLE_VIRTUAL int threadControl(const std::vector<EuThread::ThreadId> &threads, uint32_t tile, ThreadControlCmd threadCmd, std::unique_ptr<uint8_t[]> &bitmask, size_t &bitmaskSize);
    void checkStoppedThreadsAndGenerateEvents(const std::vector<EuThread::ThreadId> &threads, uint64_t memoryHandle, uint32_t deviceIndex) override;
    MOCKABLE_VIRTUAL bool checkForceExceptionBit(uint64_t memoryHandle, EuThread::ThreadId threadId, uint32_t *cr0, const SIP::regset_desc *regDesc);
    ze_result_t resumeImp(const std::vector<EuThread::ThreadId> &threads, uint32_t deviceIndex) override;
    ze_result_t interruptImp(uint32_t deviceIndex) override;

    ze_result_t getElfOffset(const zet_debug_memory_space_desc_t *desc, size_t size, const char *&elfData, uint64_t &offset);
    ze_result_t readElfSpace(const zet_debug_memory_space_desc_t *desc, size_t size, void *buffer,
                             const char *&elfData, const uint64_t offset);
    virtual bool tryReadElf(const zet_debug_memory_space_desc_t *desc, size_t size, void *buffer, ze_result_t &status);

    bool tryWriteIsa(NEO::DeviceBitfield deviceBitfield, const zet_debug_memory_space_desc_t *desc, size_t size, const void *buffer, ze_result_t &status);
    bool tryReadIsa(NEO::DeviceBitfield deviceBitfield, const zet_debug_memory_space_desc_t *desc, size_t size, void *buffer, ze_result_t &status);
    ze_result_t accessDefaultMemForThreadAll(const zet_debug_memory_space_desc_t *desc, size_t size, void *buffer, bool write);
    ze_result_t readDefaultMemory(ze_device_thread_t thread, const zet_debug_memory_space_desc_t *desc,
                                  size_t size, void *buffer);
    ze_result_t writeDefaultMemory(ze_device_thread_t thread, const zet_debug_memory_space_desc_t *desc,
                                   size_t size, const void *buffer);
    virtual bool tryAccessIsa(NEO::DeviceBitfield deviceBitfield, const zet_debug_memory_space_desc_t *desc, size_t size, void *buffer, bool write, ze_result_t &status);
    ze_result_t getISAVMHandle(uint32_t deviceIndex, const zet_debug_memory_space_desc_t *desc, size_t size, uint64_t &vmHandle);
    bool getIsaInfoForAllInstances(NEO::DeviceBitfield deviceBitfield, const zet_debug_memory_space_desc_t *desc, size_t size, uint64_t vmHandles[], ze_result_t &status);

    virtual std::vector<uint64_t> getAllMemoryHandles();

    std::unique_ptr<IoctlHandler> ioctlHandler;
    uint64_t euControlInterruptSeqno[NEO::EngineLimits::maxHandleCount];
};
} // namespace L0