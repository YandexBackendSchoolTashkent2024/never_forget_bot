#include <string>
#include <optional>

namespace NeverForgetBot {

struct User {
    std::string id;
    long telegram_id;
    std::optional<std::string> username;
    std::optional<std::string> name;
    std::string created_at;
    std::string updated_at;
};

}
