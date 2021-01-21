# JustRESTing - REST Microservices in Modern C++
An ultra-fast light-weight REST-WS server based on features of C++17
intended to quickly and easily implement micro-services.

The usage model of this framework is inspired by frameworks in other
languages, such as [Express.js](https://expressjs.com/)  and 
[SparkJava](https://sparkjava.com/).

## Simple Example
Here is a small *Hello World* example

    #include <iostream>
    #include "just_resting.hxx"
    
    using namespace std;
    using namespace std::literals;
    using namespace just_resting;
    
    int main() {
        auto app  = Application{};
        app.port(4200, true);  //true --> find an available port if the primary is busy
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

Given that the build output is in `../dist` the program can be compiled with 

    c++ --std=c++17 -I../dist/include hello.cxx -o hello ../dist/lib/just_resting.a

Then, launch the program

    > ./hello
    JustRESTing/1.0  [(C) Ribomation AB, 2018.]
    -- Routes --
    GET /
    
    Hello server started. URL = http://localhost:4200

Using a HTTP client such as [HTTPie](https://httpie.io/) it might look like this from the client side

    > http  :4200
    HTTP/1.1 200 OK
    Content-Length: 8
    Content-Type: text/plain
    Date: Thu, 21 Jan 2021 13:07:51 GMT
    Server: JustRESTing/0.1
    
    Hi there

and, like this from the server side (*debug printouts was enabled*)
    
    client connected: IP=127.0.0.1
    >>> REQ payload-size=135
    GET / HTTP/1.1<CR><NL>
    Host: localhost:4200<CR><NL>
    User-Agent: HTTPie/1.0.3<CR><NL>
    Accept-Encoding: gzip, deflate<CR><NL>
    Accept: */*<CR><NL>
    Connection: keep-alive<CR><NL>
    <CR><NL>
    **END**


## Build
The build system is based on [CMake](https://cmake.org/). Create a build directory, generate the
build files and then build the library. During the build, it will also run the unit test suite.

    cd path/to/the/just_resting/project/dir
    mkdir bld && cd bld
    cmake ..
    cmake --build .

The build output that can be used elsewhere can be found in the `../dist` directory.

## Demo
There is a small show-case application inside `./demo/` which can be run with the command

    cmake --build . --target demo

Then open a browser to [`http://localhost:4200/`](http://localhost:4200/)

## Disclaimer
This software is provided *as is*, without any implied warranties etc. 



