#ifndef LIBRARY_HH
#define LIBRARY_HH

#include <string>


struct Book {
    std::string title;
    std::string author;
    int reservations;
};

class library
{
public:
    library();
};

#endif // LIBRARY_HH
