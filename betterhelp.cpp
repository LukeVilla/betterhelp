#include <cstdio>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/json.hpp>
#include <boost/json/src.hpp>
#include "includes/argengine.hpp"
#include "includes/argengine.cpp"
using namespace std;
namespace fs = std::filesystem;
namespace algo = boost::algorithm;
namespace json = boost::json;

#define NOCACHE false
#define PRINT(x) cout << x << endl;

vector<string> readnsv(string filepath) {
    vector<string> vals;
    string temp;
    ifstream file(filepath);
    while (getline(file, temp)) {
        vals.emplace_back(temp);
    };
    return vals;
}

void createfile(string filepath) {
    ofstream file;
    file.open(filepath);
    if (!file.is_open()) {
        cerr << "Error creating file!" << endl;
        throw EBADF;
    }
    cout << "File created successfully." << endl;
    file.close();
}

bool exists(string filename) {
    fs::path filepath = filename;
    return fs::exists(filepath);
}

string runcmd(string command) {
    string output;
    string cmd = "man " + command;
    FILE *pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        cerr << "Error opening pipe" << endl;
        throw "Error opening pipe";
    }
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        output += buffer;
    }
    pclose(pipe);
    return output;
}

string getdesc(string command) {
    string temp;
    vector<string> lines;
    string desc;
    string manpage = runcmd(command);
    stringstream manstream(manpage);
    while (getline(manstream, temp, '\n')) {
        lines.emplace_back(temp);
    }
    if (lines.size() >= 3) {
        desc = lines[3];
        algo::trim(desc);
        return desc;
    }
    return "";
}

int main(int argc, char **argv) {
    vector<string> programs;
    vector<string> commands;
    string temp;
    bool nocache = false;
    bool rebuildcache = false;
    bool verbose = false;
    vector<string> filter;

    juzzlin::Argengine parser(argc, argv);
    parser.addOption({"-n", "--no-cache", "--nocache"}, [&nocache] {
        nocache = true;
    });
    parser.addOption({"-r", "--rebuild-cache", "--rebuildcache"}, [&rebuildcache] {
        rebuildcache = true;
    });
    parser.addOption({"-v", "--verbose"}, [&verbose] {
        verbose = true;
    });
    parser.setPositionalArgumentCallback([&filter] (vector<string> args) {
        filter = args;
    });
    parser.parse();
    
    
    if (nocache || NOCACHE) {
        if (filter.size() == 0) {
            for (auto entry : fs::directory_iterator("/bin")) {
                temp = entry.path();
                algo::erase_all(temp, "/bin/");
                // std::cout << temp << std::endl;
                commands.emplace_back(temp);
            };
        }
        else {
            commands = filter;
        }
        for (string name : commands) {
            string desc = getdesc(name);
            cout << desc << endl;
        };
        return 0;
    };
    
    if (exists(".bhprogs")) {
        if (verbose) {
            cout << "Program cache exists." << endl;
        }
        if (filter.size() == 0) {
            commands = readnsv(".bhprogs");
        }
        else {
            commands = filter;
        }
    }
    else {
        cout << "Program cache does not exist!" << endl;
        try
        {
            createfile(".bhprogs");
        }
        catch(int e)
        {
            // std::cerr << e.what() << '\n';
            cerr << "Error creating program cache. If this problem continues, try using the --nocache option." << endl;
            return 1;
        }
        for (auto entry : fs::directory_iterator("/bin")) {
            temp = entry.path();
            algo::erase_all(temp, "/bin/");
            // std::cout << temp << std::endl;
            commands.emplace_back(temp);
        };
        ofstream progcache(".bhprogs");
        for (string command : commands)
        {
            progcache << command << endl;
        };
        progcache.close();
    }
    if (rebuildcache) {
        goto rebuild_cache;
    };
    if (exists(".bhmans")) {
        if (verbose) {
            cout << "Description cache exists." << endl;
        }
        json::value jvdescs;
        string descs;
        string temp;
        ifstream file(".bhmans");
        json::stream_parser parser;
        json::object jodescs;
        parser.reset();
        while (getline(file, temp)) {
            parser.write(temp);
        };
        parser.finish();
        jvdescs = parser.release();
        jodescs = jvdescs.as_object();
        for (string command : commands) {
            temp = jodescs[command].as_string();
            algo::trim_left_if(temp, algo::is_any_of("\""));
            if (temp != "") {
                cout << temp << endl;
            };
        };
        file.close();
    }
    else {
        cout << "Description cache does not exist!" << endl;
        rebuild_cache:
        try
        {
            filesystem::remove(".bhmans");
            createfile(".bhmans");
        }
        catch(int e)
        {
            // std::cerr << e.what() << '\n';
            cout << "Error creating description cache." << endl;
            return 1;
        }
        cout << "Building description cache. This may take a while." << endl;
        // vector<progdesc> descs;
        // progdesc temp;
        json::object descs;
        for (string name : commands) {
            string desc = getdesc(name);
            cout << desc << endl;
            // temp.prog = name;
            // temp.desc = desc;
            descs[name] = desc;
        };
        ofstream bhmans(".bhmans");
        bhmans << json::serialize(descs);
        bhmans.close();
    }      
    return 0;
}