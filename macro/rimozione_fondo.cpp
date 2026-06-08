#include <iostream>
#include <fstream>
#include <string>
#include <TF1.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TMath.h>

using namespace std;

#define bin 8192 //numero di bin
#define in_1_name "../dati/input/spettro_incognita_germanio.txt" //nome file di input per spettro della sorgente
#define in_2_name "../dati/input/spettro_fondo_germanio.txt" //nome file di input per spettro del fondo
#define out_name "../dati/output/spettro_incognita_germanio_senza_fondo.txt" //nome per il file di testo di output per lo spettro della sorgente col fondo sottratto

void rimozione_fondo()
    {
        //dichiarazioni variabili
        string dummy;
        Double_t temp_1,temp_2,time_1,time_2,k,ch,err;
        Int_t i;

        //preparazione file
        fstream in_1(in_1_name,ios::in);
        fstream in_2(in_2_name,ios::in);
        fstream out(out_name,ios::out);

        if(in_1.is_open() && in_2.is_open())
            {
                for(i=0;i<9;i++)
                    {
                        getline(in_1,dummy);
                        getline(in_2,dummy);
                        out<<dummy<<endl;
                    }
                in_1>>time_1>>temp_1;
                out<<time_1<<"\t"<<temp_1<<endl;
                in_2>>time_2>>temp_2;
                k=(time_1/time_2);
                getline(in_1,dummy);
                getline(in_2,dummy);
                for(i=0;i<2;i++)
                    {
                        getline(in_1,dummy);
                        getline(in_2,dummy);
                        out<<dummy<<endl;
                        cout<<dummy<<endl;
                    }

                for(i=0;i<bin;i++)
                    {
                        in_1>>temp_1;
                        in_2>>temp_2;
                        ch=(temp_1-(temp_2*k));
                        err=TMath::Sqrt(temp_1+(temp_2*k*k));
                        out<<"\t"<<ch<<"\t"<<err<<endl;
                    }
                cout<<time_1<<" "<<time_2<<endl; //controllo acquisizione tempi
                in_1.close();
                in_2.close();
                out.close();
            }
    }
