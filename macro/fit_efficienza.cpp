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

//la parte più macchinosa del codice consiste nel definire i nomi dei file di input e i cicli per ciascuna sorgente, e visto che per il cobalto ad alto rateo sono state escluse le efficienze dal fit il terzo ciclo serve solo a stampare i risultati con variabili temporanee
#define n 15 //numero picchi totali a disposizione (anche quelli per la sorgente highrate, che però non andranno nel fit)
#define grado 2 //grado funzione di fit
#define in_name "../dati/input/input_complessivo.txt" //nome del file di input complessivo
#define out_1_name "../dati/output/efficienze_europio.tex" 
#define out_2_name "../dati/output/efficienze_cobalto.tex" 
#define out_3_name "../dati/output/efficienze_cobalto_highrate.tex" 
#define out_4_name "../dati/output/fit_efficienza.tex" //nome del file di output per i risultati del fit
#define out_1_pdf_name "../grafici/fit_efficienza_2.pdf" //nome del file di output per il plot del fit caratteristico in pdf
#define out_2_pdf_name "../grafici/fit_efficienza_residui_2.pdf" //nome del file di output per il plot dei residui in pdf

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

void fit_efficienza()
    {
        //dichiarazione variabili e canvas
        TCanvas *c_1=new TCanvas("fit_caratteristico", "", 800, 600);
        TCanvas *c_2=new TCanvas("andamento_residui", "", 800, 600);
        Double_t epsilon[n-2], epsilon_error[n-2], energy[n-2], energy_error[n-2]={0}, res[n-2];
        Double_t epsilon_temp, epsilon_error_temp;
        Double_t A,T,BR,N,N_error;
        Double_t temp;
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
        out_1 << setprecision(3) << "\\begin{array}{ccc}\n\t\\toprule\n\t\\% & \\epsilon & \\delta\\epsilon \\\\\n\t\\midrule" <<endl;
        out_2 << setprecision(3) << "\\begin{array}{ccc}\n\t\\toprule\n\t\\% & \\epsilon & \\delta\\epsilon \\\\\n\t\\midrule" <<endl;
        out_3 << setprecision(3) << "\\begin{array}{ccc}\n\t\\toprule\n\t\\% & \\epsilon & \\delta\\epsilon \\\\\n\t\\midrule" <<endl;

        if(in.is_open())
            {
                getline(in,dummy);
                for(i=0;i<n-4;i++)
                    {
                        in>>temp>>temp>>temp>>temp>>N>>N_error>>A>>T>>energy[i]>>BR;
                        BR/=100;
                        epsilon[i]=100*(N/(BR*A*T));
                        epsilon_error[i]=epsilon[i]*TMath::Sqrt(((N_error/N)*(N_error/N))+((1/T)*(1/T))+(0.05*0.05));

                        out_1 << "\t\\text{picco "
                        << i+1
                        << "} &"
                        << arrotonda(epsilon[i], epsilon_error[i]).Data()
                        << " \\\\" << endl;
                    }
                for(;i<n-2;i++)
                    {
                        in>>temp>>temp>>temp>>temp>>N>>N_error>>A>>T>>energy[i]>>BR;
                        BR/=100;
                        epsilon[i]=100*(N/(BR*A*T));
                        epsilon_error[i]=epsilon[i]*TMath::Sqrt(((N_error/N)*(N_error/N))+((1/T)*(1/T))+(0.05*0.05));

                        out_2 << "\t\\text{picco "
                        << i-8
                        << "} &"
                        << arrotonda(epsilon[i], epsilon_error[i]).Data()
                        << " \\\\" << endl;
                    }
                for(;i<n;i++)
                    {
                        in>>temp>>temp>>temp>>temp>>N>>N_error>>A>>T>>temp>>BR;
                        BR/=100;
                        epsilon_temp=100*(N/(BR*A*T));
                        epsilon_error_temp=epsilon_temp*TMath::Sqrt(((N_error/N)*(N_error/N))+((1/T)*(1/T))+(0.05*0.05));

                        out_3 << "\t\\text{picco "
                        << i-10
                        << "} &"
                        << arrotonda(epsilon_temp, epsilon_error_temp).Data()
                        << " \\\\" << endl;
                    }
                out_1 << "\t\\bottomrule\n\\end{array}" <<endl;
                out_2 << "\t\\bottomrule\n\\end{array}" <<endl;
                out_3 << "\t\\bottomrule\n\\end{array}" <<endl;
            }
        in.close();
        out_1.close();
        out_2.close();
        out_3.close();

        //creazione stringa per funzione di fit
        TString formula="100*exp([0]+[1]*log(x)"; 
        if(grado>1)
            {
                for(i=2; i<=grado; i++)
                    {
                        formula += Form("+[%d]*pow(log(x), %d)", i, i);
                    }
            }
        formula += ")"; 

        //creazione stringa per numero colonne tabella (dipendono dal grado del fit)
        TString colonne_tabella="cc"; 
        for(i=0; i<=grado; i++)
            {
                colonne_tabella += "cc";
            }

        //fit caratteristico e scrittura file di output di fit
        TGraphErrors *g_1=new TGraphErrors(n-2, energy, epsilon, energy_error, epsilon_error);
        TF1 *gfit=new TF1("gfit", formula, 0, 6000);
        for(i=0; i<grado; i++)
            {
                gfit->SetParameter(i, 0.1);
            }
        gfit->SetLineColor(kGreen+4);
        g_1->Fit("gfit", "", "", 50, 6000);

        out_4 << "\\begin{array}{" 
        << colonne_tabella
        << "}\n\t\\toprule\n\t";

        for(i=0; i<=grado; i++)
            {
                out_4 << "\\rho_"
                << i
                << " & "
                << " \\delta\\rho_"<<i<<" & ";
            }
        out_4<<"\\chi^2 & NDF \\\\\n\t\\midrule\n\t";

        for(i=0; i<=grado; i++)
            {
                out_4 << arrotonda(gfit->GetParameter(i), gfit->GetParError(i)).Data()
                << " & ";
            }

        out_4<<setprecision(2)<<gfit->GetChisquare()<<" & "<<setprecision(0)<<gfit->GetNDF()<<" \\\\\n\t\\bottomrule\n\\end{array}"<<endl;

        out_4.close();

        //riempimento vettore residui
        for(i=0; i<n-2; i++)
            {
                temp=gfit->Eval(energy[i]);
                res[i]=epsilon[i]-temp;
            }
        TGraph *g_2=new TGraph(n-2, epsilon, res);
        
        //salvataggio plot fit lineare
        c_1->cd();
        g_1->Draw("AP");
        gStyle->SetOptFit(1111);
        g_1->SetTitle("Fit caratteristico");
        g_1->SetLineColor(kGreen-2);
        g_1->SetMarkerStyle(2);
        g_1->GetXaxis()->SetTitle("Energia (keV)");
        g_1->GetYaxis()->SetTitle("Efficienza");
        c_1->SaveAs(out_1_pdf_name);

        //salvataggio plot residui
        c_2->cd();
        g_2->Draw("AP");
        g_2->SetTitle(Form("Residui fit funzione grado %d", grado));
        g_2->SetLineColor(kGreen-2);
        g_2->SetMarkerStyle(2);
        g_2->GetXaxis()->SetTitle("Energia (keV)");
        g_2->GetYaxis()->SetTitle("Residuo (keV)");
        c_2->SaveAs(out_2_pdf_name);
    }
