/****************************************************************************
 Copyright (c) 2013      Zynga Inc.
 Copyright (c) 2013-2014 Chukong Technologies Inc.

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

#include "CCLabel.h"
#include "CCFontAtlasCache.h"
#include "CCLabelTextFormatter.h"
#include "CCSprite.h"
#include "CCShaderCache.h"
#include "ccUTF8.h"
#include "CCSpriteFrame.h"
#include "CCDirector.h"
#include "renderer/CCRenderer.h"
#include "CCFont.h"

NS_CC_BEGIN

const int Label::DefultFontSize = 50;

Label* Label::create()
{
    Label *ret = new Label();

    if (!ret)
        return nullptr;

    ret->autorelease();

    return ret;
}

Label* Label::createWithFontDefinition(const std::string& text, FontDefinition &textDefinition)
{
    auto ret = new Label();

    if (ret)
    {
        ret->setFontDefinition(textDefinition);
        ret->setString(text);
        ret->autorelease();
    }

    return ret;
}

Label* Label::create(const std::string& text, const std::string& fontName, float fontSize, const Size& dimensions /* = Size::ZERO */, TextHAlignment hAlignment /* = TextHAlignment::LEFT */, TextVAlignment vAlignment /* = TextVAlignment::TOP */)
{
    auto ret = new Label(nullptr,hAlignment);

    if (ret)
    {
        do 
        {
            if (fontName.find('.') != fontName.npos)
            {
                TTFConfig ttfConfig(fontName.c_str(),fontSize,GlyphCollection::DYNAMIC);
                if (ret->setTTFConfig(ttfConfig))
                {
                    break;
                }
            }

            FontDefinition fontDef;
            fontDef._fontName = fontName;
            fontDef._fontSize = fontSize;
            fontDef._dimensions = dimensions;
            fontDef._alignment = hAlignment;
            fontDef._vertAlignment = vAlignment;
            ret->setFontDefinition(fontDef);
        } while (0);

        ret->setDimensions(dimensions.width,dimensions.height);
        ret->setString(text);
        ret->autorelease();
    }   

    return ret;
}

Label* Label::createWithTTF(const TTFConfig& ttfConfig, const std::string& text, TextHAlignment alignment /* = TextHAlignment::CENTER */, int lineSize /* = 0 */)
{
    Label *ret = new Label(nullptr,alignment);

    if (!ret)
        return nullptr;

    if (ret->setTTFConfig(ttfConfig))
    {
        ret->setMaxLineWidth(lineSize);
        ret->setString(text);
        ret->autorelease();
        return ret;
    }
    else
    {
        delete ret;
        return nullptr;
    }
}

Label* Label::createWithTTF(const std::string& text, const std::string& fontFilePath, int fontSize, int lineSize /* = 0 */, TextHAlignment alignment /* = TextHAlignment::CENTER */, GlyphCollection glyphs /* = GlyphCollection::NEHE */, const char *customGlyphs /* = 0 */, bool useDistanceField /* = false */)
{
    TTFConfig ttfConfig(fontFilePath.c_str(),fontSize,glyphs,customGlyphs,useDistanceField);
    return createWithTTF(ttfConfig,text,alignment,lineSize);
}

Label* Label::createWithBMFont(const std::string& bmfontFilePath, const std::string& text,const TextHAlignment& alignment /* = TextHAlignment::LEFT */, int lineWidth /* = 0 */, const Point& imageOffset /* = Point::ZERO */)
{
    Label *ret = new Label(nullptr,alignment);

    if (!ret)
        return nullptr;

    if (ret->setBMFontFilePath(bmfontFilePath,imageOffset))
    {
        ret->setMaxLineWidth(lineWidth);
        ret->setString(text);
        ret->autorelease();
        return ret;
    }
    else
    {
        delete ret;
        return nullptr;
    }
}

Label* Label::createWithCharMap(const std::string& plistFile)
{
    Label *ret = new Label();

    if (!ret)
        return nullptr;

    if (ret->setCharMap(plistFile))
    {
        ret->autorelease();
        return ret;
    }
    else
    {
        delete ret;
        return nullptr;
    }
}

Label* Label::createWithCharMap(Texture2D* texture, int itemWidth, int itemHeight, int startCharMap)
{
    Label *ret = new Label();

    if (!ret)
        return nullptr;

    if (ret->setCharMap(texture,itemWidth,itemHeight,startCharMap))
    {
        ret->autorelease();
        return ret;
    }
    else
    {
        delete ret;
        return nullptr;
    }
}

Label* Label::createWithCharMap(const std::string& charMapFile, int itemWidth, int itemHeight, int startCharMap)
{
    Label *ret = new Label();

    if (!ret)
        return nullptr;

    if (ret->setCharMap(charMapFile,itemWidth,itemHeight,startCharMap))
    {
        ret->autorelease();
        return ret;
    }
    else
    {
        delete ret;
        return nullptr;
    }
}

bool Label::setCharMap(const std::string& plistFile)
{
    FontAtlas *newAtlas = FontAtlasCache::getFontAtlasCharMap(plistFile);

    if (!newAtlas)
        return false;

    if (initWithFontAtlas(newAtlas))
    {
        _currentLabelType = LabelType::CHARMAP;
        return true;
    }

    return false;
}

bool Label::setCharMap(Texture2D* texture, int itemWidth, int itemHeight, int startCharMap)
{
    FontAtlas *newAtlas = FontAtlasCache::getFontAtlasCharMap(texture,itemWidth,itemHeight,startCharMap);

    if (!newAtlas)
        return false;

    if (initWithFontAtlas(newAtlas))
    {
        _currentLabelType = LabelType::CHARMAP;
        return true;
    }

    return false;
}

bool Label::setCharMap(const std::string& charMapFile, int itemWidth, int itemHeight, int startCharMap)
{
    FontAtlas *newAtlas = FontAtlasCache::getFontAtlasCharMap(charMapFile,itemWidth,itemHeight,startCharMap);

    if (!newAtlas)
        return false;

    if (initWithFontAtlas(newAtlas))
    {
        _currentLabelType = LabelType::CHARMAP;
        return true;
    }

    return false;
}

Label::Label(FontAtlas *atlas, TextHAlignment alignment, bool useDistanceField,bool useA8Shader)
: _reusedLetter(nullptr)
, _commonLineHeight(0.0f)
, _lineBreakWithoutSpaces(false)
, _maxLineWidth(0)
, _labelWidth(0)
, _labelHeight(0)
, _hAlignment(alignment)
, _currentUTF16String(nullptr)
, _originalUTF16String(nullptr)
, _horizontalKernings(nullptr)
, _fontAtlas(atlas)
, _isOpacityModifyRGB(false)
, _useDistanceField(useDistanceField)
, _useA8Shader(useA8Shader)
, _fontScale(1.0f)
, _uniformEffectColor(0)
,_currNumLines(-1)
,_textSprite(nullptr)
,_contentDirty(false)
{
    _cascadeColorEnabled = true;
    _batchNodes.push_back(this);

    _fontDefinition._fontName = "Helvetica";
    _fontDefinition._fontSize = 32;
    _fontDefinition._alignment = TextHAlignment::LEFT;
    _fontDefinition._vertAlignment = TextVAlignment::TOP;
}

Label::~Label()
{   
    delete [] _currentUTF16String;
    delete [] _originalUTF16String;
    delete [] _horizontalKernings;

    if (_fontAtlas)
    {
        FontAtlasCache::releaseFontAtlas(_fontAtlas);
    }

    CC_SAFE_RELEASE_NULL(_reusedLetter);
}

bool Label::init()
{ 
    bool ret = true;
    if(_fontAtlas)
    {
        ret = SpriteBatchNode::initWithTexture(_fontAtlas->getTexture(0), 30);
        if (_reusedLetter == nullptr)
        {
            _reusedLetter = Sprite::createWithTexture(_fontAtlas->getTexture(0));
            _reusedLetter->setOpacityModifyRGB(_isOpacityModifyRGB);            
            _reusedLetter->retain();
            _reusedLetter->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
            _reusedLetter->setBatchNode(this);
        }
    }
    _currLabelEffect = LabelEffect::NORMAL;
    initProgram();

    return ret;
}

void Label::initProgram()
{
    switch (_currLabelEffect)
    {
    case cocos2d::LabelEffect::NORMAL:
    case cocos2d::LabelEffect::SHADOW:
        if (_useDistanceField)
            setShaderProgram(ShaderCache::getInstance()->getProgram(GLProgram::SHADER_NAME_LABEL_DISTANCEFIELD_NORMAL));
        else if (_useA8Shader)
            setShaderProgram(ShaderCache::getInstance()->getProgram(GLProgram::SHADER_NAME_POSITION_TEXTURE_A8_COLOR));
        else
            setShaderProgram(ShaderCache::getInstance()->getProgram(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR));

        break;
    case cocos2d::LabelEffect::OUTLINE:
            setShaderProgram(ShaderCache::getInstance()->getProgram(GLProgram::SHADER_NAME_LABEL_OUTLINE));
        break;
    case cocos2d::LabelEffect::GLOW:
        if (_useDistanceField)
            setShaderProgram(ShaderCache::getInstance()->getProgram(GLProgram::SHADER_NAME_LABEL_DISTANCEFIELD_GLOW));
        break;
    default:
        return;
    }
    _uniformEffectColor = glGetUniformLocation(_shaderProgram->getProgram(), "v_effectColor");
}

bool Label::initWithFontAtlas(FontAtlas* atlas,bool distanceFieldEnabled /* = false */, bool useA8Shader /* = false */)
{
    FontAtlas *oldAtlas = _fontAtlas;
    bool oldDistanceFieldEnable = _useDistanceField;
    bool oldA8ShaderEnabel = _useA8Shader;

    _fontAtlas = atlas;
    _useDistanceField = distanceFieldEnabled;
    _useA8Shader = useA8Shader;

    bool ret = Label::init();
    if (oldAtlas)
    {
        if (ret)
        {
            FontAtlasCache::releaseFontAtlas(oldAtlas);
        }
        else
        {
            _fontAtlas = oldAtlas;
            _useDistanceField = oldDistanceFieldEnable;
            _useA8Shader = oldA8ShaderEnabel;
            Label::init();

            FontAtlasCache::releaseFontAtlas(atlas);
        }
    }

    if (_fontAtlas)
    {
        _commonLineHeight = _fontAtlas->getCommonLineHeight();
        _contentDirty = true;
    }

    return ret;
}

bool Label::setTTFConfig(const TTFConfig& ttfConfig)
{
    FontAtlas *newAtlas = FontAtlasCache::getFontAtlasTTF(ttfConfig);

    if (!newAtlas)
        return false;

    if (initWithFontAtlas(newAtlas,ttfConfig.distanceFieldEnabled,true))
    {
        _fontConfig = ttfConfig;
        if (ttfConfig.outlineSize > 0)
        {
            _fontConfig.distanceFieldEnabled = false;
            _useDistanceField = false;
            _useA8Shader = false;
            _currLabelEffect = LabelEffect::OUTLINE;
            initProgram();
        }
        else if(ttfConfig.distanceFieldEnabled)
        {
            this->setFontScale(1.0f * ttfConfig.fontSize / DefultFontSize);
        }
        _currentLabelType = LabelType::TTF;
        return true;
    }
    else
    {
        return false;
    }
}

bool Label::setBMFontFilePath(const std::string& bmfontFilePath, const Point& imageOffset /* = Point::ZERO */)
{
    FontAtlas *newAtlas = FontAtlasCache::getFontAtlasFNT(bmfontFilePath,imageOffset);

    if (!newAtlas)
        return false;

    if (initWithFontAtlas(newAtlas))
    {
        _currentLabelType = LabelType::BMFONT;
        return true;
    }

    return false;
}

void Label::setFontDefinition(const FontDefinition& textDefinition)
{
    _fontDefinition = textDefinition;
    _currentLabelType = LabelType::STRING_TEXTURE;
    _contentDirty = true;
}

void Label::setString(const std::string& text)
{
    _originalUTF8String = text;
    _contentDirty = true;
}

void Label::setAlignment(TextHAlignment hAlignment,TextVAlignment vAlignment)
{
    if (hAlignment != _hAlignment || vAlignment != _vAlignment)
    {
        _fontDefinition._alignment = hAlignment;
        _fontDefinition._vertAlignment = vAlignment;

        _hAlignment = hAlignment;
        _vAlignment = vAlignment;

        _contentDirty = true;
    }
}

void Label::setMaxLineWidth(unsigned int maxLineWidth)
{
    if (_maxLineWidth != maxLineWidth)
    {
        _maxLineWidth = maxLineWidth;
        _contentDirty = true;
    }
}

void Label::setDimensions(unsigned int width,unsigned int height)
{
    if (height != _labelHeight || width != _labelWidth)
    {
        _fontDefinition._dimensions.width = width;
        _fontDefinition._dimensions.height = height;

        _labelHeight = height;
        _labelWidth = width;
        _maxLineWidth = width;
        _contentDirty = true;
    }  
}

void Label::setLineBreakWithoutSpace(bool breakWithoutSpace)
{
    if (breakWithoutSpace != _lineBreakWithoutSpaces)
    {
        // store
        _lineBreakWithoutSpaces = breakWithoutSpace;

        // need to align text again
        if(_currentUTF16String)
        {
            resetCurrentString();
            alignText();
        }
    }
}

void Label::setScale(float scale)
{
    if (_useDistanceField)
    {
        scale *= _fontScale;
    } 
    Node::setScale(scale);
}

void Label::setScaleX(float scaleX)
{
    if (_useDistanceField)
    {
        scaleX *= _fontScale;
    } 
    Node::setScaleX(scaleX);
}

void Label::setScaleY(float scaleY)
{
    if (_useDistanceField)
    {
        scaleY *= _fontScale;
    } 
    Node::setScaleY(scaleY);
}

float Label::getScaleY() const
{
    if (_useDistanceField)
    {
        return _scaleY / _fontScale;
    }
    else
    {
        return _scaleY;
    }
}

float Label::getScaleX() const
{
    if (_useDistanceField)
    {
        return _scaleX / _fontScale;
    }
    else
    {
        return _scaleX;
    }
}

void Label::alignText()
{
    if (_fontAtlas == nullptr)
    {
        return;
    }

    for (const auto& batchNode:_batchNodes)
    {
        batchNode->getTextureAtlas()->removeAllQuads();
    }
    _fontAtlas->prepareLetterDefinitions(_currentUTF16String);
    auto textures = _fontAtlas->getTextures();
    if (textures.size() > _batchNodes.size())
    {
        for (auto index = _batchNodes.size(); index < textures.size(); ++index)
        {
            auto batchNode = SpriteBatchNode::createWithTexture(textures[index]);
            batchNode->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
            batchNode->setPosition(Point::ZERO);
            Node::addChild(batchNode,0,Node::INVALID_TAG);
            _batchNodes.push_back(batchNode);
        }
    }
    LabelTextFormatter::createStringSprites(this);    
    if(_maxLineWidth > 0 && _contentSize.width > _maxLineWidth && LabelTextFormatter::multilineText(this) )      
        LabelTextFormatter::createStringSprites(this);

    if(_labelWidth >0 || (_currNumLines > 1 && _hAlignment != TextHAlignment::LEFT))
        LabelTextFormatter::alignText(this);

    int strLen = cc_wcslen(_currentUTF16String);
    Rect uvRect;
    Sprite* letterSprite;
    for(const auto &child : _children) {
        int tag = child->getTag();
        if(tag >= strLen)
        {
            SpriteBatchNode::removeChild(child, true);
        }
        else if(tag >= 0)
        {
            letterSprite = dynamic_cast<Sprite*>(child);
            if (letterSprite)
            {
                uvRect.size.height = _lettersInfo[tag].def.height;
                uvRect.size.width  = _lettersInfo[tag].def.width;
                uvRect.origin.x    = _lettersInfo[tag].def.U;
                uvRect.origin.y    = _lettersInfo[tag].def.V;

                letterSprite->setTexture(textures[_lettersInfo[tag].def.textureID]);
                letterSprite->setTextureRect(uvRect);
            }          
        }
    }

    int index;
    for (int ctr = 0; ctr < _limitShowCount; ++ctr)
    {        
        if (_lettersInfo[ctr].def.validDefinition)
        {
            updateSpriteWithLetterDefinition(_lettersInfo[ctr].def,textures[_lettersInfo[ctr].def.textureID]);
            _reusedLetter->setPosition(_lettersInfo[ctr].position);
            index = _batchNodes[_lettersInfo[ctr].def.textureID]->getTextureAtlas()->getTotalQuads();
            _batchNodes[_lettersInfo[ctr].def.textureID]->insertQuadFromSprite(_reusedLetter,index);
        }     
    }

    updateColor();
}

bool Label::computeHorizontalKernings(unsigned short int *stringToRender)
{
    if (_horizontalKernings)
    {
        delete [] _horizontalKernings;
        _horizontalKernings = nullptr;
    }

    int letterCount = 0;
    _horizontalKernings = _fontAtlas->getFont()->getHorizontalKerningForTextUTF16(stringToRender, letterCount);

    if(!_horizontalKernings)
        return false;
    else
        return true;
}

bool Label::setOriginalString(unsigned short *stringToSet)
{
    if (_originalUTF16String)
    {
        delete [] _originalUTF16String;
    }

    int newStringLenght = cc_wcslen(stringToSet);
    _originalUTF16String = new unsigned short int [newStringLenght + 1];
    memset(_originalUTF16String, 0, (newStringLenght + 1) * 2);
    memcpy(_originalUTF16String, stringToSet, (newStringLenght * 2));
    _originalUTF16String[newStringLenght] = 0;

    return true;
}

bool Label::setCurrentString(unsigned short *stringToSet)
{
    // set the new string
    if (_currentUTF16String)
    {
        delete [] _currentUTF16String;
    }

    _currentUTF16String  = stringToSet;
    computeStringNumLines();

    // compute the advances
    if (_fontAtlas)
    {
        computeHorizontalKernings(stringToSet);
    }
    return true;
}

void Label::resetCurrentString()
{
    if ((!_currentUTF16String) && (!_originalUTF16String))
        return;

    // set the new string
    if (_currentUTF16String)
    {
        delete [] _currentUTF16String;
        _currentUTF16String = 0;
    }

    int stringLenght = cc_wcslen(_originalUTF16String);
    _currentUTF16String = new unsigned short int [stringLenght + 1];
    memcpy(_currentUTF16String, _originalUTF16String, stringLenght * 2);
    _currentUTF16String[stringLenght] = 0;

}

void Label::updateSpriteWithLetterDefinition(const FontLetterDefinition &theDefinition, Texture2D *theTexture)
{
    _reusedRect.size.height = theDefinition.height;
    _reusedRect.size.width  = theDefinition.width;
    _reusedRect.origin.x    = theDefinition.U;
    _reusedRect.origin.y    = theDefinition.V;

    if(_reusedLetter->getBatchNode() != _batchNodes[theDefinition.textureID])
        _reusedLetter->setBatchNode(_batchNodes[theDefinition.textureID]);
    _reusedLetter->setTextureRect(_reusedRect,false,_reusedRect.size);
}

bool Label::recordLetterInfo(const cocos2d::Point& point,const FontLetterDefinition& letterDef, int spriteIndex)
{
    if (static_cast<std::size_t>(spriteIndex) >= _lettersInfo.size())
    {
        LetterInfo tmpInfo;
        _lettersInfo.push_back(tmpInfo);
    }

    _lettersInfo[spriteIndex].def = letterDef;
    _lettersInfo[spriteIndex].position = point;
    _lettersInfo[spriteIndex].contentSize.width = _lettersInfo[spriteIndex].def.width;
    _lettersInfo[spriteIndex].contentSize.height = _lettersInfo[spriteIndex].def.height;
    _limitShowCount++;

    return _lettersInfo[spriteIndex].def.validDefinition;
}

bool Label::recordPlaceholderInfo(int spriteIndex)
{
    if (static_cast<std::size_t>(spriteIndex) >= _lettersInfo.size())
    {
        LetterInfo tmpInfo;
        _lettersInfo.push_back(tmpInfo);
    }

    _lettersInfo[spriteIndex].def.validDefinition = false;
    _limitShowCount++;

    return false;
}

void Label::addChild(Node * child, int zOrder/* =0 */, int tag/* =0 */)
{
    CCASSERT(0, "addChild: is not supported on Label.");
}

void Label::setLabelEffect(LabelEffect effect,const Color3B& effectColor)
{
    switch (effect)
    {
    case cocos2d::LabelEffect::NORMAL:
        disableEffect();
        break;
    case cocos2d::LabelEffect::OUTLINE:
        enableOutline(Color4B(effectColor));
        break;
    case cocos2d::LabelEffect::SHADOW:
        enableShadow(effectColor);
        break;
    case cocos2d::LabelEffect::GLOW:
        enableGlow(effectColor);
        break;
    default:
        break;
    }
}

void Label::enableGlow(const Color3B& glowColor)
{
    if(_useDistanceField == false)
        return;
    _currLabelEffect = LabelEffect::GLOW;
    _effectColor = glowColor;
    initProgram();
}

void Label::enableOutline(const Color4B& outlineColor,int outlineSize /* = 1 */)
{
    _outlineColor = outlineColor;

    if (outlineSize > 0)
    {
        if (_currentLabelType == LabelType::TTF)
        {
            if (_fontConfig.outlineSize != outlineSize)
            { 
                auto config = _fontConfig;
                config.outlineSize = outlineSize;
                setTTFConfig(config);
                initProgram();
            }
        }
        _fontDefinition._stroke._strokeEnabled = true;
        _fontDefinition._stroke._strokeSize = outlineSize;
        _fontDefinition._stroke._strokeColor = Color3B(outlineColor.r,outlineColor.g,outlineColor.b);

        _currLabelEffect = LabelEffect::OUTLINE;
        _contentDirty = true;
    }
}

void Label::enableShadow(const Color3B& shadowColor /* = Color3B::BLACK */,const Size &offset /* = Size(2 ,-2)*/, float opacity /* = 0.75f */, int blurRadius /* = 0 */)
{
    _shadowOpacity = opacity;
    _effectColor = shadowColor;
    _shadowOffset = offset;
    //todo:support blur for shadow
    _shadowBlurRadius = 0;
    _currLabelEffect = LabelEffect::SHADOW;

    _fontDefinition._shadow._shadowEnabled = true;
    _fontDefinition._shadow._shadowBlur = blurRadius;
    _fontDefinition._shadow._shadowOffset = offset;
    _fontDefinition._shadow._shadowOpacity = opacity;

    _contentDirty = true;
}

void Label::disableEffect()
{
    if (_currLabelEffect == LabelEffect::OUTLINE)
    {
        _fontConfig.outlineSize = 0;
        setTTFConfig(_fontConfig);
    }
    _currLabelEffect = LabelEffect::NORMAL;
    initProgram();
    _contentDirty = true;
}

void Label::setFontScale(float fontScale)
{
    _fontScale = fontScale;
    Node::setScale(_fontScale);
}

void Label::onDraw(const kmMat4& transform, bool transformUpdated)
{
    CC_PROFILER_START("Label - draw");

    // Optimization: Fast Dispatch
    if( _batchNodes.size() == 1 && _textureAtlas->getTotalQuads() == 0 )
    {
        return;
    }

    _shaderProgram->use();
    GL::blendFunc( _blendFunc.src, _blendFunc.dst );
    bool trans = false;

    if (_currLabelEffect == LabelEffect::OUTLINE)
    {
         _shaderProgram->setUniformLocationWith4f(_uniformEffectColor, _outlineColor.r/255.0f,_outlineColor.g/255.0f,_outlineColor.b/255.0f,_outlineColor.a/255.0f);
    }
    else if (_currLabelEffect == LabelEffect::GLOW)
    {
        _shaderProgram->setUniformLocationWith3f(_uniformEffectColor, _effectColor.r/255.0f,_effectColor.g/255.0f,_effectColor.b/255.0f);
    }
    else if(_currLabelEffect == LabelEffect::SHADOW && _shadowBlurRadius <= 0)
    {
        trans = true;
        drawShadowWithoutBlur();
    }

    _shaderProgram->setUniformsForBuiltins(transform);
   
    for(const auto &child: _children)
    {
        if(child->getTag() >= 0)
            child->updateTransform();
    }  

    for (const auto& batchNode:_batchNodes)
    {
        batchNode->getTextureAtlas()->drawQuads();
    }

    if (trans)
    {
        kmGLPopMatrix();
    }    

    CC_PROFILER_STOP("Label - draw");
}

void Label::drawShadowWithoutBlur()
{
    _position.x += _shadowOffset.width;
    _position.y += _shadowOffset.height;
    _transformDirty = _inverseDirty = true;
    Color3B oldColor = _realColor;
    GLubyte oldOPacity = _displayedOpacity;
    _displayedOpacity = _shadowOpacity * _displayedOpacity;
    setColor(_effectColor);

    _modelViewTransform = transform(_parentTransform);
    kmGLPushMatrix();
    kmGLLoadMatrix(&_modelViewTransform);

    _shaderProgram->setUniformsForBuiltins(_modelViewTransform);
    for(const auto &child: _children)
    {
        child->updateTransform();
    }
    for (const auto& batchNode:_batchNodes)
    {
        batchNode->getTextureAtlas()->drawQuads();
    }
    
    _position.x -= _shadowOffset.width;
    _position.y -= _shadowOffset.height;
    _transformDirty = _inverseDirty = true;
    _displayedOpacity = oldOPacity;
    setColor(oldColor);
    _modelViewTransform = transform(_parentTransform);
    kmGLLoadMatrix(&_modelViewTransform);
    //kmGLPopMatrix();
}

void Label::draw(Renderer *renderer, const kmMat4 &transform, bool transformUpdated)
{
    _customCommand.init(_globalZOrder);
    _customCommand.func = CC_CALLBACK_0(Label::onDraw, this, transform, transformUpdated);
    renderer->addCommand(&_customCommand);
}

void Label::createSpriteWithFontDefinition()
{
    _currentLabelType = LabelType::STRING_TEXTURE;
    auto texture = new Texture2D;
#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID) && (CC_TARGET_PLATFORM != CC_PLATFORM_IOS)
    if (_fontDefinition._shadow._shadowEnabled || _fontDefinition._stroke._strokeEnabled)
    {
        CCLOGERROR("Currently only supported on iOS and Android!");
    }
    _fontDefinition._shadow._shadowEnabled = false;
    _fontDefinition._stroke._strokeEnabled = false;
#endif
    texture->initWithString(_originalUTF8String.c_str(),_fontDefinition);

    _textSprite = Sprite::createWithTexture(texture);
    _textSprite->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    this->setContentSize(_textSprite->getContentSize());
    texture->release();

    Node::addChild(_textSprite,0,Node::INVALID_TAG);
}

void Label::updateContent()
{
    auto utf16String = cc_utf8_to_utf16(_originalUTF8String.c_str());
    setCurrentString(utf16String);
    setOriginalString(utf16String);
    if (_textSprite)
    {
        _textSprite->removeFromParent();
        _textSprite = nullptr;
    }
    if (_fontAtlas)
    {
        alignText();
    }
    else
    {
        createSpriteWithFontDefinition();
    }
    _contentDirty = false;
}

void Label::visit(Renderer *renderer, const kmMat4 &parentTransform, bool parentTransformUpdated)
{
    if (! _visible)
    {
        return;
    }
    if (_contentDirty)
    {
        updateContent();
    }

    if (! _textSprite && _currLabelEffect == LabelEffect::SHADOW && _shadowBlurRadius <= 0)
    {
        _parentTransform = parentTransform;
        draw(renderer, _modelViewTransform, true);
    }
    else
    {
        bool dirty = parentTransformUpdated || _transformUpdated;
        
        if(dirty)
            _modelViewTransform = transform(parentTransform);
        _transformUpdated = false;

        // IMPORTANT:
        // To ease the migration to v3.0, we still support the kmGL stack,
        // but it is deprecated and your code should not rely on it
        kmGLPushMatrix();
        kmGLLoadMatrix(&_modelViewTransform);

        if (_textSprite)
        {
            _textSprite->visit();
        }
        else
        {
            draw(renderer, _modelViewTransform, dirty);
        }

        kmGLPopMatrix();
    }
    
    setOrderOfArrival(0);
}

void Label::setFontName(const std::string& fontName)
{
    _fontDefinition._fontName = fontName;
    if (fontName.find('.') != fontName.npos)
    {
        auto config = _fontConfig;
        config.fontFilePath = fontName;
        setTTFConfig(config);
    }
    _contentDirty = true;
}

void Label::setFontSize(int fontSize)
{
    if (_currentLabelType == LabelType::TTF)
    {
        if (_fontConfig.distanceFieldEnabled == true)
        {
            _fontConfig.fontSize = fontSize;
            this->setFontScale(1.0f * fontSize / DefultFontSize);
        }
        else
        {
            auto fontConfig = _fontConfig;
            fontConfig.fontSize = fontSize;
            setTTFConfig(fontConfig);
        }
    }
    else
    {
        _fontDefinition._fontSize = fontSize;
        _fontConfig.fontSize = fontSize;
        _contentDirty = true;
    }
}

///// PROTOCOL STUFF
Sprite * Label::getLetter(int lettetIndex)
{
    if (_contentDirty)
    {
        updateContent();
    }
    
    if (! _textSprite && lettetIndex < _limitShowCount)
    {
        if(_lettersInfo[lettetIndex].def.validDefinition == false)
            return nullptr;

        Sprite* sp = static_cast<Sprite*>(this->getChildByTag(lettetIndex));

        if (!sp)
        {
            Rect uvRect;
            uvRect.size.height = _lettersInfo[lettetIndex].def.height;
            uvRect.size.width  = _lettersInfo[lettetIndex].def.width;
            uvRect.origin.x    = _lettersInfo[lettetIndex].def.U;
            uvRect.origin.y    = _lettersInfo[lettetIndex].def.V;

            sp = Sprite::createWithTexture(_fontAtlas->getTexture(_lettersInfo[lettetIndex].def.textureID),uvRect);
            sp->setBatchNode(this);
            sp->setAnchorPoint(Point::ANCHOR_MIDDLE);
            sp->setPosition(Point(_lettersInfo[lettetIndex].position.x+uvRect.size.width/2,_lettersInfo[lettetIndex].position.y-uvRect.size.height/2));
            sp->setOpacity(_realOpacity);

            this->addSpriteWithoutQuad(sp, lettetIndex, lettetIndex);
        }
        return sp;
    }

    return nullptr;
}

int Label::getCommonLineHeight() const
{
    return _textSprite ? 0 : _commonLineHeight;
}

void Label::computeStringNumLines()
{
    int quantityOfLines = 1;

    unsigned int stringLen = _currentUTF16String ? cc_wcslen(_currentUTF16String) : -1;
    if (stringLen < 1)
    {
        _currNumLines = stringLen;
        return;
    }

    // count number of lines
    for (unsigned int i = 0; i < stringLen - 1; ++i)
    {
        if (_currentUTF16String[i] == '\n')
        {
            quantityOfLines++;
        }
    }

    _currNumLines = quantityOfLines;
}

int Label::getStringLength() const
{
    return _currentUTF16String ? cc_wcslen(_currentUTF16String) : 0;
}

// RGBA protocol
bool Label::isOpacityModifyRGB() const
{
    return _isOpacityModifyRGB;
}

void Label::setOpacityModifyRGB(bool isOpacityModifyRGB)
{
    _isOpacityModifyRGB = isOpacityModifyRGB;

    for(const auto& child: _children) {
        child->setOpacityModifyRGB(_isOpacityModifyRGB);
    }

    _reusedLetter->setOpacityModifyRGB(true);
}

void Label::setColor(const Color3B& color)
{
    _fontDefinition._fontFillColor = color;
    if (_textSprite)
    {
        updateContent();
    }
    _reusedLetter->setColor(color);
    SpriteBatchNode::setColor(color);
}

void Label::updateColor()
{
    if (nullptr == _textureAtlas)
    {
        return;
    }

    Color4B color4( _displayedColor.r, _displayedColor.g, _displayedColor.b, _displayedOpacity );

    // special opacity for premultiplied textures
    if (_isOpacityModifyRGB)
    {
        color4.r *= _displayedOpacity/255.0f;
        color4.g *= _displayedOpacity/255.0f;
        color4.b *= _displayedOpacity/255.0f;
    }

    cocos2d::TextureAtlas* textureAtlas;
    V3F_C4B_T2F_Quad *quads;
    for (const auto& batchNode:_batchNodes)
    {
        textureAtlas = batchNode->getTextureAtlas();
        quads = textureAtlas->getQuads();
        auto count = textureAtlas->getTotalQuads();

        for (int index = 0; index < count; ++index)
        {
            quads[index].bl.colors = color4;
            quads[index].br.colors = color4;
            quads[index].tl.colors = color4;
            quads[index].tr.colors = color4;
            textureAtlas->updateQuad(&quads[index], index);
        }
    }
}

std::string Label::getDescription() const
{
    return StringUtils::format("<Label | Tag = %d, Label = '%s'>", _tag, cc_utf16_to_utf8(_currentUTF16String,-1,nullptr,nullptr));
}

const Size& Label::getContentSize()
{
    if (_contentDirty)
    {
        updateContent();
    }
    return Node::getContentSize();
}
NS_CC_END
