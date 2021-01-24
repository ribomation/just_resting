# JustRESTing - REST Microservices in Modern C++
An ultra-fast light-weight REST-WS server based on features of C++17
intended to quickly and easily implement microservices.

The usage model of this framework is inspired by frameworks in other
languages, such as [Express.js](https://expressjs.com/)  and 
[SparkJava](https://sparkjava.com/).

- [Simple Example (CLI)](#simple-example-command-line)
    - [Hello World App](#hello-world)
    - [Compilation](#compilation)
    - [Launch Server](#launch-server)
    - [Client Output](#client-output)
    - [Server Output](#server-output)
- [Simple Example (CMake)](#simple-example-cmake)
    - [Build File](#build-file)
    - [App File](#app-file)
    - [Build](#build)
    - [Launch](#launch)
- [Build Library](#how-to-build-the-library)
    - [Generate a dist archive](#generate-a-dist-archive)
    - [Unit Tests](#unit-tests)
    - [Demo App](#demo-app)
- [Disclaimer](#disclaimer)


# Simple Example (command-line)

## Hello World
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

## Compilation
Given that the build output is in `../dist` the program can be compiled with 

    c++ --std=c++17 -I../dist/include hello.cxx -o hello ../dist/lib/just_resting.a

## Launch Server
Then, launch the program

    > ./hello
    JustRESTing/1.0  [(C) Ribomation AB, 2018.]
    -- Routes --
    GET /
    
    Hello server started. URL = http://localhost:4200

## Client Output
Using a HTTP client such as [HTTPie](https://httpie.io/) it might look like this from the client side

    > http  :4200
    HTTP/1.1 200 OK
    Content-Length: 8
    Content-Type: text/plain
    Date: Thu, 21 Jan 2021 13:07:51 GMT
    Server: JustRESTing/0.1
    
    Hi there

## Server Output
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

# Simple Example (CMake) 
Although your can download and build the library yourself, and then build your own
app from the `dist/` artifacts as shown above. However, it's much more easier to use 
CMake and let it download and configure the library for you, using
[FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html)

## Build File
First create a `CMakeLists.txt` file:

    cmake_minimum_required(VERSION 3.16)
    project(app LANGUAGES CXX)
    set(CMAKE_CXX_STANDARD          17)
    
    include(FetchContent)
    FetchContent_Declare(just_resting
        GIT_REPOSITORY  https://github.com/ribomation/just_resting.git
        GIT_SHALLOW     true
        GIT_TAG         main
    )
    FetchContent_MakeAvailable(just_resting)
    
    add_executable(app app.cxx)
    target_link_libraries(app PRIVATE ribomation::just_resting)

## App File
Second, create the C++ file `app.cxx` (*simplified version of the hello app above*):

    #include <iostream>
    #include "just_resting.hxx"
    using namespace std;
    using namespace std::literals;
    using namespace just_resting;
    
    int main() {
        Application srv;
        srv.port(3500, true);
        srv.route("GET"s, "/"s, [](Request& req, Response& res) {
            res.body("Hi there, from a justRESTing microservice"s);
        });
        srv.launch([](auto port){
          cout << "server started. http://localhost:" << port << endl;
        });
        return 0;
    }

## Build
Third, build the server app. During the configuration phase of CMake, it will perform 
a shallow (*just the files, no history*) clone of the 
[*justRESTing* GIT repo](https://github.com/ribomation/just_resting).

    mkdir bld && cd bld
    cmake ..
    cmake --build .

## Launch
Finally, launch the server, still within the build directory

    ./app

Here is an example of using HTTPie CLI

    http :3500

Which provides the following output:

    HTTP/1.1 200 OK
    Content-Length: 41
    Content-Type: text/plain
    Date: Sat, 23 Jan 2021 13:51:34 GMT
    Server: JustRESTing/0.1
    
    Hi there, from a justRESTing microservice



# How to Build the Library
The build system is based on [CMake](https://cmake.org/). Create a build directory, generate the
build files and then build the library. During the build, it will also run the unit test suite.

    cd path/to/the/just_resting/project/dir
    mkdir bld && cd bld
    cmake ..
    cmake --build .

## Generate a *dist* archive
You can generate a tar file which can be used elsewhere. Run the following command

    cmake --build . --target dist

It will generate GZIP:ed TAR file in the build directory, containing the library archive
(`lib/just_resting.a`) file and the public include files (`include/**.hxx`).

## Unit Tests
You can run the unit tests (based on Catch2) with the following command

    cmake --build . --target test

## Demo App
There is a small show-case application inside `./demo/` which can be run with the command

    cmake --build . --target demo

Then open a browser to [`http://localhost:4200/`](http://localhost:4200/)

## Disclaimer
This software is provided *as is*, without any implied warranties etc. 



