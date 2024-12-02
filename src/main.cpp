#include <cstdlib>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/utils/daemon_run.hpp>
#include <tgbot/tgbot.h>

#include "bot/handlers/command_handlers.hpp"
#include "bot/utils/utils.hpp"

int main(int argc, char* argv[]) {
  auto component_list = userver::components::MinimalServerComponentList()
                  .Append<userver::components::Postgres>("postgres-db-1")
                  .Append<userver::components::HttpClient>();

  std::string token(getenv("TOKEN"));;
  printf("Token: %s\n", token.c_str());

  TgBot::Bot bot(token);

  bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
      NeverForgetBot::Commands::onStartCommand(message, bot);
  });

  bot.getEvents().onCommand("help", [&bot](TgBot::Message::Ptr message) {
      NeverForgetBot::Commands::onHelpCommand(message, bot);
  });

  bot.getEvents().onUnknownCommand([&bot](TgBot::Message::Ptr message) {
      bot.getApi().sendMessage(message->chat->id, "Invalid command");
  });

  bot.getEvents().onNonCommandMessage([&bot](TgBot::Message::Ptr message) {
      bot.getApi().sendMessage(message->chat->id, "You typed: " + message->text);
  });

  std::thread botThread(NeverForgetBot::Utils::startLongPolling, std::ref(bot));

  userver::utils::DaemonMain(argc, argv, component_list);

  botThread.join();

  return 0;
}
