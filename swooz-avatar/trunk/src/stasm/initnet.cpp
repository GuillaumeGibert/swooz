// $initnet.cpp 3.0 milbo$
// Derived from code by Henry Rowley http://vasc.ri.cmu.edu/NNFaceDetector.

#include "stasm.hpp"
// using namespace swStasm;

ForwardStruct **gNetList = NULL;

static int ngNetworks;

static const int MAX_UNITS = 10000; // used only for file sanity checking

//-----------------------------------------------------------------------------
void
FreeNetworks (void)
{
if (gNetList)
    {
    for (int i = 0; i < ngNetworks; i++)
       delete gNetList[i];
    delete[] gNetList;
    gNetList = NULL;
    }
}

//-----------------------------------------------------------------------------
// allocates memory and our global array of networks gNetList

void
AllocateNetworks (int nNetworks)
{
FreeNetworks();     // existing networks? if so deallocate them
gNetList = new ForwardStruct * [nNetworks];

for (int i = 0; i < nNetworks; i++)
    gNetList[i] = NULL;

ngNetworks = nNetworks;
}

//-----------------------------------------------------------------------------
// Given a list of network filenames, adds a ".wet" to the name, and loads
// the weights from those files.  This function loads multiple sets of
// weights into a single network, and assumes that the same set of network
// architectures has been loaded by pLoadMerge.

static void
LoadMergeWeights (ForwardStruct *pNet,                          // io
                  int nNetworks, const char *pNetworkNames[])   // in
{
int iInput = 0;
for (int iNet = 0; iNet < nNetworks; iNet++)
    {
    char s[SLEN], s1[SLEN], sFile[SLEN];
    sprintf(sFile, "%s.wet", pNetworkNames[iNet]);
    FILE *pWetFile = Fopen(sFile, "r");

    int  nEpochs, nWeights;
    if (2 != fscanf(pWetFile, "%d %s\n", &nEpochs, s1) ||
            nEpochs < 0 || strcmp(s1, "epochs"))
        Err("can't read number of epochs from %s", nEpochs, s1, sFile);

    if (2 != fscanf(pWetFile, "%d %s\n", &nWeights, s1) ||
            nWeights < 1 || nWeights > MAX_UNITS || strcmp(s1, "weights"))
        Err("can't read number of weights from %s", sFile);

    if (nWeights != pNet->nConns)
        Err("nWeights %d != nConns %d in %s", nWeights, pNet->nConns, sFile);

    for (int i = 0; i < nWeights; i++)
        {
        float tempFloat;
        if (fscanf(pWetFile, "%f\n", &tempFloat) != 1)
            Err("can't read weight from %s", sFile);
        pNet->pConnList[iInput++].pConn->weight = (_FLOAT)tempFloat;
        }
    Fgets(s, SLEN-1, pWetFile);
    if (!feof(pWetFile))
        Err("file %s has extra entries", sFile);
    fclose(pWetFile);
    }
}

//-----------------------------------------------------------------------------
// Load one or more networks into a single network.  The input is the number
// of networks to load, and either a list of names of network files (to which
// ".net" will be appended), or a list of FILE pointers from which the networks
// will be read.  The networks are merged such that they share inputs, then
// the hidden units are grouped together, and finally the outputs from each
// network are listed one after another.

static ForwardStruct *
pLoadMerge (int nNetworks, const char *pNetworkNames[], FILE *pNetFiles[])
{
// if a vector of FILE pointers was not given, use the network names in
// the pNetworkNames array to open the files

bool fFilesGiven = (pNetFiles != NULL);
if (!fFilesGiven)
    {
    pNetFiles = new FILE*[nNetworks];
    for (int iNet = 0; iNet < nNetworks; iNet++)
        {
        char sFile[SLEN];
        sprintf(sFile,"%s.net", pNetworkNames[iNet]);
        pNetFiles[iNet] = Fopen(sFile, "r");
        }
    }
int *netInputs = new int[nNetworks];
int *netHiddens = new int[nNetworks];
int *netOutputs = new int[nNetworks];
int *netUnits = new int[nNetworks];
int *netConns = new int[nNetworks];
int totalUnits = 0, totalInputs=0, totalHiddens = 0, totalOutputs = 0;
int totalConns = 0;

// Read in the number of units and number of inputs for each network.
// The inputs are merged together, but all other units must be
// represented separately

int iNet;
for (iNet = 0; iNet < nNetworks; iNet++)
    {
    char s1[SLEN];
    if (2 != fscanf(pNetFiles[iNet], "%d %s\n", &netUnits[iNet], s1) ||
            netUnits[iNet] < 1 || netUnits[iNet] > MAX_UNITS ||
            strcmp(s1, "units"))
        Err("can't read number of units from %s.net", pNetworkNames[iNet]);

    if (2 != fscanf(pNetFiles[iNet], "%d %s\n", &netInputs[iNet], s1) ||
            netInputs[iNet] < 1 || netInputs[iNet] > MAX_UNITS ||
            strcmp(s1, "inputs"))
        Err("can't read number of inputs from %s.net", pNetworkNames[iNet]);

    totalInputs = netInputs[iNet];
    totalUnits += netUnits[iNet] - netInputs[iNet];
    }
// read group,y,x information for each input unit

int *unitg = new int[totalUnits + totalInputs]; // tells you what group this unit is in
int *unity = new int[totalUnits + totalInputs];
int *unitx = new int[totalUnits + totalInputs];
for (iNet = 0;  iNet < nNetworks; iNet++)
    {
    for (int i = 0;  i < netInputs[iNet]; i++)
        if (3 != fscanf(pNetFiles[iNet], "%*d %d %d %d\n", unitg+i, unity+i, unitx+i))
            Err("can't read group,y,x from %s.net", pNetworkNames[iNet]);
    }
// read the number of hidden units, the hidden units themselves,
// and the number of output units

int ptr = totalInputs;
for (iNet = 0;  iNet < nNetworks; iNet++)
    {
    fscanf(pNetFiles[iNet], "\n%d hiddens\n", &netHiddens[iNet]);
    totalHiddens += netHiddens[iNet];
    for (int i = 0;  i < netHiddens[iNet]; i++)
        {
        if (3 != fscanf(pNetFiles[iNet], "%*d %d %d %d\n", unitg+ptr, unity+ptr, unitx+ptr))
            Err("can't read group,y,x from %s.net", pNetworkNames[iNet]);
        ptr++;
        }
    fscanf(pNetFiles[iNet], "\n%d outputs\n", &netOutputs[iNet]);
    totalOutputs += netOutputs[iNet];
    }
// read information on the output units, and the number of input connections

for (iNet = 0;  iNet < nNetworks; iNet++)
    {
    for (int i = 0;  i < netOutputs[iNet]; i++)
        {
        if (3 != fscanf(pNetFiles[iNet], "%*d %d %d %d\n", unitg+ptr, unity+ptr, unitx+ptr))
            Err("can't read group,y,x from %s.net", pNetworkNames[iNet]);
        ptr++;
        }
    char s1[SLEN];
    if (2 != fscanf(pNetFiles[iNet], "\n%d %s\n", &netConns[iNet], s1) ||
            strcmp(s1, "conns"))
        Err("can't read number of connections from %s.net", pNetworkNames[iNet]);
    totalConns += netConns[iNet];
    }
ForwardStruct *pNet = new ForwardStruct;
totalUnits += totalInputs;
pNet->nUnits = totalUnits;
pNet->nInputs = totalInputs;

pNet->iFirstHidden = totalInputs;
pNet->iFirstOutput = totalInputs+totalHiddens;
pNet->pUnitList = new ForwardUnit[pNet->nUnits];
pNet->pConnList = new ForwardConnectionRef[totalConns];
pNet->pConns = new ForwardConnection[totalConns];
pNet->nConns = totalConns;

int iUnit;
for (iUnit = 0; iUnit < pNet->nUnits; iUnit++)
    {
    pNet->pUnitList[iUnit].nInputs = 0;
    pNet->pUnitList[iUnit].activation = 1.0;

    if (iUnit == 0)
        pNet->pUnitList[iUnit].type = UNIT_TYPE_Bias;
    else if (iUnit < totalInputs)
        pNet->pUnitList[iUnit].type = UNIT_TYPE_Input;
    else
        pNet->pUnitList[iUnit].type = UNIT_TYPE_Tanh;
    }
int iFirstHidden = totalInputs;
int iFirstOutput = totalInputs + totalHiddens;

// read in the input connections

iFirstHidden = totalInputs;
iFirstOutput = totalInputs+totalHiddens;
int firstConn=0;
for (iNet = 0; iNet < nNetworks; iNet++)
    {
    for (int i = 0; i < netConns[iNet]; i++)
        {
        int iFrom,  iTo, iType;
        if (3 != fscanf(pNetFiles[iNet], "%d %d %d\n", &iFrom, &iTo, &iType))
            Err("can't read from,to,type from %s.net", pNetworkNames[iNet]);

        if (iType != -1)
            Err("\"type\" entry in %s.net is %d not -1", pNetworkNames[iNet], iType);
        if (i == netConns[iNet]-1 && iFrom != 0)
            Err("last \"from\" entry in %s.net is %d not 0", pNetworkNames[iNet], iFrom);

        if (iFrom >= netHiddens[iNet]+netInputs[iNet])
            iFrom += iFirstOutput-netHiddens[iNet]-netInputs[iNet];

        else if (iFrom >= netInputs[iNet])
            iFrom += iFirstHidden-netInputs[iNet];

        if (iTo >= netHiddens[iNet]+netInputs[iNet])
            iTo += iFirstOutput-netHiddens[iNet]-netInputs[iNet];

        else if (iTo >= netInputs[iNet])
            iTo += iFirstHidden-netInputs[iNet];

        pNet->pUnitList[iTo].nInputs++;
        pNet->pConnList[firstConn].iFrom = iFrom;
        pNet->pConnList[firstConn].iTo = iTo;
        firstConn++;
        }
    if (!fFilesGiven)
        fclose(pNetFiles[iNet]);

    iFirstHidden += netHiddens[iNet];
    iFirstOutput += netOutputs[iNet];
    }
// count the number of input connections going into each unit

firstConn = 0;
for (iUnit = 0; iUnit < totalUnits; iUnit++)
    {
    if (pNet->pUnitList[iUnit].nInputs == 0)
        pNet->pUnitList[iUnit].pConns = NULL;
    else
        {
        pNet->pUnitList[iUnit].pConns = pNet->pConns + firstConn;
        firstConn += pNet->pUnitList[iUnit].nInputs;
        pNet->pUnitList[iUnit].nInputs = 0;
        }
    }
// associate the input connections going into each unit with the unit itself

for (int iConn = 0; iConn < totalConns; iConn++)
    {
    ForwardConnection *pConn =
        pNet->pUnitList[pNet->pConnList[iConn].iTo].pConns +
        (pNet->pUnitList[pNet->pConnList[iConn].iTo].nInputs++);

    pConn->pFrom = pNet->pUnitList + pNet->pConnList[iConn].iFrom;

    pNet->pConnList[iConn].pConn = pConn;
    }
// clean up

if (!fFilesGiven)
    delete[] pNetFiles;

delete[] netInputs;
delete[] netHiddens;
delete[] netOutputs;
delete[] netUnits;
delete[] netConns;
delete[] unitx;
delete[] unity;
delete[] unitg;

return pNet;
}

//-----------------------------------------------------------------------------
// load a network, and place it in the the global array gNetList[iNet]

void
InitNetwork (int iNet, int nNetworks, const char *pNetworkNames[])
{
if (gNetList[iNet])
    delete gNetList[iNet];

gNetList[iNet] = pLoadMerge(nNetworks, pNetworkNames, NULL);

//TODOSTASMS for now we just point back to caller, should really malloc here for each net?
gNetList[iNet]->sName = pNetworkNames[iNet];

LoadMergeWeights(gNetList[iNet], nNetworks, pNetworkNames);
}
