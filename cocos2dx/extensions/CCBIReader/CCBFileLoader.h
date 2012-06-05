#ifndef _CCBFILE_LOADER_H_
#define _CCBFILE_LOADER_H_

#include "CCNodeLoader.h"

NS_CC_EXT_BEGIN

/* Forward declaration. */
class CCBReader;

class CCBFileLoader : public CCNodeLoader {
    protected:
        virtual CCNode * createCCNode(CCNode *, CCBReader *);

        virtual void onHandlePropTypeCCBFile(CCNode * pNode, CCNode * pParent, const char * pPropertyName, CCNode *, CCBReader *);
};

NS_CC_EXT_END

#endif
