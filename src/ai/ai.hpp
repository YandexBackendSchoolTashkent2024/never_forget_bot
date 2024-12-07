#pragma once
#include <userver/components/component_base.hpp>
#include <userver/components/component_list.hpp>
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include "../event-creator/event.hpp" 

namespace never_forget_bot {
class AI {
    public:
    static constexpr std::string_view kName = "ai-component";
    AI(const userver::components::ComponentConfig& config,
     const userver::components::ComponentContext& context);
     
     Event GenerateEventFromMessage(const std::string& message) {
  };
};

void AppendAI(userver::components::ComponentList& component_list);

}  // namespace never_forget_bot
