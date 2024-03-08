#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <zlib.h>
#include <fstream>
#include <sstream>
#include <map>

#include "header.h"
#include "compress.h"

using namespace std;

/*
 * Function to get the delimiter of a file
 * @param line: string
 * @return delimiter: char
 */
char get_delimiter(
        string line){
    char delimiter;
    // determine delimiter
    if(line.find('\t') != string::npos){
        delimiter = '\t';
    } else if(line.find(',') != string::npos){
        delimiter = ',';
    } else if(line.find(' ') != string::npos){
        delimiter = ' ';
    } else {
        cout << "Error: delimiter not found.\n"
                " Please use files with delimiters as tab, comma, or space." << endl;
        exit(1);
    }
    return delimiter;
}

/*
 * Function to get the column names of a file
 * @param line: string
 * @param delimiter: char
 * @return column_names_str: string
 */
string get_column_names(
        string line,
        char delimiter){

    string column_names_str;
    // split line by delimiter and store in comma separated string
    stringstream ss(line);
    string item;
    while(getline(ss, item, delimiter)){
        // remove carriage return from item
        item.erase(remove(item.begin(), item.end(), '\r'), item.end());
        column_names_str += item + ",";
    }
    // remove last comma
    column_names_str.pop_back();

    return column_names_str;
}

/*
 * Function to get the column types of a file
 * @param line: string
 * @param delimiter: char
 * @return column_types_str: string
 */
string get_column_types(
        string line,
        char delimiter) {

    string column_types_str;
    // split line by delimiter find data type and store in comma separated string
    stringstream ss(line);
    string item;
    while (getline(ss, item, delimiter)) {
        // check if item is int
        // check if item is float
        // else  item stays as string
        // if none of the above, throw error
        // search for '.' in item
        if (item.find('.') != string::npos) {
            column_types_str += "float,";
        } else {
            try {
            stoi(item);
            column_types_str += "int,";
            }
            catch (invalid_argument &e) {
                column_types_str += "string,";
            }
        }
    }
    // remove last comma
    column_types_str.pop_back();
    return column_types_str;
}
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
