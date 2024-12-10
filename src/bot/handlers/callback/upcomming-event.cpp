#include "callback.hpp"

namespace NeverForgetBot::CallbackHandlers {

    void onUpcommingEventSelection(const std::string& queryData, TgBot::CallbackQuery::Ptr query, TgBot::Bot& bot, Database& db) {
         std::string futureStatus;
         std::string eventId;
         if (queryData.find("done-") == 0 ) {
            futureStatus = "COMPLETED";
            eventId = queryData.substr(5);
         }else if (queryData.find("delete-")==0){
            futureStatus = "DELETED";
            eventId = queryData.substr(7);
         }

        db.changeEventStatus(eventId, futureStatus);

        bot.getApi().answerCallbackQuery(query->id, "Операция выполнена", false);
    }

} 

