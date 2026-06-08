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

#define n 40 //numero picchi
#define bin 8192 //numero bin
#define mode "canali"
#define SX 0 //estremo sinistro di visualizzazione
#define DX 8192 //estremo destro di visualizzazione
#define in_name "../dati/input/spettro_incognita_germanio.txt"
#define in_parameter_name "../dati/input/parametri_incognita_germanio.txt" 
#define out_name "../dati/output/fit_picchi_incognita_germanio.tex" 
#define out_pdf_spectrum_name "../grafici/spettro_incognita_germanio.pdf" 
#define out_pdf_peaks_name "../grafici/picco_incognita_germanio_%d.pdf"

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

void spettro()
    {
        //dichiarazione variabili
        string dummy;
        Int_t i;
        Double_t ch,err;
        Double_t due_rad_due_log_due = 2*TMath::Sqrt(2*TMath::Log(2));
        Double_t mean, sigma, N, FWHM, mean_error, sigma_error, N_error, FWHM_error;
        Double_t sx, dx, par_0, par_2, par_1;

        //dichiarazione canvas
        TCanvas *cspectrum=new TCanvas("c", "Spettro", 800, 600);
        TCanvas *cpeaks;
        gStyle->SetOptStat(0); 
        gStyle->SetOptFit(1);

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

        //fit iterati con scrittura progressiva del file
        fstream in_parameter(in_parameter_name, ios::in);
        fstream out(out_name, ios::out);
        out<<fixed;
        getline(in_parameter, dummy);
        if(out.is_open() && in_parameter.is_open())
            {
                out << "\\begin{array}{ccccccccccc}\n\t\\toprule\n\t(canali) & N & \\delta N & X & \\delta X & \\sigma & \\delta\\sigma & FWHM & \\delta FWHM & \\chi^2 & NDF \\\\\n\t\\midrule" << endl;
                for(i=0; i<n; i++)
                    {
                        in_parameter>>sx>>dx>>par_0>>par_1>>par_2;
                        hfit = new TF1(Form("f%d", i), "[0]/(sqrt(TMath::TwoPi())*[2])*exp(-0.5*pow((x-[1])/[2], 2))+[3]+[4]*x", sx, dx);
                        hfit->SetLineColor(kGreen+4);
                        hfit->SetParameter(1, par_1);
                        hfit->SetParameter(2, par_2);
                        hfit->SetParameter(3, 0);
                        hfit->SetParameter(4, 0);
                        TH1I *hclone = (TH1I*)hspectrum->Clone(Form("hclone_%d", i));
                        hclone->Fit(hfit, "R+Q0", "I", sx, dx);
                        N = hfit->GetParameter(0);
                        mean = hfit->GetParameter(1);
                        sigma = hfit->GetParameter(2);
                        N_error = hfit->GetParError(0);
                        mean_error = hfit->GetParError(1);
                        sigma_error = hfit->GetParError(2);
                        FWHM = due_rad_due_log_due*sigma;
                        FWHM_error = due_rad_due_log_due*sigma_error;

                        out << "\t\\text{picco " << i+1 << "} & " 
                            << setprecision(0) << N << " & " << N_error << " & "
                            << arrotonda(mean, mean_error).Data()
                            << " & "
                            << arrotonda(sigma, sigma_error).Data()
                            << " & "
                            << arrotonda(FWHM, FWHM_error).Data()
                            << " & "
                            << setprecision(2) << hfit->GetChisquare()
                            << " & " 
                            << setprecision(0) << hfit->GetNDF() 
                            << " \\\\" << endl;

                        cpeaks = new TCanvas(Form("cpeaks_%d", i), Form("Dettaglio Picco %d", i+1), 800, 600);
                        
                        hclone->GetXaxis()->SetRangeUser(sx-20, dx+20); 
                        hclone->SetTitle(Form("picco %d", i+1));
                        hclone->GetXaxis()->SetTitle(mode);
                        hclone->GetYaxis()->SetTitle("conteggi");
                        hclone->Draw("HIST");
                        hfit->Draw("same"); 
                        cpeaks->Update();
                        cpeaks->SaveAs(Form(out_pdf_peaks_name, i+1));
                    }
                out << "\t\\bottomrule\n\\end{array}";
            }
            out.close();
            in_parameter.close();

            //salvataggio canvas
            cspectrum->cd();
            hspectrum->SetLineColor(kGreen-2);
            hspectrum->SetTitle("spettro");
            hspectrum->GetXaxis()->SetTitle(mode);
            hspectrum->GetYaxis()->SetTitle("conteggi");
            hspectrum->GetXaxis()->SetRangeUser(SX, DX);
            hspectrum->Draw("HIST");
            cspectrum->SaveAs(out_pdf_spectrum_name);
    }
