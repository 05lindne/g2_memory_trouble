#ifndef CUSTOM_EXCEPTIONS_H
#define CUSTOM_EXCEPTIONS_H

#include <iostream>
#include <exception>
#include <stdexcept>
#include <sstream>
#include <string>

using namespace std;

class NotSortedException: public runtime_error {
  
  public:

    NotSortedException(const string s)
      : runtime_error( "Container data not sorted from low to high." ), path(s)  {}

    virtual const char* what() const throw()
    {
      cnvt.str( "" );

      cnvt << "Exception thrown: "<<  runtime_error::what() << endl;
      cnvt << "Data loaded from: " << getpath() << endl;
      cnvt << "Sorted order is neccessary for correct operation. Aborting.";

      return cnvt.str().c_str();
    }

    const string getpath() const
    { return path; }

  private:
    const string path;
    static ostringstream cnvt;
};

ostringstream NotSortedException::cnvt;

#endif // CUSTOM_EXCEPTIONS_H