#include <string>
#include <chrono>
#include <userver/components/component_base.hpp>
#include <userver/components/component_list.hpp>
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/logging/log.hpp>
#include "../event-creator/event.hpp" 

namespace never_forget_bot {

class AI final : public userver::components::LoggableComponentBase {
 public:
  static constexpr std::string_view kName = "ai-component";

  AI(const userver::components::ComponentConfig& config,
     const userver::components::ComponentContext& context)
      : userver::components::LoggableComponentBase(config, context) {
    LOG_INFO() << "AI component initialized";
  }

  Event GenerateEventFromMessage(const std::string& message) {
    LOG_INFO() << "Generating event from message: " << message;

    // Example stub logic: Generate an Event object based on the message
    Event event;
    event.user_id = 1234;  // Replace with real logic for extracting user_id
    event.name = "Sample Event";  // Derive name from the message
    event.time = std::chrono::system_clock::now();
    event.type = 1;  // Replace with real logic for type
    event.status = 0;  // Default status

    return event;
  }
};

void AppendAI(userver::components::ComponentList& component_list) {
  component_list.Append<AI>();
}

}  // namespace never_forget_bot
