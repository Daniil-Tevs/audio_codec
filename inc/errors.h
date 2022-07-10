#include <stdexcept>
#include <iostream>
class MyException: public std::exception{
private:
public:
    MyException(char* msg):std::exception(msg)
    {}
//    const char * what() const override
//    {
//        return "Error: this type of fail is don't available in this method";
//    }
};