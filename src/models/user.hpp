#include <string>
#include <optional>

namespace NeverForgetBot {

using namespace std;

struct User {
    string id;
    long telegram_id;
    optional<string> username;
    string name;
    string created_at;
    string updated_at;
};

}
