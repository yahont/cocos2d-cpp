local    kTagMenu = 1
local    kTagMenu0 = 0
local    kTagMenu1 = 1

local MID_CALLBACK     = 1000
local MID_CALLBACK2    = 1001
local MID_DISABLED     = 1002
local MID_ENABLE       = 1003
local MID_CONFIG       = 1004
local MID_QUIT         = 1005
local MID_OPACITY      = 1006
local MID_ALIGN        = 1007
local MID_CALLBACK3    = 1008
local MID_BACKCALLBACK = 1009

--------------------------------------------------------------------
--
-- MenuLayerMainMenu
--
--------------------------------------------------------------------
local function MenuLayerMainMenu()
    local m_disabledItem = nil

    local ret = cc.Layer:create()
    ret:setTouchEnabled(true)
    ret:setTouchPriority(cc.MENU_HANDLER_PRIORITY  + 1)
    ret:setTouchMode(cc.TOUCHES_ONE_BY_ONE )

    -- Font Item
    local  spriteNormal = cc.Sprite:create(s_MenuItem, cc.rect(0,23*2,115,23))
    local  spriteSelected = cc.Sprite:create(s_MenuItem, cc.rect(0,23*1,115,23))
    local  spriteDisabled = cc.Sprite:create(s_MenuItem, cc.rect(0,23*0,115,23))

    local  item1 = cc.MenuItemSprite:create(spriteNormal, spriteSelected, spriteDisabled)

    local function menuCallback(sender)
        cclog("menuCallback...")
        tolua.cast(ret:getParent(), "LayerMultiplex"):switchTo(1)
    end

    item1:registerScriptTapHandler(menuCallback)
    -- Image Item
    local function menuCallback2(sender)
        tolua.cast(ret:getParent(), "LayerMultiplex"):switchTo(2)
    end

    local  item2 = cc.MenuItemImage:create(s_SendScore, s_PressSendScore)
    item2:registerScriptTapHandler(menuCallback2)


    local schedulerEntry = nil
	local scheduler = cc.Director:getInstance():getScheduler()
    local function allowTouches(dt)
        local  pDirector = cc.Director:getInstance()
        --pDirector:getTouchDispatcher():setPriority(cc.MENU_HANDLER_PRIORITY +1, ret)
        if nil ~=  schedulerEntry then 
            scheduler:unscheduleScriptEntry(schedulerEntry)
            schedulerEntry = nil
        end
        cclog("TOUCHES ALLOWED AGAIN")
    end


    local function menuCallbackDisabled(sender)
        -- hijack all touch events for 5 seconds
        local  pDirector = cc.Director:getInstance()
        --pDirector:getTouchDispatcher():setPriority(cc.MENU_HANDLER_PRIORITY -1, ret)
        schedulerEntry = scheduler:scheduleScriptFunc(allowTouches, 5, false)
        cclog("TOUCHES DISABLED FOR 5 SECONDS")
    end

    -- Label Item (LabelAtlas)
    local  labelAtlas = cc.LabelAtlas:_create("0123456789", "fonts/labelatlas.png", 16, 24, string.byte('.'))
    local  item3 = cc.MenuItemLabel:create(labelAtlas)
    item3:registerScriptTapHandler(menuCallbackDisabled)
    item3:setDisabledColor( cc.c3b(32,32,64) )
    item3:setColor( cc.c3b(200,200,255) )

    local function menuCallbackEnable(sender)
        m_disabledItem:setEnabled(not m_disabledItem:isEnabled() )
    end

    -- Font Item
    local item4 = cc.MenuItemFont:create("I toggle enable items")
    item4:registerScriptTapHandler(menuCallbackEnable)

    item4:setFontSizeObj(20)
    cc.MenuItemFont:setFontName("Marker Felt")

    local function menuCallbackConfig(sender)
        tolua.cast(ret:getParent(), "LayerMultiplex"):switchTo(3)
    end

    -- Label Item (cc.LabelBMFont)
    local  label = cc.LabelBMFont:create("configuration", "fonts/bitmapFontTest3.fnt")
    local  item5 = cc.MenuItemLabel:create(label)
    item5:registerScriptTapHandler(menuCallbackConfig)

    -- Testing issue #500
    item5:setScale( 0.8 )

    local function menuCallbackPriorityTest(pSender)
        tolua.cast(ret:getParent(), "LayerMultiplex"):switchTo(4)
    end

    -- Events
    cc.MenuItemFont:setFontName("Marker Felt")
    local item6 = cc.MenuItemFont:create("Priority Test")
    item6:registerScriptTapHandler(menuCallbackPriorityTest)

    local function menuCallbackBugsTest(pSender)
        tolua.cast(ret:getParent(), "LayerMultiplex"):switchTo(5)
    end

    -- Bugs Item
    local item7 = cc.MenuItemFont:create("Bugs")
    item7:registerScriptTapHandler(menuCallbackBugsTest)

    local function onQuit(sender)
        cclog("onQuit item is clicked.")
    end

    -- Font Item
    local  item8 = cc.MenuItemFont:create("Quit")
    item8:registerScriptTapHandler(onQuit)

    local function menuMovingCallback(pSender)
        tolua.cast(ret:getParent(), "LayerMultiplex"):switchTo(6)
    end

    local  item9 = cc.MenuItemFont:create("Remove menu item when moving")
    item9:registerScriptTapHandler(menuMovingCallback)

    local  color_action = cc.TintBy:create(0.5, 0, -255, -255)
    local  color_back = color_action:reverse()
    local  seq = cc.Sequence:create(color_action, color_back)
    item8:runAction(cc.RepeatForever:create(seq))

    local  menu = cc.Menu:create()

    menu:addChild(item1)
    menu:addChild(item2)
    menu:addChild(item3)
    menu:addChild(item4)
    menu:addChild(item5)
    menu:addChild(item6)
    menu:addChild(item7)
    menu:addChild(item8)
    menu:addChild(item9)

    menu:alignItemsVertically()

    -- elastic effect
    local s = cc.Director:getInstance():getWinSize()

    local i        = 0
    local child    = nil
    local pArray   = menu:getChildren()
    local len      = table.getn(pArray)
    local pObject  = nil
    for i = 0, len-1 do
        pObject = pArray[i + 1]
        if pObject == nil then
            break
        end
        child = tolua.cast(pObject, "Node")
        local dstPointX, dstPointY = child:getPosition()
        local offset = s.width/2 + 50
        if  i % 2 == 0 then
            offset = 0-offset
        end
        child:setPosition( cc.p( dstPointX + offset, dstPointY) )
        child:runAction( cc.EaseElasticOut:create(cc.MoveBy:create(2, cc.p(dstPointX - offset,0)), 0.35) )
    end

    m_disabledItem = item3
    item3:retain()

    m_disabledItem:setEnabled( false )

    ret:addChild(menu)
    menu:setPosition(cc.p(s.width/2, s.height/2))

--  local schedulerEntry = nil
    local function onNodeEvent(event)
        if event == "exit" then
            if (schedulerEntry ~= nil) then
                scheduler:unscheduleScriptEntry(schedulerEntry)
            end
            if m_disabledItem ~= nil then
--                m_disabledItem:release()
            end
        end
    end

    ret:registerScriptHandler(onNodeEvent)

    return ret
end

--------------------------------------------------------------------
--
-- MenuLayer2
--
--------------------------------------------------------------------
local function MenuLayer2()
    local ret = cc.Layer:create()
    local m_centeredMenu = nil
    local m_alignedH = false

    local function alignMenusH()
        local i = 0
        for i=0, 1 do
            local menu = tolua.cast(ret:getChildByTag(100+i), "Menu")
            menu:setPosition( m_centeredMenu )
            if i==0 then
                -- TIP: if no padding, padding = 5
                menu:alignItemsHorizontally()
                local x, y = menu:getPosition()
                menu:setPosition( cc.pAdd(cc.p(x, y), cc.p(0,30)) )
            else
                -- TIP: but padding is configurable
                menu:alignItemsHorizontallyWithPadding(40)
                local x, y = menu:getPosition()
                menu:setPosition( cc.pSub(cc.p(x, y), cc.p(0,30)) )
            end
        end
    end

    local function alignMenusV()
        local i = 0
        for i=0, 1 do
            local menu = tolua.cast(ret:getChildByTag(100+i), "Menu")
            menu:setPosition( m_centeredMenu )
            if i==0 then
                -- TIP: if no padding, padding = 5
                menu:alignItemsVertically()
                local x, y = menu:getPosition()
                menu:setPosition( cc.pAdd(cc.p(x, y), cc.p(100,0)) )
            else
                -- TIP: but padding is configurable
                menu:alignItemsVerticallyWithPadding(40)
                local x, y = menu:getPosition()
                menu:setPosition( cc.pSub(cc.p(x, y), cc.p(100,0)) )
            end
        end
    end

    local function menuCallback(sender)
        tolua.cast(ret:getParent(), "LayerMultiplex"):switchTo(0)
    end

    local function menuCallbackOpacity(tag, sender)
        local menu = tolua.cast(sender:getParent(), "Menu")
        local opacity = menu:getOpacity()
        if opacity == 128 then
            menu:setOpacity(255)
        else
            menu:setOpacity(128)
        end
    end

    local function menuCallbackAlign(sender)
        m_alignedH = not m_alignedH

        if m_alignedH then
            alignMenusH()
        else
            alignMenusV()
        end
    end

    local i = 0
    for i=0, 1 do
        local  item1 = cc.MenuItemImage:create(s_PlayNormal, s_PlaySelect)
        item1:registerScriptTapHandler(menuCallback)

        local  item2 = cc.MenuItemImage:create(s_HighNormal, s_HighSelect)
        item2:registerScriptTapHandler(menuCallbackOpacity)

        local  item3 = cc.MenuItemImage:create(s_AboutNormal, s_AboutSelect)
        item3:registerScriptTapHandler(menuCallbackAlign)

        item1:setScaleX( 1.5 )
        item2:setScaleX( 0.5 )
        item3:setScaleX( 0.5 )

        local  menu = cc.Menu:create()

        menu:addChild(item1)
        menu:addChild(item2)
        menu:addChild(item3)

        local s = cc.Director:getInstance():getWinSize()
        menu:setPosition(cc.p(s.width/2, s.height/2))

        menu:setTag( kTagMenu )

        ret:addChild(menu, 0, 100+i)
        local x, y = menu:getPosition()
        m_centeredMenu = cc.p(x, y)
    end

    m_alignedH = true
    alignMenusH()

    return ret
end

--------------------------------------------------------------------
--
-- MenuLayer3
--
--------------------------------------------------------------------
local function MenuLayer3()
    local m_disabledItem = nil
    local ret = cc.Layer:create()
    local function menuCallback(sender)
        tolua.cast(ret:getParent(), "LayerMultiplex"):switchTo(0)
    end

    local function menuCallback2(sender)
        cclog("Label clicked. Toogling AtlasSprite")
        m_disabledItem:setEnabled( not m_disabledItem:isEnabled() )
        m_disabledItem:stopAllActions()
    end

    local function menuCallback3(sender)
        cclog("MenuItemSprite clicked")
    end

    cc.MenuItemFont:setFontName("Marker Felt")
    cc.MenuItemFont:setFontSize(28)

    local  label = cc.LabelBMFont:create("Enable AtlasItem", "fonts/bitmapFontTest3.fnt")
    local  item1 = cc.MenuItemLabel:create(label)
    item1:registerScriptTapHandler(menuCallback2)

    local  item2 = cc.MenuItemFont:create("--- Go Back ---")
    item2:registerScriptTapHandler(menuCallback)

    local spriteNormal   = cc.Sprite:create(s_MenuItem,  cc.rect(0,23*2,115,23))
    local spriteSelected = cc.Sprite:create(s_MenuItem,  cc.rect(0,23*1,115,23))
    local spriteDisabled = cc.Sprite:create(s_MenuItem,  cc.rect(0,23*0,115,23))


    local  item3 = cc.MenuItemSprite:create(spriteNormal, spriteSelected, spriteDisabled)
    item3:registerScriptTapHandler(menuCallback3)
    m_disabledItem = item3
    item3:retain()
    m_disabledItem:setEnabled( false )

    local menu = cc.Menu:create()

    menu:addChild(item1)
    menu:addChild(item2)
    menu:addChild(item3)

    menu:setPosition( cc.p(0,0) )

    local s = cc.Director:getInstance():getWinSize()

    item1:setPosition( cc.p(s.width/2 - 150, s.height/2) )
    item2:setPosition( cc.p(s.width/2 - 200, s.height/2) )
    item3:setPosition( cc.p(s.width/2, s.height/2 - 100) )

    local  jump = cc.JumpBy:create(3, cc.p(400,0), 50, 4)
    item2:runAction( cc.RepeatForever:create(cc.Sequence:create( jump, jump:reverse())))

    local  spin1 = cc.RotateBy:create(3, 360)
    local  spin2 = tolua.cast(spin1:clone(), "ActionInterval")
    local  spin3 = tolua.cast(spin1:clone(), "ActionInterval")

    item1:runAction( cc.RepeatForever:create(spin1) )
    item2:runAction( cc.RepeatForever:create(spin2) )
    item3:runAction( cc.RepeatForever:create(spin3) )

    ret:addChild( menu )

    menu:setPosition(cc.p(0,0))

    local function onNodeEvent(event)
        if event == "exit" then
            if m_disabledItem ~= nil then
                --m_disabledItem:release()
            end
        end
    end

    ret:registerScriptHandler(onNodeEvent)

    return ret
end


--------------------------------------------------------------------
--
-- MenuLayer4
--
--------------------------------------------------------------------
local function MenuLayer4()
    local ret = cc.Layer:create()
    cc.MenuItemFont:setFontName("American Typewriter")
    cc.MenuItemFont:setFontSize(18)
    local title1 = cc.MenuItemFont:create("Sound")
    title1:setEnabled(false)
    cc.MenuItemFont:setFontName( "Marker Felt" )
    cc.MenuItemFont:setFontSize(34)
    local  item1 = cc.MenuItemToggle:create(cc.MenuItemFont:create( "On" ))

    local function menuCallback(tag, sender)
        cclog("selected item: tag: %d, index:%d", tag, tolua.cast(sender, "MenuItemToggle"):getSelectedIndex() )
    end

    local function backCallback(tag, sender)
        tolua.cast(ret:getParent(), "LayerMultiplex"):switchTo(0)
    end

    item1:registerScriptTapHandler(menuCallback)
    item1:addSubItem(cc.MenuItemFont:create( "Off"))

    cc.MenuItemFont:setFontName( "American Typewriter" )
    cc.MenuItemFont:setFontSize(18)
    local  title2 = cc.MenuItemFont:create( "Music" )
    title2:setEnabled(false)
    cc.MenuItemFont:setFontName( "Marker Felt" )
    cc.MenuItemFont:setFontSize(34)
    local item2 = cc.MenuItemToggle:create(cc.MenuItemFont:create( "On" ))
    item2:registerScriptTapHandler(menuCallback)
    item2:addSubItem(cc.MenuItemFont:create( "Off"))

    cc.MenuItemFont:setFontName( "American Typewriter" )
    cc.MenuItemFont:setFontSize(18)
    local  title3 = cc.MenuItemFont:create( "Quality" )
    title3:setEnabled( false )
    cc.MenuItemFont:setFontName( "Marker Felt" )
    cc.MenuItemFont:setFontSize(34)
    local item3 = cc.MenuItemToggle:create(cc.MenuItemFont:create( "High" ))
    item3:registerScriptTapHandler(menuCallback)
    item3:addSubItem(cc.MenuItemFont:create( "Low" ))

    cc.MenuItemFont:setFontName( "American Typewriter" )
    cc.MenuItemFont:setFontSize(18)
    local  title4 = cc.MenuItemFont:create( "Orientation" )
    title4:setEnabled(false)
    cc.MenuItemFont:setFontName( "Marker Felt" )
    cc.MenuItemFont:setFontSize(34)
    local item4 = cc.MenuItemToggle:create(cc.MenuItemFont:create( "Off"),
                                          cc.MenuItemFont:create( "33%" ),
                                          cc.MenuItemFont:create( "66%" ),
                                          cc.MenuItemFont:create( "100%"))
    item4:registerScriptTapHandler(menuCallback)

    -- you can change the one of the items by doing this
    item4:setSelectedIndex( 2 )

    cc.MenuItemFont:setFontName( "Marker Felt" )
    cc.MenuItemFont:setFontSize( 34 )

    local label = cc.LabelBMFont:create( "go back", "fonts/bitmapFontTest3.fnt" )
    local  back = cc.MenuItemLabel:create(label)
    back:registerScriptTapHandler(backCallback)

    local menu = cc.Menu:create()

    menu:addChild(title1)
    menu:addChild(title2)
    menu:addChild(item1 )
    menu:addChild(item2 )
    menu:addChild(title3)
    menu:addChild(title4)
    menu:addChild(item3 )
    menu:addChild(item4 )
    menu:addChild(back  )

    menu:alignItemsInColumns(2, 2, 2, 2, 1)

    ret:addChild(menu)

    local s = cc.Director:getInstance():getWinSize()
    menu:setPosition(cc.p(s.width/2, s.height/2))
    return ret
end

local function MenuLayerPriorityTest()
    local ret = cc.Layer:create()
    local m_bPriority = false
    -- Testing empty menu
    local m_pMenu1 = cc.Menu:create()
    local m_pMenu2 = cc.Menu:create()

    local function menuCallback(tag, pSender)
        tolua.cast(ret:getParent(), "LayerMultiplex"):switchTo(0)
    end

    local function enableMenuCallback()
        m_pMenu1:setEnabled(true)
    end

    local function disableMenuCallback(tag, pSender)
        m_pMenu1:setEnabled(false)
        local wait = cc.DelayTime:create(5)
        local enable = cc.CallFunc:create(enableMenuCallback)
        local  seq = cc.Sequence:create(wait, enable)
        m_pMenu1:runAction(seq)
    end

    local function togglePriorityCallback(tag, pSender)
        if m_bPriority then
            m_pMenu2:setHandlerPriority(cc.MENU_HANDLER_PRIORITY  + 20)
            m_bPriority = false
        else
            m_pMenu2:setHandlerPriority(cc.MENU_HANDLER_PRIORITY  - 20)
            m_bPriority = true
        end
    end

    -- Menu 1
    cc.MenuItemFont:setFontName("Marker Felt")
    cc.MenuItemFont:setFontSize(18)
    local item1 = cc.MenuItemFont:create("Return to Main Menu")
    item1:registerScriptTapHandler(menuCallback)
    local item2 = cc.MenuItemFont:create("Disable menu for 5 seconds")
    item2:registerScriptTapHandler(disableMenuCallback)

    m_pMenu1:addChild(item1)
    m_pMenu1:addChild(item2)

    m_pMenu1:alignItemsVerticallyWithPadding(20)

    ret:addChild(m_pMenu1)

    -- Menu 2
    m_bPriority = true
    cc.MenuItemFont:setFontSize(48)
    item1 = cc.MenuItemFont:create("Toggle priority")
    item2:registerScriptTapHandler(togglePriorityCallback)
    item1:setColor(cc.c3b(0,0,255))
    m_pMenu2:addChild(item1)
    ret:addChild(m_pMenu2)
    return ret
end


-- BugsTest
local function BugsTest()
    local ret = cc.Layer:create()
    local function issue1410MenuCallback(tag, pSender)
        local menu = tolua.cast(pSender:getParent(), "Menu")
        menu:setTouchEnabled(false)
        menu:setTouchEnabled(true)
        cclog("NO CRASHES")
    end

    local function issue1410v2MenuCallback(tag, pSender)
        local menu = tolua.cast(pSender:getParent(), "Menu")
        menu:setTouchEnabled(true)
        menu:setTouchEnabled(false)
        cclog("NO CRASHES. AND MENU SHOULD STOP WORKING")
    end

    local function backMenuCallback(tag, pSender)
        tolua.cast(ret:getParent(), "LayerMultiplex"):switchTo(0)
    end


    local issue1410 = cc.MenuItemFont:create("Issue 1410")
    issue1410:registerScriptTapHandler(issue1410MenuCallback)
    local issue1410_2 = cc.MenuItemFont:create("Issue 1410 #2")
    issue1410_2:registerScriptTapHandler(issue1410v2MenuCallback)
    local back = cc.MenuItemFont:create("Back")
    back:registerScriptTapHandler(backMenuCallback)

    local menu = cc.Menu:create()
    menu:addChild(issue1410)
    menu:addChild(issue1410_2)
    menu:addChild(back)
    ret:addChild(menu)
    menu:alignItemsVertically()

    local s = cc.Director:getInstance():getWinSize()
    menu:setPosition(cc.p(s.width/2, s.height/2))
    return ret
end


local function RemoveMenuItemWhenMove()
    local ret = cc.Layer:create()
    local s = cc.Director:getInstance():getWinSize()

    local  label = cc.LabelTTF:create("click item and move, should not crash", "Arial", 20)
    label:setPosition(cc.p(s.width/2, s.height - 30))
    ret:addChild(label)

    local item = cc.MenuItemFont:create("item 1")
    item:retain()

    local back = cc.MenuItemFont:create("go back")
    local function goBack(tag, pSender)
        tolua.cast(ret:getParent(), "LayerMultiplex"):switchTo(0)
    end

    back:registerScriptTapHandler(goBack)

    local menu = cc.Menu:create()
    menu:addChild(item)
    menu:addChild(back)

    ret:addChild(menu)
    menu:alignItemsVertically()

    menu:setPosition(cc.p(s.width/2, s.height/2))

    ret:setTouchEnabled(true)
--[[
    local function onNodeEvent(event)
        if event == "enter" then
            cc.Director:getInstance():getTouchDispatcher():addTargetedDelegate(ret, -129, false)
        elseif event == "exit" then
           -- item:release()
        end
    end

    ret:registerScriptHandler(onNodeEvent)
    ]]--

    local function onTouchEvent(eventType, x, y)
        if eventType == "began" then
            return true
        elseif  eventType == "moved" then
            if item ~= nil then
                item:removeFromParent(true)
                --item:release()
                --item = nil
            end
        end
    end

    ret:registerScriptTouchHandler(onTouchEvent,false,-129,false)
    return ret
end

function MenuTestMain()
    cclog("MenuTestMain")
    local scene = cc.Scene:create()

    local  pLayer1 = MenuLayerMainMenu()
    local  pLayer2 = MenuLayer2()

    local  pLayer3 = MenuLayer3()
    local  pLayer4 = MenuLayer4()
    local  pLayer5 = MenuLayerPriorityTest()
    local  pLayer6 = BugsTest()
    local  pLayer7 = RemoveMenuItemWhenMove()

    local  layer = cc.LayerMultiplex:create(pLayer1,
                                            pLayer2,
                                            pLayer3,
                                            pLayer4,
                                            pLayer5,
                                            pLayer6,
                                            pLayer7)

    scene:addChild(layer, 0)
    scene:addChild(CreateBackMenuItem())
    return scene
end
