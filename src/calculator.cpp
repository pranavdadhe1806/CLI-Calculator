#include<iostream>
#include<vector>
#include"calculator.h"

using namespace std;
int main(){
    cout<<"--------------------Calculator--------------------\n\n";
    
    int count;
    cout << "Tell me how many numbers you'd like to calculate: ";
    cin >> count;
    
    vector<double> numbers(count);
    cout << "Enter each number and press ENTER after each:\n";
    for (int i = 0; i < count; i++) {
        cout << "Number " << (i + 1) << ": ";
        cin >> numbers[i];
    }

    cout << "\nThe numbers you entered are: ";
    for (double n : numbers) cout << n << ", ";
    cout << endl;

    while (true) {
    cout << "\nSelect operation (+, -, *, /). Press any other key to exit: ";
    char operation;
    cin >> operation;

    if (!(operation == '+' || operation == '-' || operation == '*' || operation == '/')) {
        cout << "Exiting the calculator. Goodbye!\n";
        break; 
    }

    double result;
    switch(operation) {
        case '+': result = add(numbers); break;
        case '-': result = subtract(numbers); break;
        case '*': result = multiply(numbers); break;
        case '/': result = divide(numbers); break;
    }

    cout << "Result: " << result << endl;
    }

    return 0;
}