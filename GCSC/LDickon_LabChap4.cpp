/*
    Logan Dickon
    September 1, 2020
    COP2224 C++ Programming
    Lab Chapter 4
*/

// Libraries
    #include <iostream> // Import iostream Library
    #include <cmath> // Import cmath Library

// Namespaces
    using namespace std; // std:: Class Namespace

// Initalize Variables & Constants
    int menu; // Menu Option Value
    double radius = 0;              // Circle's Radius
    double length = 0;              // Rectangle or Triangle's length
    double width = 0;               // Rectangle's Width
    double height = 0;              // Triangle's Height
    double area = 0;                // Calculated Area
    const double PI = 3.14159;      // Value Of PI

// Main Code Blocks:
    int main() // main Function
    {
        // Calculator Menu
            cout << "Geometry Calculator\n\n";                  // Log To Console
            cout << "1. Calculate the Area of a Circle\n";      //
            cout << "2. Calculate the Area of a Rectangle\n";   //
            cout << "3. Calculate the Area of a Triangle\n";    //
            cout << "4. Quit!\n";                               //
            cout << "Enter you choice (1-4): ";                 //
            cin >> menu;                                        // Ask User To Enter Their Choice
            
        // Calculator Functionality
            if (menu == 1) // Menu Option 1
            {
                cout << "\nEnter The Circle\'s Radius: ";       //
                cin >> radius;                                  // Ask User For Radius of Circle
                area = (PI * pow(radius, 2));                   // Calculate Area of Circle
                cout << "\n-------------------------------";    //
                cout << "\nThe Circle\'s Area Is: " << area;    //
                cout << "\n-------------------------------";    //
            } 
            else if (menu == 2) // Menu Option 2
            {
                cout << "\nEnter The Rectangle\'s Length & Width: ";    //
                cin >> length >> width;                                 // Ask User For Length & Width of Rectangle
                area = (length * width);                                // Calculate Area of Rectangle
                cout << "\n-----------------------------------";        //
                cout << "\nThe Rectangles\'s Area Is: " << area;        //
                cout << "\n-----------------------------------";        //
            }
            else if (menu == 3) // Menu Option 3
            {
                cout << "\nEnter The Triangle\'s Base & Height: ";      //
                cin >> length >> height;                                // Ask User For Base & Height of Triangle
                area = (.5 * (length * height));                        // Calculate Area of Triangle
                cout << "\n----------------------------------";         //
                cout << "\nThe Triangles\'s Area Is: " << area;         //
                cout << "\n----------------------------------";         //
            }
            else if (menu == 4) // Menu Option 4
            {
                cout << "\n----";
                cout << "\nBye!";
                cout << "\n----";
            }
            else // Menu Error
            {
                cout << "\n------------------------------";
                cout << "\nERROR! Please Only Enter (0-4)";
                cout << "\n------------------------------";
            }
            
            return 0;
    }
