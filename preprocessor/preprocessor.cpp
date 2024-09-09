#include <cassert> 
#include <filesystem> 
#include <fstream> 
#include <iostream> 
#include <regex> 
#include <sstream> 
#include <string> 
#include <vector> 
 
using namespace std; 
using filesystem::path; 
 
const regex reqex_double_quote(R"/(\s*#\s*include\s*"([^"]*)"\s*)/"); 
const regex reqex_angle_brackets(R"(\s*#\s*include\s*<([^>]*)>\s*)"); 
 
path operator""_p(const char* data, std::size_t sz) { 
    return path(data, data + sz); 
} 
 
bool PreprocessRecursive(ifstream& in, ofstream& out, const path& file_location, const vector<path>& include_directories){ 
     
    string raw_string; 
    int count = 0; 
    smatch match_double_quote; 
    smatch match_angle_brackets; 
     
    while (getline(in, raw_string)){ 
        ++count; 
         
        //не найдено соответствие 
        if (!(regex_match(raw_string, match_double_quote, reqex_double_quote)||regex_match(raw_string, match_angle_brackets, reqex_angle_brackets))){ 
            out << raw_string << endl; 
        } else if (!match_double_quote.empty()){ 
             
            bool flag = false; 
            string include_file = string(match_double_quote[1]); 
            path expected_path = file_location.parent_path() / include_file; 
            ifstream input(expected_path); 
             
            if (input.is_open()){ 
                flag = true; 
                PreprocessRecursive(input, out, expected_path, include_directories); 
            } else { 
                for (const auto& path: include_directories){ 
                    expected_path = path / include_file; 
                    ifstream input(expected_path); 
                    if (input.is_open()){ 
                        flag = true; 
                        if (!PreprocessRecursive(input, out, expected_path, include_directories)){ 
                            return false; 
                        } 
                    } 
                } 
            } 
             
            if (!flag){ 
                cout << "unknown include file "s << include_file << " at file "s << file_location.string() << " at line "s << count << endl; 
                return false; 
            } 
             
        } else { 
             
            bool flag = false; 
            string include_file = string(match_angle_brackets[1]); 
             
            for (const auto& path: include_directories){ 
                class path expected_path = path / include_file; 
                ifstream input(expected_path); 
                if (input.is_open()){ 
                    flag = true; 
                    if (!PreprocessRecursive(input, out, expected_path, include_directories)){ 
                        return false; 
                    } 
                } 
            } 
             
            if (!flag){ 
                cout << "unknown include file "s << include_file << " at file "s << file_location.string() << " at line "s << count << endl; 
                return false; 
            } 
        } 
         
    } 
     
    return true; 
} 
 
bool Preprocess(const path& in_file, const path& out_file, const vector<path>& include_directories){ 
     
    ifstream in(in_file); 
     
    if (!in.is_open()){ 
        return false; 
    } 
    ofstream out(out_file); 
     
    bool res = PreprocessRecursive(in, out, in_file, include_directories); 
    return res; 
} 
 
string GetFileContents(string file) { 
    ifstream stream(file); 
 
    return {(istreambuf_iterator<char>(stream)), istreambuf_iterator<char>()}; 
} 
