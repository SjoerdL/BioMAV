/*
    This file is part of the BioMAV project.

    The BioMAV project is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The BioMAV project is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with The BioMAV project. If not, see <http://www.gnu.org/licenses/>.
*/
/* 
 * File:   Rgb2Lms.cpp
 * Author: jlangevoort
 * 
 * Created on August 12, 2011, 11:10 AM
 *
 *  Based on Original code:
 *
 *  Hoe heet ze ook al weer? Iets uit Texas...
 *
 *  // LMS Cone sensitivity functions
 *  // Stockman and Sharpe (2000) 10-deg cone fundamentals
 *  // Based on the Stiles and Burch (1959) 10-deg CMFs
 *  //
 *  // RGB Spectral Power Distribution from Sony GDM 500.
 *  // Provided by Tony Wilson. 2005-04-27
 */

#include "Rgb2Lms.h"

#include <stdlib.h>
#include <time.h>

//Python:  def gamma(x): return pow(x,GAMMA)  # GAMMA = 2.2
// Iets over het hoofd gezien!?

/**
 * Constructor
 *  initializes data structures and stores data necessary for conversion
 *
 * @param width
 * @param height
 * @param numberOfChannels
 */
Rgb2Lms::Rgb2Lms(int width, int height, int numberOfChannels)
{
    int i;

    // temporary data structure
    float temp [] =
    {0.000000F,0.000005F,0.000023F,0.000057F,0.000107F,
    0.000175F,0.000262F,0.000367F,0.000493F,0.000638F,
    0.000805F,0.000992F,0.001202F,0.001433F,0.001687F,
    0.001963F,0.002263F,0.002586F,0.002932F,0.003303F,
    0.003697F,0.004116F,0.004560F,0.005028F,0.005522F,
    0.006041F,0.006585F,0.007155F,0.007751F,0.008373F,
    0.009021F,0.009696F,0.010398F,0.011126F,0.011881F,
    0.012664F,0.013473F,0.014311F,0.015175F,0.016068F,
    0.016988F,0.017936F,0.018913F,0.019918F,0.020951F,
    0.022013F,0.023104F,0.024223F,0.025371F,0.026549F,
    0.027755F,0.028991F,0.030257F,0.031551F,0.032876F,
    0.034230F,0.035614F,0.037029F,0.038473F,0.039947F,
    0.041452F,0.042987F,0.044553F,0.046149F,0.047776F,
    0.049433F,0.051122F,0.052842F,0.054592F,0.056374F,
    0.058187F,0.060032F,0.061907F,0.063815F,0.065754F,
    0.067725F,0.069727F,0.071761F,0.073828F,0.075926F,
    0.078057F,0.080219F,0.082414F,0.084642F,0.086901F,
    0.089194F,0.091518F,0.093876F,0.096266F,0.098689F,
    0.101145F,0.103634F,0.106156F,0.108711F,0.111299F,
    0.113921F,0.116576F,0.119264F,0.121986F,0.124741F,
    0.127530F,0.130352F,0.133209F,0.136099F,0.139022F,
    0.141980F,0.144972F,0.147998F,0.151058F,0.154152F,
    0.157281F,0.160444F,0.163641F,0.166872F,0.170138F,
    0.173439F,0.176774F,0.180144F,0.183549F,0.186989F,
    0.190463F,0.193972F,0.197516F,0.201096F,0.204710F,
    0.208360F,0.212044F,0.215764F,0.219520F,0.223310F,
    0.227137F,0.230998F,0.234895F,0.238828F,0.242796F,
    0.246800F,0.250840F,0.254916F,0.259027F,0.263175F,
    0.267358F,0.271577F,0.275833F,0.280124F,0.284452F,
    0.288816F,0.293216F,0.297653F,0.302125F,0.306635F,
    0.311180F,0.315763F,0.320382F,0.325037F,0.329729F,
    0.334458F,0.339223F,0.344026F,0.348865F,0.353741F,
    0.358654F,0.363604F,0.368591F,0.373615F,0.378676F,
    0.383775F,0.388910F,0.394083F,0.399293F,0.404541F,
    0.409826F,0.415148F,0.420508F,0.425905F,0.431340F,
    0.436813F,0.442323F,0.447871F,0.453456F,0.459080F,
    0.464741F,0.470440F,0.476177F,0.481952F,0.487765F,
    0.493616F,0.499505F,0.505432F,0.511398F,0.517401F,
    0.523443F,0.529523F,0.535642F,0.541798F,0.547994F,
    0.554227F,0.560499F,0.566810F,0.573159F,0.579547F,
    0.585973F,0.592438F,0.598942F,0.605484F,0.612066F,
    0.618686F,0.625345F,0.632043F,0.638779F,0.645555F,
    0.652370F,0.659224F,0.666117F,0.673049F,0.680020F,
    0.687031F,0.694081F,0.701170F,0.708298F,0.715465F,
    0.722672F,0.729919F,0.737205F,0.744530F,0.751895F,
    0.759300F,0.766744F,0.774227F,0.781751F,0.789314F,
    0.796917F,0.804559F,0.812241F,0.819964F,0.827726F,
    0.835528F,0.843370F,0.851252F,0.859174F,0.867136F,
    0.875138F,0.883180F,0.891262F,0.899385F,0.907547F,
    0.915750F,0.923993F,0.932277F,0.940601F,0.948965F,
    0.957370F,0.965815F,0.974300F,0.982826F,0.991393F,
    1.000000F};

    // allocate memory for gamma matrix
    gamma_fcn = new float [256];
    // initialize matrix with temporary data structure
    for(i = 0; i < 256; i++)
    {
        gamma_fcn[i] = temp[i];
    }

    // initialize additional parameters
    lr = 0.28774559272100014F; lg = 0.5474191128279999F; lb = 0.16482994276499993F;
    mr = 0.11398651070700008F; mg = 0.6235511568220002F; mb = 0.2624652872689997F;
    sr = 0.015628637254F; sg = 0.05594590353900005F; sb = 0.9284469176350001F;
    
    this->width = width;
    this->height = height;
    this->numberOfChannels = numberOfChannels;
    // allocate memory for cell types and output image
    types = new int [width*height];
    activation = new float [width*height];
}

/**
 * Destructor
 */
Rgb2Lms::~Rgb2Lms()
{
    delete[] gamma_fcn;
    delete[] types;
    delete[] activation;
}

/**
 * returns activation of the l-cone cell based on the RGB input
 *
 * @param r;    red color component
 * @param g;    green color component
 * @param b;    blue color component
 *
 * @return activation
 */
float Rgb2Lms::convertRgbToL (int r, int g, int b)
{
    return gamma_fcn[r] * lr + gamma_fcn[g] * lg + gamma_fcn[b] * lb;
}

/**
 * returns activation of the m-cone cell based on the RGB input
 *
 * @param r;    red color component
 * @param g;    green color component
 * @param b;    blue color component
 *
 * @return activation
 */
float Rgb2Lms::convertRgbToM (int r, int g, int b)
{
    return gamma_fcn[r] * mr + gamma_fcn[g] * mg + gamma_fcn[b] * mb;
}

/**
 * returns activation of the s-cone cell based on the RGB input
 *
 * @param r;    red color component
 * @param g;    green color component
 * @param b;    blue color component
 *
 * @return activation
 */
float Rgb2Lms::convertRgbToS (int r, int g, int b)
{
    return gamma_fcn[r] * sr + gamma_fcn[g] * sg + gamma_fcn[b] * sb;
}

/**
 * returns activation of the rod cell based on the RGB input
 *
 * @param r;    red color component
 * @param g;    green color component
 * @param b;    blue color component
 *
 * @return activation
 */
float Rgb2Lms::convertRgbToR (int r, int g, int b)
{
    return 0;
}

/**
 * randomly sets the types of cell to a specific given ratio
 * 
 * @param l;    number of cells of the long type (ratio)
 * @param m;    number of cells of the medium type (ratio)
 * @param s;    number of cells of the short type (ratio)
 * @return true if successful
 */
void Rgb2Lms::setCellType(int l, int m, int s)
{
    int i, random, total;
    
    total = l+m+s;
    // initialize random seed
    srand(time(NULL));
        
    for (i = 0; i < width * height; i++)
    {
		if (l == 1)
		{
			types[i] = 2;
		}
		else if (m == 1)
		{
			types[i] = 1;
		}
		else //if (m == 1)
		{
			types[i] = 0;
		}
		
		/*
        random = rand() % total;
        
        if (random < s)
        {
            // set to type short
            types[i] = 0;
        }
        else if (random < m+s)
        {
            // set to type medium
            types[i] = 1;
        }
        else // if (random < l+m+s)
        {
            // set to type long
            types[i] = 2;
        }
        */
    }
}

/**
 * converts an input image into the appropriate activation for each cell type
 * 
 * @param input;        input image that needs to be converted
 * @return activation of the photo receptor cells 
 */
float* Rgb2Lms::getLms(uint8_t* input)
{
    int i, j;
    
    float max = 0;
    #pragma omp parallel for private(i, j)
    for (i = 0; i < width * height; i++)
    {
        j = i*3;
        // if long
        if (types[i] == 2)
        {
            activation[i] = convertRgbToS(input[j], input[j+1], input[j+2]);
        }
        // if medium
        else if (types[i] == 1)
        {
            activation[i] = convertRgbToM(input[j], input[j+1], input[j+2]);
        }
        // if short
        else // if (types[i] == 0)
        {
            activation[i] = convertRgbToL(input[j], input[j+1], input[j+2]);
        }
        
        if (activation[i] > max)
            max = activation[i];
    }
    
    // normalize
    #pragma omp parallel for private(i)
    for (i = 0; i < width * height; i++)
    {
        activation[i] = activation[i] / max;
    }
    
    return activation;
}
