#pragma once

#include <string>
#include <string_view>

#include <userver/components/component_list.hpp>

namespace never_forget_bot {

enum class UserType { kFirstTime, kKnown };
std::string SayHelloTo(std::string_view name, UserType type);

void AppendHello(userver::components::ComponentList& component_list);

}  // namespace never_forget_bot
