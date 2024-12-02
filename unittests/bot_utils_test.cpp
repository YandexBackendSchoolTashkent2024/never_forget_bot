#include <userver/utest/utest.hpp>
#include "../src/bot/utils/utils.hpp"

namespace {

TEST(BotUtils, BotCommands) {
    EXPECT_EQ(NeverForgetBot::Utils::getBotDescription(),
        "Добро пожаловать в Never Forget Bot!\n\n"
        "Создавайте напоминания и получайте своевременные уведомления без лишней траты времени.\n\n"
        "Начнем! 🎯"
    );
}

} // namespace
