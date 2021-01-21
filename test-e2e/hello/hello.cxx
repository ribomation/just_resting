#include <iostream>
#include "just_resting.hxx"

using namespace std;
using namespace std::literals;
using namespace just_resting;

int main() {
    auto app  = Application{};
    app.port(4200, true);
    app.debug(true);

    app.route("GET"s, "/"s, [&](Request& req, Response& res) {
        res.body("Hi there"s);
    });

    cout << app << endl;
    app.launch([](auto port) {
        cout << "Hello server started. URL = http://localhost:" << port << endl;
    });

    return 0;
}
