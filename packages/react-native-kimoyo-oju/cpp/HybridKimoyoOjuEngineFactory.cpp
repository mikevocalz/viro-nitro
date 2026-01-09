#include "HybridKimoyoOjuEngine.hpp"
#include <NitroModules/HybridObjectRegistry.hpp>

namespace margelo::nitro::kimoyooju {

void registerKimoyoOjuHybridObjects() {
    HybridObjectRegistry::registerHybridObjectConstructor(
        "KimoyoOjuEngine",
        []() -> std::shared_ptr<HybridObject> {
            return std::make_shared<HybridKimoyoOjuEngine>();
        }
    );
}

} // namespace margelo::nitro::kimoyooju
