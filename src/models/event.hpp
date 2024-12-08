#include <string>

namespace NeverForgetBot {

using namespace std;

enum class EventType {
    WHILE_NOT_DONE,
    ONE_TIME
};

enum class EventStatus {
    PENDING,
    COMPLETED,
    NOT_COMPLETED
};

struct Event {
    string id;
    string user_id;
    string name;
    string time;
    EventType type;
    EventStatus status;
    string created_at;
    string updated_at;
};

}
