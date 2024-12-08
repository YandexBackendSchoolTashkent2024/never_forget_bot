#include <cstdlib>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/utils/daemon_run.hpp>
#include <tgbot/tgbot.h>
#include "ai/ai.hpp"
#include "bot/bot.hpp"
#include "bot/handlers/command_handlers.hpp"
#include "bot/utils/utils.hpp"
#include "event-creator/event-creator.hpp"
#include <userver/testsuite/testsuite_support.hpp>

int main(int argc, char* argv[]) {
  std::string db_name(getenv("POSTGRES_DB"));
//   std::string token(getenv("TOKEN"));
//   printf("Token: %s\n", token.c_str());
  printf("Postgres DB: %s\n", db_name.c_str());

  auto component_list = userver::components::MinimalServerComponentList()
  .Append<userver::components::TestsuiteSupport>()
                  .Append<userver::components::Postgres>(db_name)
                  .Append<userver::components::HttpClient>();

  never_forget_bot::AppendAI(component_list);
  never_forget_bot::AppendEventCreator(component_list);
  // never_forget_bot::AppendTelegramBot(component_list);
  
//   TgBot::Bot bot(token);

//   bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
//       NeverForgetBot::Commands::onStartCommand(message, bot);
//   });

//   bot.getEvents().onCommand("help", [&bot](TgBot::Message::Ptr message) {
//       NeverForgetBot::Commands::onHelpCommand(message, bot);
//   });

//   bot.getEvents().onUnknownCommand([&bot](TgBot::Message::Ptr message) {
//       bot.getApi().sendMessage(message->chat->id, "Invalid command");
//   });

//   bot.getEvents().onNonCommandMessage([&bot](TgBot::Message::Ptr message) {
//       bot.getApi().sendMessage(message->chat->id, "You typed: " + message->text);
//   });

//   std::thread botThread(NeverForgetBot::Utils::startLongPolling, std::ref(bot));

  userver::utils::DaemonMain(argc, argv, component_list);

//   botThread.join();

  return 0;
}
