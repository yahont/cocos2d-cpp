/****************************************************************************
Copyright (c) 2010 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#include "CCTMXObjectGroup.h"
#include "ccMacros.h"
namespace cocos2d {

	const char * valueForKey(std::string key, StringToStringDictionary *dict)
	{
		if (!dict)
		{
			return "";
		}
		StringToStringDictionary::iterator it = dict->find(key);
		return it!=dict->end() ? it->second.c_str() : "";
	}

	//implementation CCTMXObjectGroup

	CCTMXObjectGroup::CCTMXObjectGroup()	
	{
		m_sGroupName = "";
		m_tPositionOffset = CGPointZero;
		m_pObjects = new std::vector<StringToStringDictionary*>();
		m_pProperties = new StringToStringDictionary();
	}
	CCTMXObjectGroup::~CCTMXObjectGroup()
	{
		CCLOGINFO( "cocos2d: deallocing. ");
		CCX_SAFE_DELETE(m_pProperties);
		CCX_SAFE_DELETE(m_pObjects);
	}
	StringToStringDictionary * CCTMXObjectGroup::objectNamed(const char *objectName)
	{
		std::vector<StringToStringDictionary*>::iterator it;
		for (it = m_pObjects->begin(); it != m_pObjects->end(); ++it)
		{
			if ( *it && strcmp(valueForKey("name", *it), objectName)==0 )
			{
				return *it;
			}
		}
		// object not found
		return NULL;
	}
	const char *CCTMXObjectGroup::propertyNamed(const char* propertyName)
	{
		return valueForKey(propertyName, m_pProperties);
	}

}// namespace cocos2d