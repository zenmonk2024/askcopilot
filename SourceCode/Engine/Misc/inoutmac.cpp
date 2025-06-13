#include "Precompile.h"

#include <time.h>
#include "KWin32.h"
#include "inoutmac.h"


#ifdef WIN32

#include "malloc.h"

#define MAX_ADAPTER_DESCRIPTION_LENGTH  128 // arb.
#define MAX_ADAPTER_NAME_LENGTH         256 // arb.
#define MAX_ADAPTER_ADDRESS_LENGTH      8   // arb.


typedef struct {
    char String[4 * 4];
} K_IP_ADDRESS_STRING, *K_PIP_ADDRESS_STRING, K_IP_MASK_STRING, *K_PIP_MASK_STRING;

typedef struct _K_IP_ADDR_STRING {
    struct _K_IP_ADDR_STRING* Next;
    K_IP_ADDRESS_STRING IpAddress;
    K_IP_MASK_STRING IpMask;
    DWORD Context;
} K_IP_ADDR_STRING, *K_PIP_ADDR_STRING;

typedef struct _K_IP_ADAPTER_INFO {
    struct _K_IP_ADAPTER_INFO* Next;
    DWORD ComboIndex;
    char AdapterName[MAX_ADAPTER_NAME_LENGTH + 4];
    char Description[MAX_ADAPTER_DESCRIPTION_LENGTH + 4];
    UINT AddressLength;
    BYTE Address[MAX_ADAPTER_ADDRESS_LENGTH];
    DWORD Index;
    UINT Type;
    UINT DhcpEnabled;
    K_PIP_ADDR_STRING CurrentIpAddress;
    K_IP_ADDR_STRING IpAddressList;
    K_IP_ADDR_STRING GatewayList;
    K_IP_ADDR_STRING DhcpServer;
    BOOL HaveWins;
    K_IP_ADDR_STRING PrimaryWinsServer;
    K_IP_ADDR_STRING SecondaryWinsServer;
    time_t LeaseObtained;
    time_t LeaseExpires;
} K_IP_ADAPTER_INFO, *K_PIP_ADAPTER_INFO;



//�˺�����ȷ��������ַ����
//����ֵΪ��������
int gGetMacAndIPAddress(
    unsigned char *pMacAddressIn,   unsigned long *pIPAddressIn, 
    unsigned char  *pMacAddressOut, unsigned long *pIPAddressOut, 
    unsigned uMask, 
    int nMacLength
)
{
	typedef DWORD(CALLBACK * PGAINFO)(K_PIP_ADAPTER_INFO, PULONG);//GetAdaptersInfo

    //����IP Helper API ����Ŀ��ļ�
    HINSTANCE hInst = LoadLibrary("iphlpapi.dll");
    if(!hInst)
		return -1;
    PGAINFO pGAInfo = (PGAINFO)GetProcAddress(hInst,"GetAdaptersInfo");
	if (!pGAInfo)
		return -1;
	K_PIP_ADAPTER_INFO pInfo = NULL;

	ULONG ulSize = 0;

	pGAInfo(pInfo, &ulSize);//��һ�ε��ã���ȡ��������С

	pInfo = (K_PIP_ADAPTER_INFO)alloca(ulSize);

	pGAInfo(pInfo, &ulSize);

	bool bIn = false;
	char* pMacAddressInX = (char*)alloca(nMacLength + 1);
	DWORD nIPAddressInX = 0;
	bool bOut = false;
	char* pMacAddressOutX = (char*)alloca(nMacLength + 1);
	DWORD nIPAddressOutX = 0;

	DWORD nMAcNum = 0;
    //����ÿһ������

    while (pInfo && (!bIn || !bOut))
    {
		K_PIP_ADDR_STRING pAddTemp = &(pInfo->IpAddressList);       
		
		while (pAddTemp)/*����IP�б��е�ÿһ��Ԫ��*/
		{
			DWORD nAddress = inet_addr(pAddTemp->IpAddress.String);
			if (!bIn &&
				((nAddress & 0x0000FFFF) == uMask)
				)
			{
				bIn = true;
				nIPAddressInX = nAddress;
				//�����ַ�ĳ���
				if (pInfo->AddressLength == nMacLength)
					memcpy(pMacAddressInX, pInfo->Address, nMacLength);
				else
					memset(pMacAddressInX, 0, nMacLength);
			}
			
			if (!bOut &&
				((nAddress & 0x0000FFFF) != uMask)
				)
			{
				bOut = true;
				nIPAddressOutX = nAddress;
				//�����ַ�ĳ���
				if (pInfo->AddressLength == nMacLength)
					memcpy(pMacAddressOutX, pInfo->Address, nMacLength);
				else
					memset(pMacAddressOutX, 0, nMacLength);
			}
			pAddTemp = pAddTemp->Next;
		}
		//����ǰָ��������һ��
		pInfo = pInfo->Next;

		nMAcNum++;
	}
	
	if (bIn)
	{
		if (pMacAddressIn)
			memcpy(pMacAddressIn, pMacAddressInX, nMacLength);
		if (pIPAddressIn)
			*pIPAddressIn = nIPAddressInX;

		if (!bOut)
		{
			if (pMacAddressOut)
				memcpy(pMacAddressOut, pMacAddressInX, nMacLength);
			if (pIPAddressOut)
				*pIPAddressOut = nIPAddressInX;
		}
	}

	if (bOut)
	{
		if (pMacAddressOut)
			memcpy(pMacAddressOut, pMacAddressOutX, nMacLength);
		if (pIPAddressOut)
			*pIPAddressOut = nIPAddressOutX;

		if (!bIn)
		{
			if (pMacAddressIn)
				memcpy(pMacAddressIn, pMacAddressOutX, nMacLength);
			if (pIPAddressIn)
				*pIPAddressIn = nIPAddressOutX;
		}
	}

	FreeLibrary(hInst);

	return nMAcNum;
}

#else
// Linux

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
//#include <net/if_arp.h>
#include <arpa/inet.h>

//�˺�����ȷ��������ַ����
//����ֵΪ��������
int gGetMacAndIPAddress(
    unsigned char *pMacAddressIn,  unsigned long *pIPAddressIn, 
    unsigned char *pMacAddressOut, unsigned long *pIPAddressOut, 
    unsigned ulMask, 
    int nMacLength
)
{
    int nResult  = false;
    int nRetCode = false;

    int nSocket = -1;
    int nSize   = -1;
    struct ifconf  ifc;
    struct ifreq  *pifreq    = NULL;
    
	struct sockaddr_in InAddress;
	unsigned long ulAddress = 0;
	bool bIn = false;
	char *pMacAddressInX = (char *)alloca(nMacLength + 1);
	unsigned long ulIPAddressInX = 0;
	bool bOut = false;
	char *pMacAddressOutX = (char *)alloca(nMacLength + 1);
	unsigned ulIPAddressOutX = 0;
    

    memset(&ifc, 0, sizeof(ifc));

    nSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (nSocket < 0)
        goto Exit0;

    ifc.ifc_req = NULL;
    ifc.ifc_len = 0;
    nRetCode = ioctl(nSocket, SIOCGIFCONF, &ifc);
    if (nRetCode)  // success when nRetCode == 0
        goto Exit0;
        
    nSize = ifc.ifc_len / sizeof(struct ifreq);
    if (!nSize)
        goto Exit0;
        
    ifc.ifc_req = (struct ifreq *)malloc(ifc.ifc_len + sizeof(struct ifreq));   // add for last check node
    if (!ifc.ifc_req)
        goto Exit0;
    
    memset(ifc.ifc_req, 0, sizeof(ifc.ifc_len + sizeof(struct ifreq)));
    
    nRetCode = ioctl(nSocket, SIOCGIFCONF, &ifc);
    if (nRetCode)  // success when nRetCode == 0
        goto Exit0;
    
    for (pifreq = ifc.ifc_req; pifreq < (ifc.ifc_req + nSize); ++pifreq)
    {
        if (pifreq[0].ifr_addr.sa_data == pifreq[1].ifr_addr.sa_data)
            continue;  // duplicate, skip it

        //printf( "Interface:  %s\n", ifr->ifr_name );
        nRetCode = ioctl(nSocket, SIOCGIFFLAGS, pifreq);
        if (nRetCode)   // success when nRetCode == 0
            continue;   // failed to get flags, skip it

        nRetCode = ioctl(nSocket, SIOCGIFADDR, pifreq);
        if (nRetCode)   // success when nRetCode == 0
            continue;
        
        InAddress = *((struct sockaddr_in *)&(pifreq->ifr_addr));
        ulAddress = InAddress.sin_addr.s_addr; 
        
        if (ulAddress == 0x100007f) // 127.0.0.1    skip
            continue;

        nRetCode = ioctl(nSocket, SIOCGIFHWADDR, pifreq);
        if (nRetCode)   // success when nRetCode == 0
            continue;
            
        if (
            (!bIn) &&
	        ((ulAddress & 0x0000FFFF) == ulMask)
	    )
	    {
	        bIn = true;
	        ulIPAddressInX = ulAddress;
            memcpy(pMacAddressInX, &pifreq->ifr_hwaddr.sa_data, IFHWADDRLEN);
        }

        if (
            (!bOut) &&
	        ((ulAddress & 0x0000FFFF) != ulMask)
	    )
        {
	        bOut = true;
	        ulIPAddressOutX = ulAddress;
            memcpy(pMacAddressOutX, &pifreq->ifr_hwaddr.sa_data, IFHWADDRLEN);
        }
    }
    
    
	if (bIn)
	{
		if (pMacAddressIn)
			memcpy(pMacAddressIn, pMacAddressInX, nMacLength);
		
		if (pIPAddressIn)
			*pIPAddressIn = ulIPAddressInX;

		if (!bOut)
		{
			if (pMacAddressOut)
				memcpy(pMacAddressOut, pMacAddressInX, nMacLength);
			if (pIPAddressOut)
				*pIPAddressOut = ulIPAddressInX;
		}
	}

	if (bOut)
	{
		if (pMacAddressOut)
			memcpy(pMacAddressOut, pMacAddressOutX, nMacLength);
		if (pIPAddressOut)
			*pIPAddressOut = ulIPAddressOutX;

		if (!bIn)
		{
			if (pMacAddressIn)
				memcpy(pMacAddressIn, pMacAddressOutX, nMacLength);
			if (pIPAddressIn)
				*pIPAddressIn = ulIPAddressOutX;
		}
	}
    

    nResult = true;    
Exit0:
    
    if (ifc.ifc_req)
    {
        free(ifc.ifc_req);
        ifc.ifc_req = NULL;
    }

    if (nSocket != -1)
    {
        close(nSocket);
        nSocket = -1;
    }
    
    if (!nResult)
        nSize = -1;
    
    return nSize;
}




#endif // WIN32
