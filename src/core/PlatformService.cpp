#include "PlatformService.h"
#include "MacPlatformService.h"
#include "WindowsPlatformService.h"

std::shared_ptr<IPlatformService> IPlatformService::create() {
#ifdef Q_OS_MACOS
    return std::make_shared<MacPlatformService>();
#elif defined(Q_OS_WIN)
    return std::make_shared<WindowsPlatformService>();
#else
    // Fallback to Mac-like or generic paths for other Unix
    return std::make_shared<MacPlatformService>();
#endif
}
