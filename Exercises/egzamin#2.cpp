#include <iostream>
#include <stdlib.h>

using namespace std;

//Zadanie 1
class Element
{
public:
    Element(unsigned int num = 0): numer(num)
    {
    }
    unsigned int podajNumer() const
    {
        return numer;
    }
    void wstawNumer(unsigned int num)
    {
        numer = num;
    }
public:
    unsigned int numer;
};

class Silnik : public Element
{
public:
    Silnik(unsigned int num = 0, double poj = 0):Element(num), pojemnosc(poj)
    {
    }
    double pojemnosc;
};

enum KOLOR
{
    CZARNY,
    NIEBIESKI,
    ZIELONY,
    CZERWONY,
    BIALY
};

class Nadwozie : public Element
{
public:
    Nadwozie(unsigned int num = 0, KOLOR kol = CZARNY):Element(num), kolor(kol)
    {
    }
    KOLOR kolor;
};

class SrodekTransportu
{
public:
    SrodekTransportu(unsigned int num = 0):numerEwid(num)
    {
    }
    unsigned int numerEwid;
};

class Pojazd : public SrodekTransportu
{
public:
    Pojazd(unsigned int num = 0, unsigned int numSil = 0, unsigned int numNad = 0, unsigned int poj = 0, KOLOR kol = CZARNY)
    :SrodekTransportu(num), silnik(numSil, poj), nadwozie(numNad, kol)
    {
    }
    Silnik silnik;
    Nadwozie nadwozie;
};

//Zadanie 2
enum MATERIAL
{
    DREWNIANA,
    METALOWA,
    KOMPOZYTOWA
};

class Lufa
{
public:
    Lufa(unsigned int num = 0, double kal = 0):kaliber(kal)
    {
    }
    double kaliber;
};

class Magazynek
{
public:
    Magazynek(unsigned int num = 0, unsigned int nab = 0):naboje(nab)
    {
    }
    unsigned int naboje;
};

class Kolba
{
public:
    Kolba(unsigned int num = 0, MATERIAL mat = DREWNIANA):material(mat)
    {
    }
    MATERIAL material;
};

class BronPalna
{
public:
    BronPalna(unsigned int num = 0, unsigned int numMag = 0, unsigned int numLufa = 0):magazynek(numMag), lufa(numLufa)
    {
    }
    Magazynek magazynek;
    Lufa lufa;
};

class KarabinekAutomatyczny : public BronPalna
{
public:
    KarabinekAutomatyczny(unsigned int num = 0, unsigned int kolb = 0, MATERIAL mat = DREWNIANA,
                          unsigned int numMag = 0, unsigned int numLufa = 0)
    :BronPalna(num, numMag, numLufa), kolba(kolb, mat)
    {
    }
    Kolba kolba;
};

class Pistolet : public BronPalna
{
public:
    Pistolet(unsigned int num = 0, unsigned int numMag = 0, unsigned int numLufa = 0):BronPalna(num, numMag, numLufa)
    {
    }
};

//Zadanie 3
class Jacht
{
public:
    Jacht(unsigned int lZagli):liczbaZagli(lZagli = 0)
    {
    }
private:
    unsigned int liczbaZagli;
    unsigned int pobierzZagle()
    {
        return liczbaZagli;
    }
    void ustawZagle(unsigned int lZagli)
    {
        liczbaZagli = lZagli;
    }
    void pokazInfo()
    {
        cout << liczbaZagli << endl;
    }
};

class JachtMotorowy
{
public:
    JachtMotorowy(unsigned int pojSilnika):pojemnoscSilnika(pojSilnika = 0)
    {
    }
private:
    double pojemnoscSilnika;
    double pobierzPojemnosc()
    {
        return pojemnoscSilnika;
    }
    void ustawPojemnosc(double pojSilnika)
    {
        pojemnoscSilnika = pojSilnika;
    }
    void pokazInfo()
    {
        cout << pojemnoscSilnika << endl;
    }
};
