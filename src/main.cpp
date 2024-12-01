#include <userver/clients/http/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>
#include <tgbot/tgbot.h>

#include "hello.hpp"

using namespace std;
using namespace TgBot;

void startLongPolling(TgBot::Bot& bot) {
    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        bot.getApi().deleteWebhook();
        bot.getApi().setMyShortDescription("An awesome bot to make you forget about forgetting");

        TgLongPoll longPoll(bot);
        while (true) {
            printf("Long poll started\n");
            longPoll.start();
        }
    } catch (std::exception& e) {
        printf("error: %s\n", e.what());
    }
}

int main(int argc, char* argv[]) {
  auto component_list = userver::components::MinimalServerComponentList()
                            .Append<userver::server::handlers::Ping>()
                            .Append<userver::components::TestsuiteSupport>()
                            .Append<userver::components::HttpClient>()
                            .Append<userver::server::handlers::TestsControl>();

  never_forget_bot::AppendHello(component_list);


  string token = "5899644754:AAGDugrRiYvH4RvWsJkimfC0ut0mq0HuviY";
  printf("Token: %s\n", token.c_str());

  Bot bot(token);

  bot.getEvents().onCommand("start", [&bot](Message::Ptr message) {
      bot.getApi().sendMessage(message->chat->id, "welcome");
  });

  bot.getEvents().onCommand("help", [&bot](Message::Ptr message) {
      bot.getApi().sendMessage(message->chat->id, "help");
  });

  std::thread botThread(startLongPolling, std::ref(bot));

  userver::utils::DaemonMain(argc, argv, component_list);

  botThread.join();
}
