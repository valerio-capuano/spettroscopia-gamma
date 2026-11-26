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

#define n 2 //numero picchi
#define bin 8192 //numero bin
#define mode "canali"
#define SX 0 //estremo sinistro di visualizzazione
#define DX 8192 //estremo destro di visualizzazione
#define in_name "../dati/input/spettro_cobalto_baf.txt"
#define in_parameter_name "../dati/input/parametri_cobalto_baf.txt" 
#define out_name "../dati/output/fit_picchi_cobalto_baf.tex" 
#define out_pdf_spectrum_name "../grafici/spettro_cobalto_baf.pdf" 

//funzione che arrotonda per avere sempre incertezza con due cifre significative
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

void spettro_coincidenza()
    {
        //dichiarazione variabili
        string dummy;
        Int_t i;
        Double_t ch,err;
        Double_t due_rad_due_log_due = 2*TMath::Sqrt(2*TMath::Log(2));
        Double_t mean[n], sigma[n], N[n], FWHM[n], mean_error[n], sigma_error[n], N_error[n], FWHM_error[n];
        Double_t sx[n], dx[n], par_0[n], par_2[n], par_1[n];

        //dichiarazione canvas
        TCanvas *cspectrum=new TCanvas("c", "Spettro", 800, 600);
        gStyle->SetOptStat(0); 
        gStyle->SetOptFit(100);

        //dichiarazione istogrammi
        TH1I *hspectrum=new TH1I("h", mode, bin, 0, bin);
        TH1I *hpeak;
        TF1 *hfit;

        //lettura spettro
        fstream in(in_name, ios::in);
        if(in.is_open())
            {
                for(i=0; i<12; i++)
                    {
                        getline(in, dummy);
                    }
                
                for(i=0; i<bin; i++)
                    {
                        in>>ch;
                        //in>>err;
                        hspectrum->SetBinContent(i+1, ch);
                        //hspectrum->SetBinError(i+1, err);
                    }
                in.close();
            }

        //fit gaussiana doppia
        fstream in_parameter(in_parameter_name, ios::in);
        fstream out(out_name, ios::out);
        out<<fixed;
        getline(in_parameter, dummy);
        if(out.is_open() && in_parameter.is_open())
            {
                out << "\\begin{array}{ccccccccc}\n\t\\toprule\n\t(canali) & N & \\delta N & X & \\delta X & \\sigma & \\delta\\sigma & FWHM & \\delta FWHM  \\\\\n\t\\midrule" << endl;
                for(i=0; i<n; i++)
                    {
                        in_parameter>>sx[i]>>dx[i]>>par_0[i]>>par_1[i]>>par_2[i];
                    }
                hfit = new TF1(Form("f%d", i), "[0]/(sqrt(TMath::TwoPi())*[2])*exp(-0.5*pow((x-[1])/[2], 2))+[3]+[4]*x + [5]/(sqrt(TMath::TwoPi())*[7])*exp(-0.5*pow((x-[6])/[7], 2))+[8]+[9]*x", sx[0], dx[1]);
                hfit->SetLineColor(kGreen+4);
                hfit->SetParameter(1, par_1[0]);
                hfit->SetParameter(2, par_2[0]);
                hfit->SetParameter(3, 0);
                hfit->SetParameter(4, 0);
                hfit->SetParameter(6, par_1[0]);
                hfit->SetParameter(7, par_2[0]);
                hfit->SetParameter(8, 0);
                hfit->SetParameter(9, 0);
                hspectrum->Fit(hfit, "R+Q", "I", sx[0], dx[1]);
                N[0] = hfit->GetParameter(0);
                mean[0] = hfit->GetParameter(1);
                sigma[0] = hfit->GetParameter(2);
                N_error[0] = hfit->GetParError(0);
                mean_error[0] = hfit->GetParError(1);
                sigma_error[0] = hfit->GetParError(2);
                FWHM[0] = due_rad_due_log_due*sigma[0];
                FWHM_error[0] = due_rad_due_log_due*sigma_error[0];
                N[1] = hfit->GetParameter(5);
                mean[1] = hfit->GetParameter(6);
                sigma[1] = hfit->GetParameter(7);
                N_error[1] = hfit->GetParError(5);
                mean_error[1] = hfit->GetParError(6);
                sigma_error[1] = hfit->GetParError(7);
                FWHM[1] = due_rad_due_log_due*sigma[1];
                FWHM_error[1] = due_rad_due_log_due*sigma_error[1];

                out << "\t\\text{picco 1} & " 
                    << setprecision(0) << N[0] << " & " << N_error[0] << " & "
                    << arrotonda(mean[0], mean_error[0]).Data()
                    << " & "
                    << arrotonda(sigma[0], sigma_error[0]).Data()
                    << " & "
                    << arrotonda(FWHM[0], FWHM_error[0]).Data()
                    << " \\\\" << endl;

                out << "\t\\text{picco 1} & " 
                    << setprecision(0) << N[1] << " & " << N_error[1] << " & "
                    << arrotonda(mean[1], mean_error[1]).Data()
                    << " & "
                    << arrotonda(sigma[1], sigma_error[1]).Data()
                    << " & "
                    << arrotonda(FWHM[1], FWHM_error[1]).Data()
                    << " \\\\" << endl;
            
                out << "\t\\bottomrule\n\\end{array}";
            }
            out.close();
            in_parameter.close();

            //salvataggio canvas
            cspectrum->cd();
            hspectrum->SetLineColor(kGreen-2);
            hspectrum->SetTitle("Spettro in coincidenza");
            hspectrum->GetXaxis()->SetTitle(mode);
            hspectrum->GetYaxis()->SetTitle("conteggi");
            hspectrum->GetXaxis()->SetRangeUser(sx[0] - 50, dx[1] + 50);
            hspectrum->Draw("HIST");
            hfit->Draw("SAME");
            cspectrum->SaveAs(out_pdf_spectrum_name);
    }
