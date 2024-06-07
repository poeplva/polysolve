#include <iostream>
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <string>

using namespace std;

/**

notes:
- to be safe, I make all argument declarations const whenever I know the function will not change the given argument
- to reduce memory usage, I pass all arguments by reference whenever its ok to do so

**/

// the minimum number of arguments the program expects, 2 for the polynomial's coefficients, 3 for the other parameters
const int min_number_of_arguments = 2 + 3;
const int max_number_of_iterations = 10e6;

// function that checks whether given string is numeric (is a double) or not
bool is_numeric(const char* s) {
	double test;
	auto str_stream = istringstream(s);
	str_stream >> test;
	// check if the string stream reaches the end by checking the eof flag, if it does not reach the end then it means that the string s contains non-numeric characters
	return !str_stream.fail() && str_stream.eof();
}

// function that outputs the sign of given double
int sign(double x) {
	if (x < 0) {
		return -1;
	}
	return 1;
}

// print usage information to help the user use the program
void usage(const char* program_name) {
	cout << "SYNOPSIS:" << endl;
	cout << program_name << " double(COEFFICIENT_n) double(COEFFICIENT_n-1) ... double(INITIAL_GUESS_1) double(INITIAL_GUESS_2) double(TOLERANCE_LEVEL)" << endl << endl;
	cout << "This program solves a given generic polynomial using two different algorithms." << endl;
	cout << "These algorithms (namely secant and bisection) requires the user to input two initial guesses." << endl;
	cout << "Then both of the algorithms will find a root that lies in between the initial guesses." << endl;
	cout << "Hence, the program expects two arguments for the initial guesses, and also an argument to specify the tolerance level." << endl;
	cout << "The tolerance level simply means that the program will output a solution such that the real solution and the output will differ at most by this amount." << endl;
	cout << "The rest of the arguments are the coefficients of the polynomial to solve, there must be at least 2 coefficients specified since solving a constant polynomial is not of interest or value." << endl;
	cout << "Hence there must be at least " << min_number_of_arguments << " arguments to this program." << endl;

	exit(0);
}

class Polynomial {
	int degree;
	double* coefs;

public:
	// constructor that takes the degree and the coefficients of the polynomial to create the polynomial object
	Polynomial(int degree_, double* coefs_) : degree(degree_), coefs(coefs_) {}

	// print the polynomial
	void print();

	// evaluate at the point x
	double evaluate(double x) const;
};

void Polynomial::print() {
	cout << coefs[0];
	for (int i = 1; i <= degree; i++) {
		cout << " + " << coefs[i] << "x^" << i;
	}
	cout << endl;
}

double Polynomial::evaluate(double x) const {
	double res = 0;
	for (int i = 0; i <= degree; i++) {
		res += coefs[i] * pow(x, i);
	}
	return res;
}

// function that checks if the user input is valid, and returns the polynomial object the user put in
Polynomial interpret_args(const int &argc, const char* argv[], double &guess_1, double &guess_2, double &tol) {
	// if no arguments are passed to the program, print out the usage string
	if (argc == 1) {
		usage(argv[0]);
	}

	string err_message;

	// if not enough arguments, throw an error. not particularly necessary since we will later check the degree. but still useful to quickly exit the program in case not enough arguments are provided
	if (argc < min_number_of_arguments + 1) {
		err_message = "This program expects at least " + to_string(min_number_of_arguments) + " arguments!";
		throw invalid_argument(err_message);
	}

	// check if all arguments are numeric, and throw an error if not. not particularly necessary since we will later try to cast them to doubles later, but still useful to check quickly and exit before with a clear error message
	for (int i = 1; i < argc; i++) {
		if (!is_numeric(argv[i])) {
			err_message = "Every argument to this program must be a number! Argument #" + to_string(i) + " (" + argv[i] + ") is not a number!";
			throw invalid_argument(err_message);
		}
	}

	guess_1 = stod(argv[argc - 3]);
	guess_2 = stod(argv[argc - 2]);
	tol = stod(argv[argc - 1]);

	if (guess_1 == guess_2) {
		err_message = "Initial guesses must be different!";
		throw invalid_argument(err_message);
	}
	if (tol <= 0) {
		err_message = "Tolerance level must be a positive integer!";
		throw invalid_argument(err_message);
	}

	// create the polynomial object
	int size = argc - 4;
	double* poly_arr = new double[size];
	for (int i = 1; i < size + 1; i++) {
		poly_arr[size - i] = stod(argv[i]);
	}
	// get the last nonzero coefficient
	int degree = 0;
	for (int i = 0; i < size; i++) {
		if (poly_arr[i] != 0) {
			degree = i;
		}
	}
	// throw an error if degree is low
	if (degree < 1) {
		err_message = "The polynomial should be of degree at least 1!";
		throw invalid_argument(err_message);
	}
	Polynomial poly(degree, poly_arr);
	return poly;
}

// function to apply bisection algorithm
double bisection(const Polynomial &f, double &guess_1, double &guess_2, const double &tol, int &no_of_iterations, const int &iteration = max_number_of_iterations) {
	if (sign(f.evaluate(guess_1)) == sign(f.evaluate(guess_2))) {
		throw invalid_argument("Bisection method only applicable when the polynomial changes sign between the two guesses!");
	}
	if (abs(f.evaluate(guess_1)) < tol) return guess_1;
	if (abs(f.evaluate(guess_2)) < tol) return guess_2;

	int i;
	double guess_3 = guess_1 + (guess_2 - guess_1) / 2;
	for (i = 0; (i < iteration && abs(f.evaluate(guess_3)) > tol); i++) {
		guess_3 = guess_1 + (guess_2 - guess_1) / 2;
		if (sign(f.evaluate(guess_3)) != sign(f.evaluate(guess_1))) {
			guess_2 = guess_3;
			continue;
		}
		guess_1 = guess_3;
	}

	no_of_iterations = i;
	return guess_3;
}

// function to apply secant algorithm
double secant(const Polynomial &f, double &guess_1, double &guess_2, const double &tol, int &no_of_iterations, const int &iteration = max_number_of_iterations) {
	if (abs(f.evaluate(guess_1)) < tol) return guess_1;
	if (abs(f.evaluate(guess_2)) < tol) return guess_2;

	int i;
	double guess_3 = guess_2 - f.evaluate(guess_2) * (guess_2 - guess_1)/(f.evaluate(guess_2) - f.evaluate(guess_1));
	for (i = 0; (i < iteration && abs(f.evaluate(guess_3)) > tol); i++) {
		guess_3 = guess_2 - f.evaluate(guess_2) * (guess_2 - guess_1)/(f.evaluate(guess_2) - f.evaluate(guess_1));
		guess_1 = guess_2;
		guess_2 = guess_3;
	}

	if (abs(f.evaluate(guess_3)) > tol) {
		no_of_iterations = 0;
	} else {
		no_of_iterations = i;
	}
	return guess_3;
}

void print_result(string alg, const double &root, const int &no_of_iterations) {
	cout << "The root found by using the " << alg << " algorithm is " << root << ". It took " << no_of_iterations << " iterations for the algorithm to conclude." << endl;
}

int main(const int argc, const char* argv[]) {
	double guess_1, guess_2, tol;
	// interpret the inputs
	Polynomial f = interpret_args(argc, argv, guess_1, guess_2, tol);

	int no_of_iterations;
	// apply bisection algorithm
	double bi_guess_1 = guess_1;
	double bi_guess_2 = guess_2;
	double bi_root = bisection(f, bi_guess_1, bi_guess_2, tol, no_of_iterations);
	print_result("bisection", bi_root, no_of_iterations);

	// apply secant algorithm
	double sec_guess_1 = guess_1;
	double sec_guess_2 = guess_2;
	double sec_root = secant(f, sec_guess_1, sec_guess_2, tol, no_of_iterations);
	if (no_of_iterations == 0) {
		cout << "The algorithm did not converge and hit the maximum number of iterations (" << max_number_of_iterations << ")." << endl;
	} else {
		print_result("secant", sec_root, no_of_iterations);
	}
	// appyl hybrid algorithm
	double hyb_guess_1 = guess_1;
	double hyb_guess_2 = guess_2;
	bisection(f, hyb_guess_1, hyb_guess_2, tol, no_of_iterations, 2);
	int no_of_iterations_ = no_of_iterations;
	double hyb_root = secant(f, hyb_guess_1, hyb_guess_2, tol, no_of_iterations);
	if (no_of_iterations == 0) {
		cout << "The algorithm did not converge and hit the maximum number of iterations (" << max_number_of_iterations << ")." << endl;
	} else {
		print_result("hybrid", hyb_root, no_of_iterations + no_of_iterations_);
	}

	return 0;
}
