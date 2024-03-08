#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "header.h"
#include "compress.h"

using namespace std;


/*
string get_zlib_header(){
    string zlib_header = "";
    int example_int = 1;
    string example_string = "1";
    char example_char = '1';

    // compress example_int with zlib_compress
    string compressed_int = zlib_compress(to_string(example_int));
    // compress example_string with zlib_compress
    string compressed_string = zlib_compress(example_string);
    // compress example_char with zlib_compress
    string compressed_char = zlib_compress(to_string(example_char));


    return gzip_header;
}
*/

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
    switch_case["block header lengths"] = 3;
    switch_case["block lengths"] = 4;
    switch_case["block sizes"] = 5;

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
            for (int i = stoi(header_list[0])+2+stoi(header_list[1])*2; i < stoi(header_list[0])+2+stoi(header_list[1])*2+2; i++){
                header_query_list.push_back(header_list[i]);
            }
            break;
        default:
            cout << "Error: header_query not found." << endl;
            exit(1);
    }

    return header_query_list;
}
