#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <TAxis.h>
#include <TGraphErrors.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TMath.h>
#include "TStyle.h"

using namespace std;

//la parte più macchinosa del codice consiste nel definire i nomi dei file di input e i cicli per ciascuna sorgente, e visto che per il cobalto ad alto rateo sono state escluse le risoluzioni dal fit il terzo ciclo serve solo a stampare i risultati con variabili temporanee
#define n 13 //numero picchi totali a disposizione
#define in_name "../dati/input/input_complessivo.txt" //nome del file di input complessivo
#define out_1_name "../dati/output/risoluzioni_europio.tex"
#define out_2_name "../dati/output/risoluzioni_cobalto.tex" 
#define out_3_name "../dati/output/risoluzioni_cobalto_highrate.tex" 
#define out_4_name "../dati/output/fit_risoluzione.tex" //nome del file di output per i risultati del fit
#define out_pdf_name "../grafici/fit_risoluzione.pdf" //nome del file di output per il grafico del fit in pdf

#define A 0.323882 //valore della pendenza della retta di calibrazione
#define A_error 0.000018 //valore dell'incertezza sulla pendenza della retta di calibrazione

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

void fit_risoluzione()
    {
        //dichiarazione variabili e canvas
        TCanvas *c=new TCanvas("fit_caratteristico", "", 800, 600);
        Double_t R[n], R_error[n], energy[n], energy_error[n]={0}, res[n];
        Double_t FWHM, FWHM_error, temp;
        Int_t i;
        string dummy;

        //preparazione file, dichiarazione istogrammi
        fstream in(in_name,ios::in);
        fstream out_1(out_1_name,ios::out);
        fstream out_2(out_2_name,ios::out);
        fstream out_3(out_3_name,ios::out);
        fstream out_4(out_4_name,ios::out);
        out_1<<fixed;
        out_2<<fixed;
        out_3<<fixed;
        out_4<<fixed;
        
        //lettura e scrittura file
        out_1 << "\\begin{array}{ccc}\n\t\\toprule\n\t\\% & R & \\delta R \\\\\n\t\\midrule" <<endl;
        out_2 << "\\begin{array}{ccc}\n\t\\toprule\n\t\\% & R & \\delta R \\\\\n\t\\midrule" <<endl;
        out_3 << "\\begin{array}{ccc}\n\t\\toprule\n\t\\% & R & \\delta R \\\\\n\t\\midrule" <<endl;

        if(in.is_open())
            {
                getline(in,dummy);
                for(i=0;i<n-2;i++)
                    {
                        in>>temp>>temp>>FWHM>>FWHM_error>>temp>>temp>>temp>>temp>>energy[i]>>temp;
                        FWHM*=A;
                        FWHM_error*=A;
                        R[i]=(FWHM/energy[i]);
                        R_error[i]=(FWHM_error/energy[i]);
                        R[i]*=100;
                        R_error[i]*=100;
                        out_1 << "\t\\text{picco "
                        << i+1
                        << "} &"
                        << arrotonda(R[i], R_error[i]).Data()
                        << " \\\\" << endl;
                    }
                for(;i<n;i++)
                    {
                        in>>temp>>temp>>FWHM>>FWHM_error>>temp>>temp>>temp>>temp>>energy[i]>>temp;
                        FWHM*=A;
                        FWHM_error*=A;
                        R[i]=(FWHM/energy[i]);
                        R_error[i]=(FWHM_error/energy[i]);
                        R[i]*=100;
                        R_error[i]*=100;
                        out_2 << "\t\\text{picco "
                        << i+1-11
                        << "} &"
                        << arrotonda(R[i], R_error[i]).Data()
                        <<" \\\\" << endl;
                    }
                for(;i<n+2;i++)
                    {
                        in>>temp>>temp>>FWHM>>FWHM_error>>temp>>temp>>temp>>temp>>temp>>temp;
                        FWHM*=A;
                        FWHM_error*=A;
                        out_3 << "\t\\text{picco "
                        << i+1-13
                        << "} &"
                        << arrotonda((FWHM*100/energy[i-2]), (FWHM_error*100/energy[i-2])).Data()
                        << " \\\\" <<endl;
                    }
                out_1 << "\t\\bottomrule\n\\end{array}" <<endl;
                out_2 << "\t\\bottomrule\n\\end{array}" <<endl;
                out_3 << "\t\\bottomrule\n\\end{array}" <<endl;
            }
        in.close();
        out_1.close();
        out_2.close();
        out_3.close();

        //fit caratteristico e scrittura file di output
        TGraphErrors *g=new TGraphErrors(n, energy, R, energy_error, R_error);
        TF1 *gfit=new TF1("gfit","100*sqrt([0]/(x*x) + [1]/x + [2])", 0, 6000);
        gfit->SetParameter(0, 1.0);
        gfit->SetParameter(1, 0.01);
        gfit->SetLineColor(kGreen+4);
        g->Fit("gfit", "", "", 50, 6000);

        out_4 << "\\begin{array}{cccccccc}\n\ta & \\delta a & b & \\delta b & c & \\delta c & \\chi^2 & NDF \\\\\n\t\\midrule\n\t"
        << arrotonda(gfit->GetParameter(0), gfit->GetParError(0)).Data() <<
        " & "
        << arrotonda(gfit->GetParameter(1), gfit->GetParError(1)).Data() <<
        " & "
        << arrotonda(gfit->GetParameter(2), gfit->GetParError(2)).Data() <<
        " & "
        << setprecision(2) << gfit->GetChisquare() <<
        " & "
        << setprecision(0) << gfit->GetNDF()
        << " \\\\\n\t\\bottomrule\n\\end{array}" << endl;

        out_4.close();
        
        //salvataggio plot fit caratteristico
        g->Draw("AP");
        gStyle->SetOptFit(1111);
        g->SetTitle("Fit caratteristico");
        g->SetLineColor(kGreen-2);
        g->SetMarkerStyle(2);
        g->GetXaxis()->SetTitle("Energia (keV)");
        g->GetYaxis()->SetTitle("Risoluzione");
        c->SaveAs(out_pdf_name);
    }
