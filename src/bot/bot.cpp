#include <tgbot/tgbot.h>
#include <userver/components/component_base.hpp>
#include <userver/components/component_list.hpp>
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/logging/log.hpp>
#include "../event-creator/event-creator.hpp"

namespace never_forget_bot {

class TelegramBot final : public userver::components::LoggableComponentBase {
 public:
  static constexpr std::string_view kName = "telegram-bot";

  TelegramBot(const userver::components::ComponentConfig& config,
              const userver::components::ComponentContext& context)
      : userver::components::LoggableComponentBase(config, context) {

    // Initialize Telegram bot
    bot_ = std::make_unique<TgBot::Bot>("7976443896:AAEe_wFlsuGwKaGsW7K9sjRjoXQWggXAQeU");

    // Register commands
    bot_->getEvents().onCommand("start", [this](TgBot::Message::Ptr message) {
      bot_->getApi().sendMessage(message->chat->id, "Welcome to the Never Forget Bot!");
    });

    // bot_->getEvents().onCommand("create_event", [this](TgBot::Message::Ptr message) {
    //   std::string user_id = std::to_string(message->chat->id); // Use chat ID as user ID
    //   std::string message_text = message->text.substr(13); // Assuming command is "/create_event <message>"

    //   if (event_creator_.CreateEvent(user_id, message_text)) {
    //     bot_->getApi().sendMessage(message->chat->id, "Event created successfully!");
    //   } else {
    //     bot_->getApi().sendMessage(message->chat->id, "Failed to create event.");
    //   }
    // });

    // Start polling in a separate thread
    polling_thread_ = std::thread([this] {
      try {
        TgBot::TgLongPoll longPoll(*bot_);
        while (is_running_) {
          longPoll.start();  // This blocks until updates are processed
        }
      } catch (const std::exception& e) {
        LOG_ERROR() << "Polling error: " << e.what();
      }
    });
  }

  ~TelegramBot() {
    // Stop polling gracefully
    is_running_ = false;
    if (polling_thread_.joinable()) {
      polling_thread_.join();
    }
  }

 private:
  std::unique_ptr<TgBot::Bot> bot_;
  // EventCreator& event_creator_;
  std::atomic<bool> is_running_{true};
  std::thread polling_thread_;
};

// Function to append the TelegramBot to the component list
void AppendTelegramBot(userver::components::ComponentList& component_list) {
  component_list.Append<TelegramBot>();
}

}  // namespace never_forget_bot
