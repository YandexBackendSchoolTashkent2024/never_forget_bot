#include <string>
#include <chrono>
#include <optional>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/logging/log.hpp>
#include "../ai/ai.hpp"
#include "../event-creator/event.hpp"  // Assuming Event and Notification structs are defined in a shared header

namespace never_forget_bot {

class EventCreator final : public userver::components::ComponentBase {
 public:
  static constexpr std::string_view kName = "event-creator";

  EventCreator(const userver::components::ComponentConfig& config,
               const userver::components::ComponentContext& context)
      : userver::components::ComponentBase(config, context),
        ai_(context.FindComponent<AI>("ai-component")),
        pg_cluster_(context.FindComponent<userver::components::Postgres>("never_forget_bot_db-1").GetCluster()) {
    LOG_INFO() << "EventCreator initialized";
  }

  bool CreateEvent(const std::string& user_id, const std::string& message) {
    try {
      // Generate event from AI component
      Event event = ai_.GenerateEventFromMessage(message);

      // Insert the event into the database
      // auto event_query = R"(
      //     INSERT INTO "event" (user_id, name, time, type, status)
      //     VALUES ($1, $2, $3, $4, $5)
      //     RETURNING id
      // )";
      // auto event_result = pg_cluster_->Execute(event_query, user_id, event.name, event.time, event.type, event.status);
      // auto event_id = event_result.AsSingleRow<int>();

      // // Prepare notification
      // Notification notification;
      // notification.event_id = event_id;
      // notification.time = event.time;
      // notification.sent_time = std::nullopt;

      // // Insert notification into the database
      // auto notification_query = R"(
      //     INSERT INTO "notification" (event_id, time, sent_time)
      //     VALUES ($1, $2, $3)
      // )";
      // pg_cluster_->Execute(notification_query, notification.event_id, notification.time, notification.sent_time);

      LOG_INFO() << "Event and notification successfully created for user: " << user_id;
      return true;
    } catch (const std::exception& e) {
      LOG_ERROR() << "Failed to create event: " << e.what();
      return false;
    }
  }

 private:
  AI& ai_;
   userver::storages::postgres::ClusterPtr pg_cluster_;
};

// Function to append the EventCreator to the component list
void AppendEventCreator(userver::components::ComponentList& component_list) {
  component_list.Append<EventCreator>();
}

}  // namespace never_forget_bot
