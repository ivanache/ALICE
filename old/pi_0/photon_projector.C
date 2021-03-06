// This macro is designed to make lambda vs E projections from the total photon output of photon_analyzer
// Programmer: Ivan Chernyshev; Date Started :June 26, 2017

#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include <TCanvas.h>
#include <iostream>
#include "atlasstyle-00-03-05/AtlasStyle.h"
#include "atlasstyle-00-03-05/AtlasStyle.C"
#include "atlasstyle-00-03-05/AtlasUtils.h"
#include "atlasstyle-00-03-05/AtlasUtils.C"
#include "atlasstyle-00-03-05/AtlasLabels.h"
#include "atlasstyle-00-03-05/AtlasLabels.C"

// Concatenates two strings and gives a char array
char* str_concat_converter(string str1, string str2){
    string sumstring = str1 + str2;
    char* output = new char[sumstring.length() + 1];
    strcpy(output, sumstring.c_str());
    return output;
}

/**
 Main function
 */
// Precondition: NumOfSigmasFromMeanMax = 1, 2, 3, or 4
void photon_projector(string NumOfSigmasFromMeanMax) {
    const double lambdamin = 0;
    const double lambdamax = 2;
    const int axis_pionLambda1 = 9;
    const int y_min = 8;
    const int y_max = 10;
    const string directory_name = Form("%ssigma/projections/", NumOfSigmasFromMeanMax.c_str());
    TCanvas* canvas = new TCanvas();
    
    // Set ATLAS Style
    //gROOT->LoadMacro("AtlasStyle.C");
    //SetAtlasStyle();
    
    // Open the root file, get the data
    TFile* fIn = new TFile(Form("%ssigmaPhotonsOutput.root", NumOfSigmasFromMeanMax.c_str()), "READ");
    fIn->Print();
    TH2D* hist = 0;
    fIn->GetObject("total_Evslambda", hist);
    int yminbin = hist->GetYaxis()->FindBin(y_min);
    int ymaxbin = hist->GetYaxis()->FindBin(y_max);
    
    // Create the TH1D object, using the lambda vs entries graph from the THnSparses as a template
    TFile* TemplateSource = new TFile("PionSparsesOutput_angle_17mrad.root", "READ");
    THnSparse* temp = 0;
    TemplateSource->GetObject("h_Pion", temp);
    TH1D* projection = temp->Projection(axis_pionLambda1);
    // Copy the energy data from the E vs lambda histogram, integrate it over Y, put it into the corresponding bin of the projection
    int maxbin = projection->GetXaxis()->FindBin(lambdamax);
    double actual_bin;
    
    for (int i = projection->GetXaxis()->FindBin(lambdamin); i <= maxbin; i++) {
        actual_bin = hist->Integral(i, i, yminbin, ymaxbin);
        projection->SetBinContent(i, actual_bin);
        projection->SetBinError(i, TMath::Sqrt(projection->GetBinContent(i)));
    }
    // Before graphing, create an object to write to the root file being read from
    TFile* fOut = new TFile(Form("%ssigmaPhotonsOutput.root", NumOfSigmasFromMeanMax.c_str()), "UPDATE");

    // Graph and update the root file
    projection->SetTitle("Total Pi0 Photon Lambda vs. Total Pi0 Photon Energy Projection; lambda0; Number of Total Photons with energy 2-16 GeV");
    projection->GetXaxis()->SetTitleOffset(1.0);
    projection->GetYaxis()->SetTitleOffset(1.4);
    projection->Draw();
    myText(.03,.97, kBlack, "Total Pi0 Photon Lambda vs. Total Pi0 Photon Energy Projection, Pt 6-20 GeV");
    myText(.35,.92, kBlack, Form("mass within %s sigma of the mean", NumOfSigmasFromMeanMax.c_str()));
    projection->Write("Lambda_projection");
    canvas->SaveAs(str_concat_converter(directory_name, "Lambda_vs_E_projection.png"));
    
    
    // Repeat for all intervals of momentum
    const int num_of_intervals = 6;
    double intervals[num_of_intervals][2] = {{6.0, 8.0}, {8.0, 10.0}, {10.0, 12.0}, {12.0, 14.0}, {14.0, 16.0}, {16.0, 20.0}};
    for(int i = 0; i < num_of_intervals; i++) {
        double ptmin = intervals[i][0];
        double ptmax = intervals[i][1];
        canvas->Clear();
        
        // Open the root file, get the data
        fIn->GetObject(Form("total_Evslambda_ptmin_%2.2fGeV_ptmax_%2.2fGeV", ptmin, ptmax), hist);
        yminbin = hist->GetYaxis()->FindBin(y_min);
        ymaxbin = hist->GetYaxis()->FindBin(y_max);
        
        // Copy the energy data from the E vs lambda histogram, integrate it over Y, put it into the corresponding bin of the projection
        maxbin = projection->GetXaxis()->FindBin(lambdamax);
        for (int i = projection->GetXaxis()->FindBin(lambdamin); i <= maxbin; i++) {
            actual_bin = hist->Integral(i, i, yminbin, ymaxbin);
            projection->SetBinContent(i, actual_bin);
            projection->SetBinError(i, TMath::Sqrt(projection->GetBinContent(i)));
        }
        // Graph
        projection->SetTitle(Form("Total Pi0 Photon Lambda vs. Total Pi0 Photon Energy Projection (Momentum %2.2f-%2.2f Gev); lambda0; Number of Total Photons with energy 2-16 GeV", ptmin, ptmax));
        projection->Draw();
        myText(.10,.97, kBlack, "Total Pi0 Photon Lambda vs. Total Pi0 Photon Energy Projection, ");
        myText(.25,.92, kBlack, Form("Pt %2.2f-%2.2f GeV, mass within %s sigma of the mean", ptmin, ptmax, NumOfSigmasFromMeanMax.c_str()));
        projection->Write(Form("Lambda_projection_ptmin_%2.2fGeV_ptmax_%2.2fGeV", ptmin, ptmax));
        canvas->SaveAs(str_concat_converter(directory_name, Form("Lambda_vs_E_projection_ptmin_%2.2fGeV_ptmax_%2.2fGeV.png", ptmin, ptmax)));

    }
    
    canvas->Close();
    
}
