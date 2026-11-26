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

#define n 13 //numeri picchi totali a disposizione
#define grado 1 //grado polinomio di calibrazione
#define err_nl 0 //errore sistematico sul centroide dovuto alla non perfetta linearità dell'ADC
#define in_name "../dati/input/input_complessivo.txt"  //nome del file di input complessivo
#define out_name "../dati/output/fit_calibrazione.tex" //nome del file di output per i risultati del fit
#define out_1_pdf_name "../grafici/fit_calibrazione.pdf"  //nome del file di output per il plot dle fit in pdf
#define out_2_pdf_name "../grafici/fit_calibrazione_residui.pdf" //nome del file di output per il plot dei residui in pdf

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

void fit_calibrazione()
    {
        //dichiarazioni canvas e variabili
        string greche[] = {"\\alpha", "\\beta", "\\gamma", "\\delta", "\\epsilon", "\\zeta", "\\eta"};
        TCanvas *c_1=new TCanvas("fit_polinomiale", "", 800, 600);
        TCanvas *c_2=new TCanvas("andamento_residui", "", 800, 600);
        Double_t energy[n], energy_error[n]={0}, mean[n], mean_error[n], res[n];
        Double_t temp;
        Int_t i;
        string dummy;

        //riempimento vettori con lettura file di input
        fstream in(in_name,ios::in);
        if(in.is_open())
            {
                getline(in,dummy);
                for(Int_t i=0;i<n;i++)
                    {
                        in>>mean[i]>>mean_error[i]>>temp>>temp>>temp>>temp>>temp>>temp>>energy[i]>>temp;
                        mean_error[i]=TMath::Sqrt((mean_error[i]*mean_error[i])+(err_nl*err_nl));
                    }
                in.close();
            }

        //creazione stringa per numero colonne tabella (dipendono dal grado del fit)
        TString colonne_tabella="cc"; 
        for(i=0; i<=grado; i++)
            {
                colonne_tabella += "cc";
            }

        //fit polinomiale con scrittura file
        TGraphErrors *g_1=new TGraphErrors(n, mean, energy, mean_error, energy_error);
        TF1 *gfit=new TF1("gfit", Form("pol%d", grado), 0, 6000);
        for(i=0; i<=grado; i++)
            {
                gfit->SetParameter(i, 1);
            }
        gfit->SetLineColor(kGreen+4);
        g_1->Fit("gfit","","",0,6000);
        fstream out(out_name,ios::out);
        
        out<<fixed;

        out << "\\begin{array}{" 
        << colonne_tabella
        << "}\n\t\\toprule\n\t";

        for(i=0; i<=grado; i++)
            {
                if(grado >= 7)
                    {
                        cout << "Grado troppo alto: non bastano le lettere greche!" << endl;
                        break;
                    }
                out << greche[i]
                << " & "
                << "\\delta"
                << greche[i]
                << " & ";
            }
        out << "\\chi^2 & NDF \\\\\n\t\\midrule\n\t";

        for(i=0; i<=grado; i++)
            {
                out << arrotonda(gfit->GetParameter(i), gfit->GetParError(i)).Data()
                << " & ";
            }

        out << setprecision(2) << gfit->GetChisquare()
        << " & "
        << setprecision(0) << gfit->GetNDF()
        << " \\\\\n\t\\bottomrule\n\\end{array}" << endl;

        out.close();

        //riempimento vettore residui
        for(i=0; i<n; i++)
            {
                temp=gfit->Eval(mean[i]);
                res[i]=energy[i]-temp;
            }
        TGraph *g_2=new TGraph(n, energy, res);
        
        //salvataggio plot fit polinomiale
        c_1->cd();
        g_1->Draw("AP");
        gStyle->SetOptFit(1111);
        g_1->SetTitle(Form("Fit polinomio grado %d", grado));
        g_1->SetLineColor(kGreen-2);
        g_1->SetMarkerStyle(2);
        g_1->GetXaxis()->SetTitle("Canale");
        g_1->GetYaxis()->SetTitle("Energia (keV)");
        c_1->SaveAs(out_1_pdf_name);

        //salvataggio plot residui
        c_2->cd();
        g_2->Draw("AP");
        g_2->SetTitle(Form("Residui fit polinomio grado %d", grado));
        g_2->SetLineColor(kGreen-2);
        g_2->SetMarkerStyle(2);
        g_2->GetXaxis()->SetTitle("Energia (keV)");
        g_2->GetYaxis()->SetTitle("Residuo (keV)");
        c_2->SaveAs(out_2_pdf_name);
    }
