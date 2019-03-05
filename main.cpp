/*
####################################################################
# TIE-02207 Programming 2: Basics, S2019                           #
#                                                                  #
# Project2: library                                                #
# Program description: Works likes library websites, you can       #
# search for library, author or book. And it will so details for it#
# File: main.cpp                                                   #
# Description: Starts executing the program.                       #
#                                                                  #
# Author: Niklas Nurminen, 283756., niklas.nurminen@tuni.fi        #
####################################################################
*/


#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <set>
#include <fstream>
#include <algorithm>



struct Book {
    std::string title;
    std::string author;
    int reservations;
};

using Books = std::vector<Book>;
using Authors = std::map<std::string,Books>;
using Libraries = std::map<std::string,Authors>;

const int ON_THE_SHELF = -1;
const int MAX_RESERVATIONS = 100;

/* Splits incoming s to vector.
s: splitattava stringi
delimiter: is a character, and it tells to function where to split
ignore_empty: true if we dont want empty
return: returns strings parts in vector
*/
std::vector<std::string> split(const std::string& s, const char delimiter, bool ignore_empty = false){
    std::vector<std::string> result;
    std::string tmp = s;

    while(tmp.find(delimiter) != std::string::npos)
    {
        std::string new_part = tmp.substr(0, tmp.find(delimiter));
        tmp = tmp.substr(tmp.find(delimiter)+1, tmp.size());
        if(not (ignore_empty and new_part.empty()))
        {
            result.push_back(new_part);
        }
    }
    if(not (ignore_empty and tmp.empty()))
    {
        result.push_back(tmp);
    }
    return result;
}

/*
Makes a new "book" and inserts it to our map, also detects if new reservations
information comes to our knowledge and updates it.
Libs: Datastructure that I am using for this project
library: library name
author: book's author
book: book's name/title
reservations: how many reservations book has
*/
void insert_book(Libraries& libs, const std::string& library, const std::string& author,
                 const std::string& book, int reservations){
    if(libs.find(library) == libs.end()){
        libs.insert({library,{}});
    }

    if(libs.at(library).find(author) == libs.at(library).end()){
        libs.at(library).insert({author,{}});
    }

    Books& books = libs.at(library).at(author);
    for(Book& old_book : books ){
        if(old_book.title == book && old_book.author == author){
            old_book.reservations = reservations;
            return;
        }
    }
    books.push_back({book, author, reservations});

}

/*
Asks user to give filename. Opens the file and first it splits the content.
Then we call our function "insert_book" to insert them to datastructure
libs: Datastructre which is going to save all data from file.
return: returns true, if passed whole function without error else false.
*/
bool readInputFile(Libraries& libs){
    std::string file_name;
    std::cout << "Input file: ";
    std::getline(std::cin, file_name);

    std::ifstream file(file_name);
    if(!file){
        std::cout << "Error: the input file cannot be opened" << std::endl;
        return false;
    }

    std::string line;
    while(std::getline(file,line)){
        std::vector<std::string> parts = split(line, ';', true);
        if(parts.size() != 4){
            std::cout << "Error: empty field" << std::endl;
            file.close();
            return false;
        }

        std::string library = parts.at(0);
        std::string author = parts.at(1);
        std::string book = parts.at(2);
        std::string reservations_str = parts.at(3);

        int reservations;
        if(reservations_str == "on-the-shelf"){
            reservations = ON_THE_SHELF;
        }else{
            reservations = std::stoi(reservations_str);
        }

        insert_book(libs, library, author, book, reservations);

    }

    file.close();
    return true;
}

/*
Finds specific library, which has lowest reservations for the book.
libs: Datastructure that holds information from the file
Fields: Gives function information what book user is searching.
*/
void reservables(const Libraries& libs, const std::vector<std::string>& fields){
    if(fields.size() < 2){
        std::cout << "Error: error in command reservable" << std::endl;
        return;
    }

    std::string book_name;
    for(unsigned i = 1; i < fields.size() - 1; ++i){
        book_name += fields.at(i);
        book_name += " ";
    }

    book_name += fields.back();

    if(book_name.front() == '\"' && book_name.back() == '\"'){
        book_name = book_name.substr(1, book_name.size() - 2);
        if(book_name.empty()){
            std::cout << "Error: error in command reservable" << std::endl;
            return;
        }
    }

    //Finds the shortest possible reservation for book.
    int shortest = MAX_RESERVATIONS;
    std::set<std::string> shortest_libs;
    for(const auto& library : libs){
        for(const auto& authors : library.second){
            for(const auto& book  : authors.second){
                if(book.title == book_name){
                    if(book.reservations < shortest){
                        shortest_libs.clear();
                        shortest = book.reservations;
                        shortest_libs.insert(library.first);
                    }else if(book.reservations == shortest){
                        shortest_libs.insert(library.first);
                    }
                }
            }
        }
    }

    if(shortest_libs.empty()){
        std::cout << "Book is not a library book." << std::endl;
    }else if(shortest == MAX_RESERVATIONS){
        std::cout << "The book is not reservable from any library." << std::endl;
    }else{
        if(shortest == ON_THE_SHELF){
            std::cout << "on the shelf" << std::endl; //EHKÄ?
        }else{
            std::cout << shortest << " reservations" << std::endl;
        }
        for(const std::string& lib : shortest_libs){
            std::cout << "--- " << lib << std::endl;
        }
    }
}

void print_libs(Libraries& libs, const std::vector<std::string>& fields){
    if(fields.size() > 1){
        std::cout << "Error: error in command libraries" << std::endl;
        return;
    }

    for (Libraries::iterator it=libs.begin(); it!=libs.end(); ++it)
        std::cout << it->first << std::endl;

}

/*
Prints all books that specific library has.
libs: Datastructure which holds the file information
fields: Must have two values. Second value is library name.
How it works: It add's components to empty string, then pushes them to vector.
Then vector is sorted in alphabetical order and printed out.
*/
void print_material(Libraries& libs, const std::vector<std::string>& fields){
    if(fields.size() > 2){
        std::cout <<"Error: error in command material" << std::endl;
        return;
    }

    if(libs.find(fields.at(1)) == libs.end()){
        std::cout << "Error: unknown library name" << std::endl;
        return;
    }

    std::vector<std::string> temporary_vec;
    std::string temporary_string;

    for(const auto& author : libs.at(fields.at(1))){
        for(const auto& book : author.second){
            temporary_string += author.first;
            temporary_string += ": ";
            temporary_string += book.title;
            temporary_vec.push_back(temporary_string);
            temporary_string.clear();
        }
    }

    std::sort(temporary_vec.begin(), temporary_vec.end());

    for(unsigned long int i=0; i < temporary_vec.size(); ++i){
        std::cout << temporary_vec.at(i) << std::endl;
    }

}

/*
Prints all books that library has in specific under author's name.
libs: Datastructure which holds the file information
fields: Must have three values in vector, where second is library name
and third value is author.
How it works: It add's components to empty string, then pushes them to vector.
Then the vector is going to be sorted in alphabetical order then printed out.
*/
void print_books(Libraries& libs, const std::vector<std::string>& fields){
    if(fields.size() != 3){
        std::cout << "Error: error in command books" << std::endl;
        return;
    }else if(libs.find(fields.at(1)) == libs.end()){
        std::cout << "Error: unknown library name" << std::endl;
        return;
    }else if(libs.at(fields.at(1)).find(fields.at(2)) == libs.at(fields.at(1)).end()){
        std::cout << "Error: unknown author" << std::endl;
        return;
    }

    std::vector<std::string> temporary_vec;
    std::string temporary_string;

    for(const auto& books : libs.at(fields.at(1)).at(fields.at(2))){
        temporary_string += books.title;
        temporary_string += " --- ";
        if(books.reservations == -1){
            temporary_string += "on the shelf";
        }else{
            std::string s = std::to_string(books.reservations);
            temporary_string += s + " reservations";
        }
        temporary_vec.push_back(temporary_string);
        temporary_string.clear();
    }

    std::sort(temporary_vec.begin(), temporary_vec.end());

    for(unsigned long int i = 0; i < temporary_vec.size(); ++i){
        std::cout << temporary_vec.at(i) << std::endl;
    }


}

/*
Prints out all loanable bookss from every library.
libs: Datastructure that holds information from the file.
How it works: We use set datastructure, because it automatically sorts it in alphabetical order.
Empty string get's book's author and title, then inserted to set datastructure. Then printed out.
*/
void print_loanables(const Libraries& libs){

    std::set<std::string> temp_set;
    std::string temp_string;

    for(const auto& library : libs){
        for(const auto& authors : library.second){
            for(const auto& book  : authors.second){
                if(book.reservations == -1){
                    temp_string += book.author;
                    temp_string += ": ";
                    temp_string += book.title;
                    temp_set.insert(temp_string);
                    temp_string.clear();
                }
            }
        }
    }

    for(const auto& loanable : temp_set){
        std::cout << loanable << std::endl;

    }
}

/*
Asks command from user while user is not quitting or error's hasnt came up.
How it works: it splits the command, selected right option and call for right function.
*/
int main(){
    Libraries libs;

    if(!readInputFile(libs)){
        return EXIT_FAILURE;
    }

    while(true){
        std::string command;
        std::cout << "> ";
        std::getline(std::cin, command);

        std::vector<std::string> parts = split(command, ' ');

        if(parts.at(0) == "libraries"){

            print_libs(libs, parts);

        }else if(parts.at(0) == "material"){

            print_material(libs, parts);

        }else if(parts.at(0) == "reservable"){

            reservables(libs, parts);

        }else if(parts.at(0) == "books"){

            print_books(libs, parts);

        }else if(parts.at(0) == "loanable"){

            print_loanables(libs);

        }else if(parts.at(0) == "quit"){

            if(parts.size() > 1){
                std::cout << "Error: error in command quit" << std::endl;
                continue;
            }else{
                return EXIT_SUCCESS;
            }
        }else{
            std::cout << "Error: Unknown command: " << parts.at(0) << std::endl;
        }

    }

    return 0;

}
