#include<openssl/bio.h>
#include<openssl/ssl.h>
#include<openssl/err.h>
#include<openssl/rsa.h>
#include<openssl/pem.h>
#include<stdio.h>
#include<string>
#include<cassert>
#include<iostream>
#include<unistd.h>
#include<fcntl.h>

using namespace std;
//公钥解密
int DecodeRSAKeyFile(const char * _strPemFileName , const char * _strData , unsigned char * buffer , int length)
{
    std::string strPemFileName = _strPemFileName;
    std::string strData = _strData ;

    //cout << _strData << endl;
    cout << "strData len " << strData.length() << endl;

    if(strPemFileName.empty() || strData.empty()){
        assert(false);
        return 0 ;
    }
     
    FILE * hPubKeyFile = fopen(strPemFileName.c_str() , "rb");
    if(hPubKeyFile == NULL){
        assert(false);
        return 0;
    }
 
    std::string strRet;
    RSA * pRSAPublicKey = RSA_new();
    if(PEM_read_RSA_PUBKEY(hPubKeyFile , &pRSAPublicKey , 0 , 0) == NULL){
        assert(false);
        return 0;
    }
 
    int nLen = RSA_size(pRSAPublicKey);
    cout << nLen << endl;
    char * pEncode = new char[nLen + 1] ;
    int ret = RSA_public_decrypt(256 , (const unsigned char *)_strData , (unsigned char * ) pEncode , pRSAPublicKey , RSA_PKCS1_PADDING); //加密时当采用RSA_PKCS1_PADDING补齐时，明文长度<=RSA_size(rsa)-11
    if(ret >= 0){
        strRet = std::string(pEncode , ret) ;
    }
 
    delete[] pEncode;
    RSA_free(pRSAPublicKey);
    fclose(hPubKeyFile);
    CRYPTO_cleanup_all_ex_data();
     
    if(strRet.length() + 1 > length){
        return 0;
    }
 
    memset(buffer , 0 , strRet.length() + 1) ;
    memcpy(buffer , &strRet[0] ,strRet.length());
 
    return strRet.length();
}

//私钥加密
int EncodeRSAKeyFile(const char * _strPemfileName , const char * _strData , unsigned char * buffer ,  int length)
{
    std::string strPemFileName = _strPemfileName;
    std::string strData = _strData ;
    if(strPemFileName.empty() || strData.empty()){
        assert(false);
        return 0;
    }
 
    FILE* hPriKeyFile = NULL;
    hPriKeyFile =  fopen(strPemFileName.c_str() , "rb");
    if(hPriKeyFile == NULL){
        assert(false);
        return 0;
    }
 
    std::string strRet;
    RSA* pRSAPriKey = RSA_new();
    if(PEM_read_RSAPrivateKey(hPriKeyFile , &pRSAPriKey , 0 , 0) == NULL ){
        assert(false);
        return 0;
    }
 
    int nLen = RSA_size(pRSAPriKey);
    char * pDecode = new char[nLen + 1];
    int ret = RSA_private_encrypt(strData.length() , (const unsigned char *)strData.c_str() , (unsigned char *)pDecode , pRSAPriKey , RSA_PKCS1_PADDING);
 
    if(ret >= 0){
        strRet = std::string((char *)pDecode , ret);
    }
 
    delete [] pDecode;
    RSA_free(pRSAPriKey);
    fclose(hPriKeyFile);
    CRYPTO_cleanup_all_ex_data();
 
    if(strRet.length() + 1 > length){
        return 0 ;
    } else {
        memset(buffer , 0 , strRet.length() + 1);
        memcpy(buffer , &strRet[0] , strRet.length());
    }

    return strRet.length();
}

/*int main()
{
#if 1
    //私钥加密
    //const std::string one = "{\"device_number\":\"99999\",\"logdev_type\":\"ac_server\",\"phydev_type\":\"x86_server\",\"logdev_id\":\"0000-0000-0000-0004\",\"logport_addr\":\"FE\",\"mac_addr\":\"60:F2:EF:00:00:04\",\"ether_type\":\"0800\",\"vlan_tag\":\"0000\",\"master_level\":2,\"master_topo\":\"free\",\"devnum_prefix\":\"00199-00366-00103\",\"logaddr_prefix\":\"030B.0203.0239\",\"run_mode\":\"master\",\"backup_acs_info\":{\"logdev_id\":\"1111-0000-0001-0002\",\"mac_addr\":\"60:F1:EF:AA:00:17\"},\"hb_timeout\":10,\"acs_bkup_timeout\":1}";
    //const std::string one = "{\"device_number\":\"99999\",\"logdev_type\":\"ac_server\",\"phydev_type\":\"x86_server\",\"logdev_id\":\"0000-0000-0000-0004\"}";
    std::string strPriKey = "./pri.key";
    std::string devRegFile = "./dev.info";
    int fd = open(devRegFile.c_str(),O_RDONLY);
    if(fd < 0)
    {
        std::cout<< "打开dev.info失败" <<std::endl;
        return -1;
    }
    char devinfo_json[1024] = {0};
    ssize_t r=0,w=0,len=0;
    while(true)
    {
        r = read(fd,devinfo_json+len,1024-len);
        if(r == 0)
        {
            close(fd);
            std::cout << "读取完成:\n" <<devinfo_json <<std::endl;
            break;
        }
        else if (r < 0)
        {
            std::cout << "读取dev.info失败" <<std::endl;
            return -1;
        }
        else
        {
            len += r;
        }
    }

    //加密
    unsigned char buffer[1024]={0} , buffer1[1024] ={0};
    int length = EncodeRSAKeyFile(strPriKey.c_str() , devinfo_json , buffer , 1024);

    fd = open("./dev.reg",O_WRONLY | O_CREAT |O_TRUNC,0644);
    if(fd < 0)
    {
        std::cout<< "创建dev.reg失败" <<std::endl;
        return -1;
    }
    len=0;
    while(true)
    {
        w = write(fd,buffer+len,length-len);
        if(w == 0)
        {
            close(fd);
            std::cout << "写入完成:\n" <<buffer <<std::endl;
            break;
        }
        else if (w < 0)
        {
            std::cout << "写入dev.reg失败" <<std::endl;
            return -1;
        }
        else
        {
            len += w;
        }
    }

    // std::string strResult = std::string((char *)buffer , length);
//     cout << "加密返回长度: " << length << endl;
//     cout << "密文:" << strResult << endl;
//     cout << "密文长度:" << strResult.length() << endl;
    return 0;
#else
    std::string strPriKey = "./pub.key";
    int fd = open("./dev.reg",O_RDONLY);
    char buf[20480]={0};
    int r = read(fd,buf,2048);
    std::cout<<r<<endl;
    unsigned char buf2[2048] = {0};
    std::string str(buf,r);
    std::cout<< str.size() <<std::endl;
    int length = DecodeRSAKeyFile(strPriKey.c_str() , str.c_str(), buf2 , r );
    std::string strOrgTxt = std::string((char *)buf2 , length);
    cout << "解密返回长度：" << length << endl <<  "解密后数据:" << strOrgTxt << endl ;
    return 0;
#endif
}*/
