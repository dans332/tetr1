#include <iostream>
#include <thread>
#include <vector>
using namespace std;

#include <Windows.h>

wstring forma[7];
int nLaukoPlotis = 12;
int nLaukoAukstis = 18;
unsigned char *pLaukas = nullptr;

int nEkranoPlotis = 80;
int nEkranoAukstis = 30;

class PirmaForma
{
public:

};

int Pasukimas(int dx, int dy, int r)
{
    switch (r % 4)
    {
    case 0: return dy * 4 + dx;
    case 1: return 12 + dy - (dx * 4);
    case 2: return 15 - (dy * 4) - dx;
    case 3: return 3 - dy + (dx * 4);
    }
    return 0;
}

bool ArDetaleTelpa(int nForma, int nPasisukimas, int nPozX, int nPozY)
{
    //Jeigu laukas >0 tai ten yra detale
    for (int px = 0; px < 4; px++)
    {
        for (int py = 0; py < 4; py++)
        {
            //Indekso irasymas i detale
            int pi = Pasukimas(px, py, nPasisukimas);

            //Indekso irasymas i lauka
            int fi = (nPozY + py) * nLaukoPlotis + (nPozX + px);

            if (nPozX + px >= 0 && nPozX + px < nLaukoPlotis)
            {
                if (nPozY + py >= 0 && nPozY + py < nLaukoAukstis)
                {
                    if (forma[nForma][pi] == L'X' && pLaukas[fi] != 0)
                    {
                        return false;
                    }
                }
            }
        }
    }
    
    return true;
}

int main()
{
    forma[0].append(L"..X.");
    forma[0].append(L"..X.");
    forma[0].append(L"..X.");
    forma[0].append(L"..X.");

    forma[1].append(L"..X.");
    forma[1].append(L".XX.");
    forma[1].append(L".X..");
    forma[1].append(L"....");

    forma[2].append(L".X..");
    forma[2].append(L".XX.");
    forma[2].append(L"..X.");
    forma[2].append(L"....");

    forma[3].append(L"....");
    forma[3].append(L".XX.");
    forma[3].append(L".XX.");
    forma[3].append(L"....");

    forma[4].append(L"....");
    forma[4].append(L"..X.");
    forma[4].append(L".XX.");
    forma[4].append(L"..X.");

    forma[5].append(L"....");
    forma[5].append(L".XX.");
    forma[5].append(L"..X.");
    forma[5].append(L"..X.");

    forma[6].append(L"....");
    forma[6].append(L".XX.");
    forma[6].append(L".X..");
    forma[6].append(L".X..");

    //Sukuriamas zaidimo laukas
    pLaukas = new unsigned char[nLaukoPlotis * nLaukoAukstis];
    for (int x = 0; x < nLaukoPlotis; x++)
    {
        for (int y = 0; y < nLaukoAukstis; y++)
        {
            pLaukas[y * nLaukoPlotis + x] = (x == 0 || x == nLaukoPlotis - 1 || y == nLaukoAukstis - 1) ? 10 : 0;
        }
    }

    //Sukuriamas zaidimo ekranas
    wchar_t *ekranas = new wchar_t[nEkranoPlotis * nEkranoAukstis];
    for (int i = 0; i < nEkranoPlotis * nEkranoAukstis; i++)
    {
        ekranas[i] = L' ';
    }
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwIrasytiBaitai = 0;

    bool bZaidimoPabaiga = false;

    int nEsamaDetale = 0;
    int nEsamasPasukimas = 0;
    int nEsamasX = nLaukoPlotis / 2;
    int nEsamasY = 0;

    bool bMygtukas[4];
    bool bPasukimoUzlaikymas = false;

    int nGreitis = 20;
    int nGreicioSkaiciuokle = 0;
    bool bPriverstinisNuleidimas = false;
    int nDetaliuSkaiciuokle = 0;
    int nTaskai = 0;

    vector<int> vLinijos;

    //Zaidimo ciklas
    while (!bZaidimoPabaiga)
    {
        this_thread::sleep_for(50ms);
        nGreicioSkaiciuokle++;
        bPriverstinisNuleidimas = (nGreicioSkaiciuokle == nGreitis);

        //Mygtuku ivedimas
        for (int k = 0; k < 4; k++)
        {
                                                                       //D   K   A Z
            bMygtukas[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;
        }

        //Detales judinimas
        nEsamasX += (bMygtukas[0] && ArDetaleTelpa(nEsamaDetale, nEsamasPasukimas, nEsamasX + 1, nEsamasY)) ? 1 : 0;
        nEsamasX -= (bMygtukas[1] && ArDetaleTelpa(nEsamaDetale, nEsamasPasukimas, nEsamasX - 1, nEsamasY)) ? 1 : 0;
        nEsamasY += (bMygtukas[2] && ArDetaleTelpa(nEsamaDetale, nEsamasPasukimas, nEsamasX, nEsamasY + 1)) ? 1 : 0;

        //Kad vienu mygtuko paspaudimu vienas pasukimas butu
        if (bMygtukas[3])
        {
            nEsamasPasukimas += (!bPasukimoUzlaikymas && ArDetaleTelpa(nEsamaDetale, nEsamasPasukimas + 1, nEsamasX, nEsamasY)) ? 1 : 0;
            bPasukimoUzlaikymas = true;
        }
        else
        {
            bPasukimoUzlaikymas = false;
        }

        //Priversitinis detales nuleidimas zemyn
        if (bPriverstinisNuleidimas)
        {
            //Tikrinimas ar detale gali buti nuleista zemyn
            if (ArDetaleTelpa(nEsamaDetale, nEsamasPasukimas, nEsamasX, nEsamasY + 1))
            {
                nEsamasY++;
            }
            else
            {
                //Negali buti pajudinta zemyn, uzrakinti detales vieta
                for (int px = 0; px < 4; px++)
                {
                    for (int py = 0; py < 4; py++)
                    {
                        if (forma[nEsamaDetale][Pasukimas(px, py, nEsamasPasukimas)] == L'X')
                        {
                            pLaukas[(nEsamasY + py) * nLaukoPlotis + (nEsamasX + px)] = nEsamaDetale + 1;
                        }
                    }
                }

                //Pasunkinti lygi kas 10 detaliu
                nGreicioSkaiciuokle = 0;
                nDetaliuSkaiciuokle++;
                if (nDetaliuSkaiciuokle % 10 == 0)
                {
                    if (nGreitis >= 10)
                    {
                        nGreitis--;
                    }
                }

                for (int py = 0; py < 4; py++)
                {
                    if (nEsamasY + py < nLaukoAukstis - 1)
                    {
                        bool bLinija = true;
                        for (int px = 1; px < nLaukoPlotis - 1; px++)
                        {
                            bLinija &= (pLaukas[(nEsamasY + py) * nLaukoPlotis + px]) != 0;
                        }

                        if (bLinija)
                        {
                            for (int px = 1; px < nLaukoPlotis - 1; px++)
                            {
                                pLaukas[(nEsamasY + py) * nLaukoPlotis + px] = 8;
                            }

                            vLinijos.push_back(nEsamasY + py);
                        }
                    }
                }

                nTaskai += 25;
                if (!vLinijos.empty()) nTaskai += (1 << vLinijos.size()) * 100;

                nEsamasX = nLaukoPlotis / 2;
                nEsamasY = 0;
                nEsamasPasukimas = 0;
                nEsamaDetale = rand() % 7;

                bZaidimoPabaiga = !ArDetaleTelpa(nEsamaDetale, nEsamasPasukimas, nEsamasX, nEsamasY);
            }

            nGreicioSkaiciuokle = 0;
        }

        for (int x = 0; x < nLaukoPlotis; x++)
        {
            for (int y = 0; y < nLaukoAukstis; y++)
            {
                ekranas[(y + 2) * nEkranoPlotis + (x + 2)] = L" ABCDEFG=#"[pLaukas[y * nLaukoPlotis + x]];
            }
        }

        for (int px = 0; px < 4; px++)
        {
            for (int py = 0; py < 4; py++)
            {
                if (forma[nEsamaDetale][Pasukimas(px, py, nEsamasPasukimas)] == L'X')
                {
                    ekranas[(nEsamasY + py + 2) * nEkranoPlotis + (nEsamasX + px + 2)] = nEsamaDetale + 65;
                }
            }
        }

        //swprintf_s(&ekranas[2 * nEkranoPlotis + nLaukoPlotis + 6], 16, L"Taskai: %8d", nTaskai);

        if (!vLinijos.empty())
        {
            WriteConsoleOutputCharacter(hConsole, ekranas, nEkranoPlotis * nEkranoAukstis, { 0,0 }, &dwIrasytiBaitai);
            this_thread::sleep_for(400ms);

            for (auto& v : vLinijos)
            {
                for (int px = 1; px < nLaukoPlotis - 1; px++)
                {
                    for (int py = v; py > 0; py--)
                    {
                        pLaukas[py * nLaukoPlotis + px] = pLaukas[(py - 1) * nLaukoPlotis + px];
                    }
                    pLaukas[px] = 0;
                }
            }
            vLinijos.clear();
        }

        WriteConsoleOutputCharacter(hConsole, ekranas, nEkranoPlotis * nEkranoAukstis, { 0,0 }, &dwIrasytiBaitai);
    }

    CloseHandle(hConsole);
    cout << "Zaidimo pabaiga! Taskai:" << nTaskai << endl;
    system("pause");

    return 0;
}