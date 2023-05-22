// This file is generated by kconfig_compiler_kf5 from breeze.kcfg.
// All changes you do to this file will be lost.

#include "breezestyleconfigdata.h"

#include <qglobal.h>
#include <QFile>

using namespace Breeze;

namespace Breeze {

class StyleConfigDataHelper
{
  public:
    StyleConfigDataHelper() : q(nullptr) {}
    ~StyleConfigDataHelper() { delete q; q = nullptr; }
    StyleConfigDataHelper(const StyleConfigDataHelper&) = delete;
    StyleConfigDataHelper& operator=(const StyleConfigDataHelper&) = delete;
    StyleConfigData *q;
};

}
Q_GLOBAL_STATIC(StyleConfigDataHelper, s_globalStyleConfigData)
StyleConfigData *StyleConfigData::self()
{
  if (!s_globalStyleConfigData()->q) {
    new StyleConfigData;
    s_globalStyleConfigData()->q->read();
  }

  return s_globalStyleConfigData()->q;
}

StyleConfigData::StyleConfigData( )
  : KConfigSkeleton( QStringLiteral( "breezerc" ) )
{
  Q_ASSERT(!s_globalStyleConfigData()->q);
  s_globalStyleConfigData()->q = this;
  setCurrentGroup( QStringLiteral( "Common" ) );

  KConfigSkeleton::ItemInt  *itemShadowStrength;
  itemShadowStrength = new KConfigSkeleton::ItemInt( currentGroup(), QStringLiteral( "ShadowStrength" ), mShadowStrength, 255 );
  itemShadowStrength->setMinValue(25);
  itemShadowStrength->setMaxValue(255);
  addItem( itemShadowStrength, QStringLiteral( "ShadowStrength" ) );
  QList<KConfigSkeleton::ItemEnum::Choice> valuesShadowSize;
  {
    KConfigSkeleton::ItemEnum::Choice choice;
    choice.name = QStringLiteral("ShadowNone");
    valuesShadowSize.append( choice );
  }
  {
    KConfigSkeleton::ItemEnum::Choice choice;
    choice.name = QStringLiteral("ShadowSmall");
    valuesShadowSize.append( choice );
  }
  {
    KConfigSkeleton::ItemEnum::Choice choice;
    choice.name = QStringLiteral("ShadowMedium");
    valuesShadowSize.append( choice );
  }
  {
    KConfigSkeleton::ItemEnum::Choice choice;
    choice.name = QStringLiteral("ShadowLarge");
    valuesShadowSize.append( choice );
  }
  {
    KConfigSkeleton::ItemEnum::Choice choice;
    choice.name = QStringLiteral("ShadowVeryLarge");
    valuesShadowSize.append( choice );
  }
  KConfigSkeleton::ItemEnum  *itemShadowSize;
  itemShadowSize = new KConfigSkeleton::ItemEnum( currentGroup(), QStringLiteral( "ShadowSize" ), mShadowSize, valuesShadowSize, ShadowLarge );
  addItem( itemShadowSize, QStringLiteral( "ShadowSize" ) );
  KConfigSkeleton::ItemColor  *itemShadowColor;
  itemShadowColor = new KConfigSkeleton::ItemColor( currentGroup(), QStringLiteral( "ShadowColor" ), mShadowColor, QColor( 0, 0, 0 ) );
  addItem( itemShadowColor, QStringLiteral( "ShadowColor" ) );
  KConfigSkeleton::ItemBool  *itemOutlineCloseButton;
  itemOutlineCloseButton = new KConfigSkeleton::ItemBool( currentGroup(), QStringLiteral( "OutlineCloseButton" ), mOutlineCloseButton, false );
  addItem( itemOutlineCloseButton, QStringLiteral( "OutlineCloseButton" ) );

  setCurrentGroup( QStringLiteral( "Style" ) );

  KConfigSkeleton::ItemBool  *itemAnimationsEnabled;
  itemAnimationsEnabled = new KConfigSkeleton::ItemBool( currentGroup(), QStringLiteral( "AnimationsEnabled" ), mAnimationsEnabled, true );
  addItem( itemAnimationsEnabled, QStringLiteral( "AnimationsEnabled" ) );
  KConfigSkeleton::ItemInt  *itemAnimationSteps;
  itemAnimationSteps = new KConfigSkeleton::ItemInt( currentGroup(), QStringLiteral( "AnimationSteps" ), mAnimationSteps, 10 );
  addItem( itemAnimationSteps, QStringLiteral( "AnimationSteps" ) );
  KConfigSkeleton::ItemInt  *itemAnimationsDuration;
  itemAnimationsDuration = new KConfigSkeleton::ItemInt( currentGroup(), QStringLiteral( "AnimationsDuration" ), mAnimationsDuration, 100 );
  addItem( itemAnimationsDuration, QStringLiteral( "AnimationsDuration" ) );
  KConfigSkeleton::ItemBool  *itemStackedWidgetTransitionsEnabled;
  itemStackedWidgetTransitionsEnabled = new KConfigSkeleton::ItemBool( currentGroup(), QStringLiteral( "StackedWidgetTransitionsEnabled" ), mStackedWidgetTransitionsEnabled, false );
  addItem( itemStackedWidgetTransitionsEnabled, QStringLiteral( "StackedWidgetTransitionsEnabled" ) );
  KConfigSkeleton::ItemBool  *itemProgressBarAnimated;
  itemProgressBarAnimated = new KConfigSkeleton::ItemBool( currentGroup(), QStringLiteral( "ProgressBarAnimated" ), mProgressBarAnimated, true );
  addItem( itemProgressBarAnimated, QStringLiteral( "ProgressBarAnimated" ) );
  KConfigSkeleton::ItemInt  *itemProgressBarBusyStepDuration;
  itemProgressBarBusyStepDuration = new KConfigSkeleton::ItemInt( currentGroup(), QStringLiteral( "ProgressBarBusyStepDuration" ), mProgressBarBusyStepDuration, 800 );
  addItem( itemProgressBarBusyStepDuration, QStringLiteral( "ProgressBarBusyStepDuration" ) );
  KConfigSkeleton::ItemInt  *itemScrollBarAddLineButtons;
  itemScrollBarAddLineButtons = new KConfigSkeleton::ItemInt( currentGroup(), QStringLiteral( "ScrollBarAddLineButtons" ), mScrollBarAddLineButtons, 0 );
  addItem( itemScrollBarAddLineButtons, QStringLiteral( "ScrollBarAddLineButtons" ) );
  KConfigSkeleton::ItemInt  *itemScrollBarSubLineButtons;
  itemScrollBarSubLineButtons = new KConfigSkeleton::ItemInt( currentGroup(), QStringLiteral( "ScrollBarSubLineButtons" ), mScrollBarSubLineButtons, 0 );
  addItem( itemScrollBarSubLineButtons, QStringLiteral( "ScrollBarSubLineButtons" ) );
  QList<KConfigSkeleton::ItemEnum::Choice> valuesMnemonicsMode;
  {
    KConfigSkeleton::ItemEnum::Choice choice;
    choice.name = QStringLiteral("MN_NEVER");
    valuesMnemonicsMode.append( choice );
  }
  {
    KConfigSkeleton::ItemEnum::Choice choice;
    choice.name = QStringLiteral("MN_AUTO");
    valuesMnemonicsMode.append( choice );
  }
  {
    KConfigSkeleton::ItemEnum::Choice choice;
    choice.name = QStringLiteral("MN_ALWAYS");
    valuesMnemonicsMode.append( choice );
  }
  KConfigSkeleton::ItemEnum  *itemMnemonicsMode;
  itemMnemonicsMode = new KConfigSkeleton::ItemEnum( currentGroup(), QStringLiteral( "MnemonicsMode" ), mMnemonicsMode, valuesMnemonicsMode, MN_AUTO );
  addItem( itemMnemonicsMode, QStringLiteral( "MnemonicsMode" ) );
  KConfigSkeleton::ItemBool  *itemToolBarDrawItemSeparator;
  itemToolBarDrawItemSeparator = new KConfigSkeleton::ItemBool( currentGroup(), QStringLiteral( "ToolBarDrawItemSeparator" ), mToolBarDrawItemSeparator, true );
  addItem( itemToolBarDrawItemSeparator, QStringLiteral( "ToolBarDrawItemSeparator" ) );
  KConfigSkeleton::ItemBool  *itemViewDrawFocusIndicator;
  itemViewDrawFocusIndicator = new KConfigSkeleton::ItemBool( currentGroup(), QStringLiteral( "ViewDrawFocusIndicator" ), mViewDrawFocusIndicator, true );
  addItem( itemViewDrawFocusIndicator, QStringLiteral( "ViewDrawFocusIndicator" ) );
  KConfigSkeleton::ItemBool  *itemSliderDrawTickMarks;
  itemSliderDrawTickMarks = new KConfigSkeleton::ItemBool( currentGroup(), QStringLiteral( "SliderDrawTickMarks" ), mSliderDrawTickMarks, true );
  addItem( itemSliderDrawTickMarks, QStringLiteral( "SliderDrawTickMarks" ) );
  KConfigSkeleton::ItemBool  *itemViewDrawTreeBranchLines;
  itemViewDrawTreeBranchLines = new KConfigSkeleton::ItemBool( currentGroup(), QStringLiteral( "ViewDrawTreeBranchLines" ), mViewDrawTreeBranchLines, true );
  addItem( itemViewDrawTreeBranchLines, QStringLiteral( "ViewDrawTreeBranchLines" ) );
  KConfigSkeleton::ItemBool  *itemViewInvertSortIndicator;
  itemViewInvertSortIndicator = new KConfigSkeleton::ItemBool( currentGroup(), QStringLiteral( "ViewInvertSortIndicator" ), mViewInvertSortIndicator, true );
  addItem( itemViewInvertSortIndicator, QStringLiteral( "ViewInvertSortIndicator" ) );
  KConfigSkeleton::ItemBool  *itemTabBarDrawCenteredTabs;
  itemTabBarDrawCenteredTabs = new KConfigSkeleton::ItemBool( currentGroup(), QStringLiteral( "TabBarDrawCenteredTabs" ), mTabBarDrawCenteredTabs, false );
  addItem( itemTabBarDrawCenteredTabs, QStringLiteral( "TabBarDrawCenteredTabs" ) );
  KConfigSkeleton::ItemBool  *itemDockWidgetDrawFrame;
  itemDockWidgetDrawFrame = new KConfigSkeleton::ItemBool( currentGroup(), QStringLiteral( "DockWidgetDrawFrame" ), mDockWidgetDrawFrame, false );
  addItem( itemDockWidgetDrawFrame, QStringLiteral( "DockWidgetDrawFrame" ) );
  KConfigSkeleton::ItemBool  *itemSidePanelDrawFrame;
  itemSidePanelDrawFrame = new KConfigSkeleton::ItemBool( currentGroup(), QStringLiteral( "SidePanelDrawFrame" ), mSidePanelDrawFrame, false );
  addItem( itemSidePanelDrawFrame, QStringLiteral( "SidePanelDrawFrame" ) );
  KConfigSkeleton::ItemBool  *itemMenuItemDrawStrongFocus;
  itemMenuItemDrawStrongFocus = new KConfigSkeleton::ItemBool( currentGroup(), QStringLiteral( "MenuItemDrawStrongFocus" ), mMenuItemDrawStrongFocus, true );
  addItem( itemMenuItemDrawStrongFocus, QStringLiteral( "MenuItemDrawStrongFocus" ) );
  QList<KConfigSkeleton::ItemEnum::Choice> valuesWindowDragMode;
  {
    KConfigSkeleton::ItemEnum::Choice choice;
    choice.name = QStringLiteral("WD_NONE");
    valuesWindowDragMode.append( choice );
  }
  {
    KConfigSkeleton::ItemEnum::Choice choice;
    choice.name = QStringLiteral("WD_MINIMAL");
    valuesWindowDragMode.append( choice );
  }
  {
    KConfigSkeleton::ItemEnum::Choice choice;
    choice.name = QStringLiteral("WD_FULL");
    valuesWindowDragMode.append( choice );
  }
  KConfigSkeleton::ItemEnum  *itemWindowDragMode;
  itemWindowDragMode = new KConfigSkeleton::ItemEnum( currentGroup(), QStringLiteral( "WindowDragMode" ), mWindowDragMode, valuesWindowDragMode, WD_FULL );
  addItem( itemWindowDragMode, QStringLiteral( "WindowDragMode" ) );
  KConfigSkeleton::ItemStringList  *itemWindowDragWhiteList;
  itemWindowDragWhiteList = new KConfigSkeleton::ItemStringList( currentGroup(), QStringLiteral( "WindowDragWhiteList" ), mWindowDragWhiteList );
  addItem( itemWindowDragWhiteList, QStringLiteral( "WindowDragWhiteList" ) );
  KConfigSkeleton::ItemStringList  *itemWindowDragBlackList;
  itemWindowDragBlackList = new KConfigSkeleton::ItemStringList( currentGroup(), QStringLiteral( "WindowDragBlackList" ), mWindowDragBlackList );
  addItem( itemWindowDragBlackList, QStringLiteral( "WindowDragBlackList" ) );
  KConfigSkeleton::ItemBool  *itemSplitterProxyEnabled;
  itemSplitterProxyEnabled = new KConfigSkeleton::ItemBool( currentGroup(), QStringLiteral( "SplitterProxyEnabled" ), mSplitterProxyEnabled, true );
  addItem( itemSplitterProxyEnabled, QStringLiteral( "SplitterProxyEnabled" ) );
  KConfigSkeleton::ItemInt  *itemSplitterProxyWidth;
  itemSplitterProxyWidth = new KConfigSkeleton::ItemInt( currentGroup(), QStringLiteral( "SplitterProxyWidth" ), mSplitterProxyWidth, 12 );
  addItem( itemSplitterProxyWidth, QStringLiteral( "SplitterProxyWidth" ) );
  KConfigSkeleton::ItemBool  *itemWidgetExplorerEnabled;
  itemWidgetExplorerEnabled = new KConfigSkeleton::ItemBool( currentGroup(), QStringLiteral( "WidgetExplorerEnabled" ), mWidgetExplorerEnabled, false );
  addItem( itemWidgetExplorerEnabled, QStringLiteral( "WidgetExplorerEnabled" ) );
  KConfigSkeleton::ItemBool  *itemDrawWidgetRects;
  itemDrawWidgetRects = new KConfigSkeleton::ItemBool( currentGroup(), QStringLiteral( "DrawWidgetRects" ), mDrawWidgetRects, false );
  addItem( itemDrawWidgetRects, QStringLiteral( "DrawWidgetRects" ) );
  KConfigSkeleton::ItemInt  *itemMenuOpacity;
  itemMenuOpacity = new KConfigSkeleton::ItemInt( currentGroup(), QStringLiteral( "MenuOpacity" ), mMenuOpacity, 100 );
  addItem( itemMenuOpacity, QStringLiteral( "MenuOpacity" ) );
}

StyleConfigData::~StyleConfigData()
{
  if (s_globalStyleConfigData.exists() && !s_globalStyleConfigData.isDestroyed()) {
    s_globalStyleConfigData()->q = nullptr;
  }
}

