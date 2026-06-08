#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <TF1.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TMath.h>
#include <TH1I.h>
#include "TStyle.h"

using namespace std;

#define n 40
#define in_name "../dati/output/fit_picchi_incognita_germanio.tex"
#define out_name "../dati/output/energie_incognita_germanio.tex"

#define A 0.3248607 //valore della pendenza della retta di calibrazione
#define A_error 0.0000027 //valore dell'incertezza sulla pendenza della retta di calibrazione
#define B 14.5468
#define B_error 0.0038

//funzione che arrotonda per avere incertezza con due cifre significative
TString arrotonda(Double_t val, Double_t err)
    {
        if (err <= 0) return Form(" %f & %f ", val, err);
        Double_t copy_err = 0;
        Int_t ordine, cifra, i;
        for(i=0; (int)copy_err==0; i++)
            {
                copy_err=err;
                copy_err*=(TMath::Power(10.0, i+1));
            }
        cifra=(i+1);
        ordine=TMath::Power(10.0, i+1);
        val=((int)((ordine*val)+0.5))/((double)ordine);
        err=((int)((ordine*err)+0.5))/((double)ordine);
        return Form(" %.*f & %.*f ", cifra, val, cifra, err);
    }

void conversione_energia()
    {
        //dichiarazione variabili
        string dummy;
        Double_t temp;
        Double_t mean_ch, mean_error_ch, FWHM_ch, FWHM_error_ch;
        Double_t mean_en, mean_error_en, FWHM_en, FWHM_error_en;

        //conversione risultati fit in energie
        fstream out(out_name, ios::out);
        out<<fixed;
        out << "\\begin{array}{ccccc}\n\t\\toprule\n\t(keV) & E & \\delta E & FWHM & \\delta FWHM \\\\\n\t\\midrule" << endl;
        fstream in(in_name, ios::in);
        if(in.is_open())
            {
                getline(in, dummy);
                getline(in, dummy);
                getline(in, dummy);
                getline(in, dummy);
                for(Int_t i=0;i<n;i++)
                    {
                        in>>dummy>>dummy>>dummy>>temp>>dummy>>temp>>dummy>>mean_ch>>dummy>>mean_error_ch>>dummy>>temp>>dummy>>temp>>dummy>>FWHM_ch>>dummy>>FWHM_error_ch>>dummy>>temp>>dummy>>temp>>dummy;
                        mean_en=((A*mean_ch)+B);
                        mean_error_en=TMath::Sqrt((mean_ch*mean_ch*A_error*A_error)+(B_error*B_error)+(A*A*mean_error_ch*mean_error_ch));
                        FWHM_en=(A*FWHM_ch);
                        FWHM_error_en=TMath::Sqrt((FWHM_ch*FWHM_ch*A_error*A_error)+(A*A*FWHM_error_ch*FWHM_error_ch));
                        out << setprecision(2) << "\t"
                        << "\\text{picco "
                        << i+1
                        << "} & "
                        << arrotonda(mean_en, mean_error_en)
                        << " & "
                        << arrotonda(FWHM_en, FWHM_error_en)
                        << " \\\\ " << endl;
                    }
                out << "\t\\bottomrule\n\\end{array}";
                in.close();
                out.close();
            }
    }
