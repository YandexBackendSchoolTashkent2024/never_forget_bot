#include <string>
#include <chrono>
// Forward declarations of AI, EventRepository, and NotificationRepository classes
class AI;
class EventRepository;
class NotificationRepository;
struct Event {
    
};
namespace never_forget_bot {
class EventCreator {
public:
    EventCreator(AI& ai, EventRepository& event_repository, NotificationRepository& notification_repository)
        : ai_(ai),
          event_repository_(event_repository),
          notification_repository_(notification_repository) {}

    bool CreateEvent(const std::string& user_id, const std::string& message) {
        Event event = ai_.GenerateEventFromMessage(message);
        if (!event.IsValid()) {
            LOG_ERROR() << "Failed to generate event from message: " << message;
            return false;
        }

        // Step 2: Insert the event into the database
        bool event_created = event_repository_.CreateEvent(user_id, event);
        if (!event_created) {
            LOG_ERROR() << "Failed to create event in database for user: " << user_id;
            return false;
        }

        // Step 3: Create notification for the event
        Notification notification = CreateNotificationForEvent(event);
        bool notification_created = notification_repository_.CreateNotification(notification);
        if (!notification_created) {
            LOG_ERROR() << "Failed to create notification for event: " << event.id;
            return false;
        }

        LOG_INFO() << "Event and notification successfully created for user: " << user_id;
        return true;
    }

private:
    Notification CreateNotificationForEvent(const Event& event) {
        // Create a notification based on the event time and details
        Notification notification;
        notification.event_id = event.id;
        notification.time = event.time;  // For example, set it to the event's scheduled time
        return notification;
    }

    AI& ai_;
    EventRepository& event_repository_;
    NotificationRepository& notification_repository_;
};

void AppendEventCreator(){}
};
