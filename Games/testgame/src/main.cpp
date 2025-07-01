#include <Sparkle.h>

#include "core/assert.h"
// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
int main() {
    SPA_LOG_TRACE("hello world");
    SPA_LOG_DEBUG("hello ");
    const char* msg = "world";
    SPA_LOG_INFO("hello {}", msg);
    SPA_LOG_WARN("hello {}", msg);
    SPA_LOG_ERROR("hello {}", msg);
    SPA_LOG_FATAL("hello {}", msg);
    SPA_ASSERT(1 ==0);





    return 0;
}