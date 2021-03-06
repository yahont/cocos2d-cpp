#include "TableViewTestScene.h"
#include "CustomTableViewCell.h"
#include "../ExtensionsTest.h"

#include "base/CCDirector.h"
#include "base/ccUTF8.h"

using namespace cocos2d;
using namespace cocos2d::extension;

TableViewTests::TableViewTests()
{
    ADD_TEST_CASE(TableViewTest);
}

// on "init" you need to initialize your instance
bool TableViewTest::init()
{
    if ( !TestCase::init() )
    {
        return false;
    }

	Size winSize = Director::getInstance()->getWinSize();

    TableView* tableView = TableView::create(this, Size(250, 60));
    tableView->setDirection(ScrollView::Direction::HORIZONTAL);
    tableView->setPosition(Vec2(20,winSize.height/2-30));
    tableView->setDelegate(this);
    this->addChild(tableView);
    tableView->reloadData();

    auto testNode = Node::create();
    testNode->setName("testNode");
    tableView->addChild(testNode);
    tableView->removeChild(testNode, true);
    CCASSERT(nullptr == tableView->getChildByName("testNode"), "The added child has been removed!");


	tableView = TableView::create(this, Size(60, 250));
	tableView->setDirection(ScrollView::Direction::VERTICAL);
	tableView->setPosition(Vec2(winSize.width-150,winSize.height/2-120));
	tableView->setDelegate(this);
	tableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
	this->addChild(tableView);
	tableView->reloadData();

    return true;
}

void TableViewTest::tableCellTouched(TableView *, TableViewCell* cell)
{
    CCLOG("cell touched at index: %ld", static_cast<long>(cell->getIdx()));
}

Size TableViewTest::tableCellSizeForIndex(TableView *, ssize_t idx)
{
    if (idx == 2) {
        return Size(100, 100);
    }
    return Size(60, 60);
}

TableViewCell* TableViewTest::tableCellAtIndex(TableView *table, ssize_t idx)
{
    auto string = StringUtils::format("%ld", static_cast<long>(idx));
    TableViewCell *cell = table->dequeueCell();
    if (!cell) {
        cell = new (std::nothrow) CustomTableViewCell();
        cell->autorelease();
        auto sprite = Sprite::create("Images/Icon.png");
        sprite->setAnchorPoint(Vec2::ZERO);
        sprite->setPosition(Vec2(0, 0));
        cell->addChild(sprite);

        auto label = Label::createWithSystemFont(string, "Helvetica", 20.0);
        label->setPosition(Vec2::ZERO);
		label->setAnchorPoint(Vec2::ZERO);
        label->setTag(123);
        cell->addChild(label);
    }
    else
    {
        auto label = (Label*)cell->getChildByTag(123);
        label->setString(string);
    }


    return cell;
}

ssize_t TableViewTest::numberOfCellsInTableView(TableView *)
{
    return 20;
}
