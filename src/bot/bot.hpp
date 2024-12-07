#pragma once

#include <string>
#include <string_view>

#include <userver/components/component_list.hpp>

namespace never_forget_bot {
class TelegramBot;
void AppendTelegramBot(userver::components::ComponentList& component_list);

}  