// Interpolacja_Newtona.cpp 
//Patryk Heizig, Tomasz Ratajczak

#include "pch.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>

using namespace std;

//oblicza wartosc zadanej funkcji dla podanego x
inline double funkcja(double x)
{
	return abs(sin(x)) / (1 + pow(x, 2)); // |sin x| / (1 + x^2)
}

//funkcja wprowadzająca dane
void wprowadz_dane(double &a, double &b, int &n, int &np)
{
	cout << "Podaj (a) i wcisnij enter: ";
	cin >> a;
	cout << "Podaj (b) i wcisnij enter: ";
	cin >> b;
	cout << "Wprowadz liczbe wezlow (n) i nacisnij enter: ";
	cin >> n;
	cout << "Wprowadz (np) i nacisnij enter: ";
	cin >> np;
}

//funkcja tablicująca węzły i wartości funkcji interpolowanej f(x) w tych węzłach
void tab_wezly_fx(double *xp, double *fx, double a, double b, int n, int np)
{
	double hp;
	hp = ((b - a) / (double)np);
	for (int j = 0; j <= np; j++)
	{
		xp[j] = a + (j * hp);
		fx[j] = funkcja(xp[j]);
	}
}

//funkcja wyznaczająca współczynniki wielomianu Newtona (ilorazy różnicowe)
//parametry:	f_roznicowy - tablica wartosci funkcji f(x)
//				xp - tablica wartosci xp(j)
//				rzad - aktualny rząd obliczeń
double rekurencja_iloraz_roznicowy(double *f_roznicowy, double* xp, int rzad)
{
	//blednie wprowadzone dane
	if (rzad < 0)
	{
		cerr << "Niepoprawny rzad w rekurencji" << endl;
		return 0;
	}
	//warunek stopu dla rekurencji
	if (rzad == 0)
	{
		return f_roznicowy[0];
	}
	else
	{
		double *f_lewy = new double[rzad - 1]; //nowa tabela f[ x(i+1) ... x(i+j) ]
		for (int i = 0; i < rzad; i++)
		{
			f_lewy[i] = f_roznicowy[i + 1];
		}
	
		double *f_prawy = new double[rzad - 1]; //nowa tabela f[ x(i) ... x(i+j-1) ]
		for (int i = 0; i < rzad; i++)
		{
			f_prawy[i] = f_roznicowy[i];
		}

		//sprawdzenie dzielenia przez zero
		if ((xp[rzad] - xp[0]) == 0)
		{
			cerr << "Dzielenie przez zero w mianowniku (w rekurencji)" << endl;
			throw 0; //nie mozna dzielic przez zero
		}
		else
		//f[ x(i+1) ... x(i+j) ] - f[ x(i) ... x(i+j-1) ] / ( x(i+j) - (x(i))
		{
			return ((rekurencja_iloraz_roznicowy(f_lewy, xp, rzad - 1) - rekurencja_iloraz_roznicowy(f_prawy, xp, rzad - 1))
				/ (xp[rzad] - xp[0]));
		}
		
		//zwolnienie pamięci
		delete[] f_lewy;
		delete[] f_prawy;
	}
}


//funkcja wyznaczająca wartości wielomianu Newtona w dowolnym punkcie
// parametry:	punkt x, w którym poszukujemy wartości wielomianu
//				fx - tablica f(x) - wartości funkcji interpolowanej
//				xp - tablica x(p) - punktów
//				rzad - liczba węzłów
//zwraca = wartosc w podanym punkcie
double wartosc_w_punkcie(double punkt, double *fx, double* xp, int rzad)
{
	double *alfa = new double[rzad + 1];
	double wynik = 0.0;

	for (int i = 0; i <= rzad; i++)
	{
		// wyznaczenie alfy
		alfa[i] = rekurencja_iloraz_roznicowy(fx, xp, i);

		//wyznaczenie omegi
		double omega = 1;
		if (i > 0)
			for (int j = 0; j < i; j++)
			{
				omega *= (punkt - xp[j]);
			}
		wynik += alfa[i] * omega;
	}
	return wynik;
	
	//zwolnienie pamięci
	delete[] alfa;
}

//funkcja wyznaczająca tablice: punktów, wartości funkcji interpolowanej i wielomianu Newtona 
//xpj, f(xpj), Ln(xpj) i zapisujace te 3 tabele do pliku
//parametry:	a - lewa strona przedziału
//				b - prawa strona przedziału
//				n - liczba węzłów równoodległych
//				np - liczba węzłów dobranych optymalnie
void wydrukuj_tablice(double a, double b, int n, int np)
{
	double *xp = new double[n + 1];
	double *fx = new double[n + 1];

	//generowanie tablicy xp(j) i f(xpj)
	tab_wezly_fx(xp, fx, a, b, n, np);

	//generowanie tablicy Ln(xpj)
	double *lnxp = new double[n + 1];
	for (int i = 0; i <= n; i++)
	{
		lnxp[i] = wartosc_w_punkcie(xp[i], fx, xp, n);
	}

	////////// praca na pliku ///////////

	//okreslenie nazwy pliku wyjsciowego
	cout << "Podaj nazwe pliku wyjsciowego: ";
	string nazwa;
	cin >> nazwa;

	//utworzenie pliku do zapisu
	ofstream out;
	out.open(nazwa);

	out << "Interpolacja Newtona, plik wyjsciowy \"" << nazwa << "\"\n\n";
	out << "Wprowadzono następujące dane: a = " 
		<< a << " b = " << b << " n = " << n << " np = " << np << " Rzad (n) = " << n << endl;

	//drukuj xpj
	out << "\nxpj:" << endl;
	for (int j = 0; j <= n; j++)
	{
		out <<  xp[j] << endl;
	}

	//drukuj f(xpj)
	out << "\nf(xpj):" << endl;
	for (int j = 0; j <= n; j++)
	{
		out << fx[j] << endl;
	}
	//drukuj Ln(xpj)
	out << "\nLn(xpj):" << endl;
	for (int j = 0; j <= n; j++)
	{
		out << lnxp[j] << endl;
	}
	

	////////// zamknięcie pliku ///////////
	out.close();

	//zwolnienie pamięci
	delete[] xp;
	delete[] fx;
	delete[] lnxp;
}

int main()
{
	double a, b;
	int n, np;
	wprowadz_dane(a, b, n, np);
	wydrukuj_tablice(a, b, n, np);

	return 0;
}

