#ifndef CDNBALANCE_H
#define CDNBALANCE_H

#include <string>
#include <sys/types.h>
#include <regex.h>


class CDNBalance {
public:
    static CDNBalance *getInstance() {
        return instance;
    }

    /**
     * @brief _img2
     * @param srcfile
     * @param suffix ,for example， 100x100.jpg
     * @param isUseHttps
     * @return
     */
    std::string _img2(const std::string& srcfile,const std::string& suffix,bool isUseHttps);
    // geo="",isUseHttps=false,isUseWebp=false
    std::string _img(const std::string& srcfile);
    // isUseHttps=false,isUseWebp=false
    std::string _img(const std::string& srcfile,const std::string& geo);
    // isUseWebp=false
    std::string _img(const std::string& srcfile,const std::string& geo,bool isUseHttps);
    std::string _img(const std::string& srcfile,const std::string& geo,bool isUseHttps,bool isUseWebp);

    std::string get_cdn_url(const std::string& path,const std::string& srcType,const std::string& time,bool useUrlTime,bool useHttps,const std::string& anotherHash,bool isFix);

    std::string SourceUrlPrepare(const std::string& srcfile);

    std::string smartGetImg(const std::string&  srcfile,bool isUseHttps) {
        return smartGetImg(srcfile,"",isUseHttps);
    }

    std::string smartGetImg(const std::string&  srcfile,const std::string&  suffix,bool isUseHttps);
    bool isMeilishuoPic(std::string& line);

    std::string generateThumbSuffix(const std::string& origUrl,
                                           const std::string& sType,
                                           const std::string& thumbWidth,
                                           const std::string& thumbHeight,
                                           bool sharpenFlag);


    std::string generateWebpSuffix(const std::string& thumbWidth,
                                          const std::string& thumbHeight,
                                          int rate);

    std::string generateThumbUrl(const std::string&   origUrl,
                                        const std::string&   sType,
                                        const std::string&   thumbWidth,
                                        const std::string&   thumbHeight,
                                         bool sharpenFlag);

    ~CDNBalance();

private:
    CDNBalance();
    bool isTimeStringOk(const std::string& timestr);
    std::string _imgHelper(const std::string& srcfile,const std::string& geo,bool isUseHttps,bool isUseWebp);
    static CDNBalance *instance;        
    regex_t regex;    
};

#endif


