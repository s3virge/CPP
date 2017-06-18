//
//1
////down vote
////I don't know of a better way than:
//
//unsigned char byData[xxx]; 
//
//int nLength = sizeof(byData) * 2;
//char *pBuffer = new char[nLength + 1];
//pBuffer[nLength] = 0;
//
//for (int i = 0; i < sizeof(byData); i++)
//{
//    sprintf(pBuffer[2 * i], "%02X", byData[i]);
//}
//
////You can speed it up by using a Nibble to Hex method
//
//unsigned char byData[xxx];
//
//const char szNibbleToHex = { "0123456789ABCDEF" };
//
//int nLength = sizeof(byData) * 2;
//char *pBuffer = new char[nLength + 1];
//
//pBuffer[nLength] = 0;
//
//for (int i = 0; i < sizeof(byData); i++)
//{
//    // divide by 16
//    int nNibble = byData[i] >> 4;
//    pBuffer[2 * i]  = pszNibbleToHex[nNibble];
//
//    nNibble = byData[i] & 0x0F;
//    pBuffer[2 * i + 1]  = pszNibbleToHex[nNibble];
//
//}

///////////////////////////////////////////////////////

//3
//down vote
//This is a modified version of the Nibble to Hex method
//char **pp = 0;//указатель на массив указателей, которые содержат адреса строк

void hexArrayToStr(wchar_t* info, unsigned int infoLength, wchar_t **buffer) {
    
	const char* pszNibbleToHex = {"0123456789ABCDEF"};
    int nNibble, i;
    
	if (infoLength > 0) {
        if (info != NULL) {
            *buffer = (wchar_t *) malloc((infoLength * 2) + 1);
            buffer[0][(infoLength * 2)] = 0;
    
			for (i = 0; i < infoLength; i++) {
                nNibble = info[i] >> 4; // divide by 16
                buffer[0][2 * i] = pszNibbleToHex[nNibble];
                nNibble = info[i] & 0x0F;
                buffer[0][2 * i + 1] = pszNibbleToHex[nNibble];
            }
        } 
		else {
            *buffer = NULL;
        }
    } 
	else {
        *buffer = NULL;
    }
}