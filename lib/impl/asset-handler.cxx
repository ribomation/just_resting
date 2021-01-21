
#include "asset-handler.hxx"

namespace just_resting {
    using namespace std;
    using namespace std::literals;
    namespace fs = std::filesystem;

    unordered_map<string, string> AssetHandler::mimeTypes{
            {".html"s, "text/html"s},
            {".png"s,  "image/png"s},
            {".jpg"s,  "image/jpg"s},
            {".gif"s,  "image/gif"s},
            {".gif"s,  "image/gif"s},
            {".txt"s,  "text/plain"s},
            {".css"s,  "text/css"s},
            {".js"s,   "application/javascript"s},
            {".json"s, "application/json"s},
            {".xml"s,  "application/xml"s},
            {".zip"s,  "application/zip"s},
            {".csv"s,  "text/csv"s},
    };

    string AssetHandler::mimeType(fs::path asset)  {
        string ext = asset.extension();
        auto   it  = mimeTypes.find(ext);
        if (it != mimeTypes.end()) return it->second;
        return "application/octet-stream"s;
    }

    void AssetHandler::addMimeType(string ext, string type) {
        mimeTypes.insert({ext, type});
    }

    void AssetHandler::operator()(Request& req, Response& res) {
        ifstream file{asset, ios::binary};
        if (!file) throw runtime_error{"cannot open asset file: "s + asset.string()};

        auto buffer = new char[size];
        file.read(buffer, size);
        unique_ptr<char[]> body{buffer};
        res.body(move(body), size);
        res.type(type);
    }

}
