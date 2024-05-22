#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "header.h"
#include "compress.h"

using namespace std;

/*
 * Function to parse header list
 * @param header_list: vector<string>
 * @param header_query: string
 * @return header_query_list: vector<string>
 */
vector<string> parse_header_list(
        vector<string> header_list,
        string header_query){
    vector<string> header_query_list;
    map<string, int> switch_case;
    switch_case["num columns"] = 0;
    switch_case["num blocks"] = 1;
    switch_case["column names"] = 2;
    switch_case["block header end bytes"] = 3;
    switch_case["block end bytes"] = 4;
    switch_case["block sizes"] = 5;

    int num_blocks = stoi(header_list[1]);

    // switch statement for query element
    switch (switch_case[header_query]) {
        case 0:
            header_query_list.push_back(header_list[0]);
            break;
        case 1:
            header_query_list.push_back(header_list[1]);
            break;
        case 2:
            for (int i = 2; i < stoi(header_list[0])+2; i++){
                header_query_list.push_back(header_list[i]);
            }
            break;
        case 3:
            for (int i = stoi(header_list[0])+2; i < stoi(header_list[0])+2+stoi(header_list[1]); i++){
                header_query_list.push_back(header_list[i]);
            }
            break;
        case 4:
            for (int i = stoi(header_list[0])+2+stoi(header_list[1]); i < stoi(header_list[0])+2+stoi(header_list[1])*2; i++){
                header_query_list.push_back(header_list[i]);
            }
            break;
        case 5:
//            for (int i = stoi(header_list[0])+2+stoi(header_list[1])*2; i < stoi(header_list[0])+num_blocks+stoi(header_list[1])*2+num_blocks; i++){
            for (int i = stoi(header_list[0])+2+stoi(header_list[1])*2; i < stoi(header_list[0])+2+stoi(header_list[1])*2+num_blocks; i++){
                if (header_list[i] != "") {
                    header_query_list.push_back(header_list[i]);
                }else{ break; }
            }
            break;
        default:
            cout << "Error: header_query not found." << endl;
            exit(1);
    }

    return header_query_list;
}

/*
 * Function to remove the common header from
 * a string compressed with zlib
 */
string remove_zlib_header(string compressed_string,
                          string zlib_header){
    string zlib_header_removed = compressed_string.substr(zlib_header.length());
    return zlib_header_removed;
}

string add_zlib_header(string compressed_string,
                       string zlib_header){
    string zlib_header_added = zlib_header + compressed_string;
    return zlib_header_added;
}
