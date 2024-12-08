#include "ai.hpp"
#include <userver/logging/log.hpp>

namespace never_forget_bot {

// Constructor definition
AI::AI(const userver::components::ComponentConfig& config,
       const userver::components::ComponentContext& context)
    : userver::components::ComponentBase(config, context) {
  LOG_INFO() << "AI component initialized";
}

// Method definition
Event AI::GenerateEventFromMessage(const std::string& message) {
  LOG_INFO() << "Generating event from message: " << message;

  Event event;
  event.user_id = 1234;  // Stub logic
  event.name = "Sample Event";  // Example logic
  event.time = std::chrono::system_clock::now();
  event.type = 1;  // Example type
  event.status = 0;  // Default status

  return event;
}

// Function to append AI to the component list
void AppendAI(userver::components::ComponentList& component_list) {
  component_list.Append<AI>();
}

}  // namespace never_forget_bot

