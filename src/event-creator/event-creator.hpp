#pragma once

#include <string>
#include <string_view>

#include <userver/components/component_list.hpp>

namespace never_forget_bot {
class EventCreator{
  public:
    EventCreator(const userver::components::ComponentConfig& config,
               const userver::components::ComponentContext& context);
               
    bool CreateEvent(const std::string& user_id, const std::string& message) {
  }

};
void AppendEventCreator(userver::components::ComponentList& component_list);

}  