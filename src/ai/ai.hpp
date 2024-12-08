#pragma once

#include <string>
#include <chrono>
#include <userver/components/component_base.hpp>
#include <userver/components/component_list.hpp>
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include "../event-creator/event.hpp"

namespace never_forget_bot {

class AI final : public userver::components::ComponentBase {
 public:
  static constexpr std::string_view kName = "ai-component";

  // Constructor declaration
  AI(const userver::components::ComponentConfig& config,
     const userver::components::ComponentContext& context);

  // Method declaration
  Event GenerateEventFromMessage(const std::string& message);
};

// Function to append AI to the component list
void AppendAI(userver::components::ComponentList& component_list);

}  // namespace never_forget_bot
