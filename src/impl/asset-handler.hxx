#pragma once

#include <string>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <memory>
#include <unordered_map>
#include "route.hxx"

namespace justresting {
    using namespace std;
    using namespace std::literals;
    namespace fs = std::filesystem;

    class AssetHandler {
        const fs::path      asset;
        const unsigned long size;
        const string        type;
        const string        uri_;

        static unordered_map<string, string> mimeTypes;

    public:
        AssetHandler(fs::path asset, fs::path dir)
                : asset{asset},
                  size{fs::file_size(asset)},
                  type{mimeType(asset)},
                  uri_{"/"s + fs::relative(asset, dir).string()} {}

        AssetHandler(fs::path asset, fs::path dir, string index)
                : asset{asset / index},
                  size{fs::file_size(asset / index)},
                  type{mimeType(asset / index)},
                  uri_{"/"s + fs::relative(asset, dir).string()} {}


        string uri() const { return uri_; }
        void operator ()(Request& req, Response& res);

        static string mimeType(fs::path asset);
        static void addMimeType(string ext, string type);
    };


}

