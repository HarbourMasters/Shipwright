#include "mutils.h"

#include <codecvt>
#include <fstream>
#include <algorithm>
#include <cstring>

#define IS_64_BIT (UINTPTR_MAX == 0xFFFFFFFFFFFFFFFFU)
#define IS_BIG_ENDIAN (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)

namespace fs = std::filesystem;
using namespace std;

namespace MoonUtils {

#if IS_BIG_ENDIAN
    const char* endian = "big";
#else
    const char* endian = "little";
#endif

#if IS_64_BIT
    const char* bitwidth = "64";
#else
    const char* bitwidth = "32";
#endif

    const char* cwd = NULL;

    char *sys_strdup(const char *src) {
        const unsigned len = strlen(src) + 1;
        char *newstr = new char[len];
        if (newstr) memcpy(newstr, src, len);
        return newstr;
    }

    char pathSeparator() {
		#ifdef _WIN32
		        return '\\';
		#else
		        return '/';
		#endif
    }

    string normalize(string path){
#ifdef _WIN32
        ranges::replace(path, '/', '\\');
#else
        replace(path.begin(), path.end(), '\\', '/');
#endif
        return path;
    }

    string join(string base, string file){
        if( file == "/" ) return normalize(base + file);
        return normalize( (fs::path(base) / fs::path(file)).string() );
    }

    string dirname(string path){
        return normalize( fs::path(path).parent_path().string());
    }

    string filename(string path){
        return normalize( fs::path(path).filename().string() );
    }

    string basename(string path){
        return normalize( fs::path(path).stem().string() );
    }

    vector<string> split (const string &s, char delim) {
        vector<string> result;
        stringstream ss (s);
        string item;
        while (getline(ss, item, delim)) 
        {
            if (item.at(item.size() - 1) == '\r')
                item = item.substr(0, item.size() - 1);

            result.push_back (item);
        }
        return result;
    }

    std::string concat(std::vector<std::string> list, std::string separator) {
        std::string output;
        for (auto& entry : list) output.append(entry + separator);
        return output;
    }

    bool exists(string path){
        return fs::exists(path);
    }

    void mkdir(string path){
        fs::create_directories(path);
    }

    void rm(string path){
        if(!fs::exists(path)) return;
        if (fs::is_directory(path)) fs::remove_all(path);
        else fs::remove(path);
    }

    string absolute(string path) {
        return normalize(fs::absolute(path).string());
    }

    string relative(string parent, string child){
        return normalize(fs::relative(child, parent).string());
    }

    void move(string src, string dst){
	#ifdef _WIN32
        // if (!MoveFileExA(const_cast<char*>(src.c_str()), const_cast<char*>(dst.c_str()), MOVEFILE_COPY_ALLOWED)) {
        //     printf ("MoveFileEx failed with error %d\n", GetLastError());
        //     return;
        // }
    #else
        vector<string> trashcan;
        for(auto& p: fs::recursive_directory_iterator(src)){
            // Create path in target, if not existing.
            const auto relativeSrc = fs::relative(p, src);
            const auto targetParentPath = dst / relativeSrc.parent_path();
            fs::create_directories(targetParentPath);
            // Copy to the targetParentPath which we just created.
            fs::copy(p, targetParentPath, fs::copy_options::overwrite_existing);

            string dname = p.path().parent_path().string();
            trashcan.push_back(dname);
        }
        for(auto& p: trashcan) rm(p);
    #endif
    }

    void copy(string src, string dst){
        if(!fs::exists(dirname(dst))) mkdir(dirname(dst));
        fs::copy(src, dst, fs::copy_options::update_existing | fs::copy_options::recursive);
    }

    void write(std::string path, char* data, int size){
        mkdir(dirname(path));
        ofstream outfile(path, ios::out | ios::binary);
        outfile.write(data, size);
        outfile.flush();
        outfile.close();
    }

    void writeFile(string path, string content){
        ofstream out(path);
        out << content;
        out.close();
    }

    void dirscan(string path, vector<string> &files){
        if(!fs::is_directory(path)) return;
        for (auto& p : fs::directory_iterator(path)){
            string fpath = p.path().string();
            if(fs::is_directory(fpath))
            {
                dirscan(fpath, files);
            } else {
                files.push_back(normalize(fpath));
            }
        }
    }

    string extname(string file) {
        const char *filename = file.c_str();
        const char *dot = strrchr(filename, '.');
        if(!dot || dot == filename) return "";
        return string(dot + 1);
    }

	std::wstring wide(const std::string& str) {
        return std::wstring_convert<codecvt_utf8<wchar_t>>().from_bytes(str);
    }
	std::string narrow(const std::wstring& str) {
        return std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(str);
    }
}