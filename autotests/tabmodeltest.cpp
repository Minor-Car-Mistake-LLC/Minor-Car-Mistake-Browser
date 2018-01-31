/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2018 David Rosca <nowrep@gmail.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
* ============================================================ */
#include "tabmodeltest.h"
#include "autotests.h"
#include "tabmodel.h"
#include "tabtreemodel.h"
#include "webtab.h"
#include "tabwidget.h"
#include "tabbedwebview.h"
#include "mainapplication.h"
#include "browserwindow.h"

#include "modeltest.h"

void TabModelTest::initTestCase()
{
}

void TabModelTest::cleanupTestCase()
{
}

void TabModelTest::basicTest()
{
    BrowserWindow *w = mApp->createWindow(Qz::BW_NewWindow);

    TabModel model1(w);
    ModelTest modelTest(&model1);

    QSignalSpy rowsInsertedSpy(&model1, &TabModel::rowsInserted);
    QSignalSpy rowsRemovedSpy(&model1, &TabModel::rowsRemoved);

    QCOMPARE(model1.rowCount(), 0);

    rowsInsertedSpy.wait();

    QCOMPARE(rowsInsertedSpy.count(), 1);
    WebTab *tab0 = w->weView(0)->webTab();
    QCOMPARE(rowsInsertedSpy.at(0).at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(rowsInsertedSpy.at(0).at(1).toInt(), 0);
    QCOMPARE(rowsInsertedSpy.at(0).at(2).toInt(), 0);
    QCOMPARE(model1.data(model1.index(0, 0), TabModel::WebTabRole).value<WebTab*>(), tab0);

    rowsInsertedSpy.clear();

    w->tabWidget()->addView(QUrl("http://test.com"));

    QCOMPARE(rowsInsertedSpy.count(), 1);
    WebTab *tab1 = w->weView(1)->webTab();
    QCOMPARE(rowsInsertedSpy.at(0).at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(rowsInsertedSpy.at(0).at(1).toInt(), 1);
    QCOMPARE(rowsInsertedSpy.at(0).at(2).toInt(), 1);
    QCOMPARE(model1.data(model1.index(1, 0), TabModel::WebTabRole).value<WebTab*>(), tab1);

    w->tabWidget()->moveTab(0, 1);
    QCOMPARE(w->weView(0)->webTab(), tab1);
    QCOMPARE(w->weView(1)->webTab(), tab0);

    w->tabWidget()->moveTab(1, 0);
    QCOMPARE(w->weView(0)->webTab(), tab0);
    QCOMPARE(w->weView(1)->webTab(), tab1);

    w->tabWidget()->moveTab(0, 1);
    QCOMPARE(w->weView(0)->webTab(), tab1);
    QCOMPARE(w->weView(1)->webTab(), tab0);

    QCOMPARE(rowsRemovedSpy.count(), 0);

    w->tabWidget()->closeTab(1);

    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.at(0).at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(rowsRemovedSpy.at(0).at(1).toInt(), 1);
    QCOMPARE(rowsRemovedSpy.at(0).at(2).toInt(), 1);

    QCOMPARE(model1.rowCount(), 1);

    TabModel model2(w);
    ModelTest modelTest2(&model2);
    QCOMPARE(model2.rowCount(), 1);

    QTest::qWait(10);
    delete w;
}

void TabModelTest::dataTest()
{
    BrowserWindow *w = mApp->createWindow(Qz::BW_NewWindow);
    TabModel model(w);
    ModelTest modelTest(&model);

    QTRY_COMPARE(model.rowCount(), 1);

    WebTab *tab0 = w->weView(0)->webTab();
    QCOMPARE(model.index(0, 0).data(TabModel::TitleRole).toString(), tab0->title());
    QCOMPARE(model.index(0, 0).data(Qt::DisplayRole).toString(), tab0->title());
    QCOMPARE(model.index(0, 0).data(TabModel::IconRole).value<QIcon>().pixmap(16), tab0->icon().pixmap(16));
    QCOMPARE(model.index(0, 0).data(Qt::DecorationRole).value<QIcon>().pixmap(16), tab0->icon().pixmap(16));
    QCOMPARE(model.index(0, 0).data(TabModel::PinnedRole).toBool(), tab0->isPinned());
    QCOMPARE(model.index(0, 0).data(TabModel::RestoredRole).toBool(), tab0->isRestored());

    w->tabWidget()->addView(QUrl("http://test.com"));

    WebTab *tab1 = w->weView(1)->webTab();

    QTest::qWait(10);
    delete w;
}
void TabModelTest::treeModelTest()
{
    BrowserWindow *w = mApp->createWindow(Qz::BW_NewWindow);

    TabModel sourceModel(w);
    TabTreeModel model;
    model.setSourceModel(&sourceModel);
    ModelTest modelTest(&model);

    w->tabWidget()->addView(QUrl());
    w->tabWidget()->addView(QUrl());
    w->tabWidget()->addView(QUrl());
    w->tabWidget()->addView(QUrl());
    w->tabWidget()->addView(QUrl());

    QTRY_COMPARE(model.rowCount(QModelIndex()), 6);

    WebTab *tab1 = w->weView(0)->webTab();
    WebTab *tab2 = w->weView(1)->webTab();
    WebTab *tab3 = w->weView(2)->webTab();
    WebTab *tab4 = w->weView(3)->webTab();
    WebTab *tab5 = w->weView(4)->webTab();
    WebTab *tab6 = w->weView(5)->webTab();

    QCOMPARE(model.index(0, 0).data(TabModel::WebTabRole).value<WebTab*>(), tab1);
    QCOMPARE(model.index(1, 0).data(TabModel::WebTabRole).value<WebTab*>(), tab2);
    QCOMPARE(model.index(2, 0).data(TabModel::WebTabRole).value<WebTab*>(), tab3);
    QCOMPARE(model.index(3, 0).data(TabModel::WebTabRole).value<WebTab*>(), tab4);
    QCOMPARE(model.index(4, 0).data(TabModel::WebTabRole).value<WebTab*>(), tab5);
    QCOMPARE(model.index(5, 0).data(TabModel::WebTabRole).value<WebTab*>(), tab6);

    QPersistentModelIndex tab1index = model.index(0, 0);
    QPersistentModelIndex tab2index = model.index(1, 0);
    QPersistentModelIndex tab3index = model.index(2, 0);
    QPersistentModelIndex tab4index = model.index(3, 0);
    QPersistentModelIndex tab5index = model.index(4, 0);
    QPersistentModelIndex tab6index = model.index(5, 0);

    QCOMPARE(model.rowCount(tab1index), 0);
    tab1->addChildTab(tab2);

    QCOMPARE(model.rowCount(tab1index), 1);
    QCOMPARE(model.index(0, 0, tab1index).data(TabModel::WebTabRole).value<WebTab*>(), tab2);

    tab1->addChildTab(tab3);
    QCOMPARE(model.rowCount(tab1index), 2);
    QCOMPARE(model.index(0, 0, tab1index).data(TabModel::WebTabRole).value<WebTab*>(), tab2);
    QCOMPARE(model.index(1, 0, tab1index).data(TabModel::WebTabRole).value<WebTab*>(), tab3);

    tab1->addChildTab(tab4, 1);
    QCOMPARE(model.rowCount(tab1index), 3);
    QCOMPARE(model.index(0, 0, tab1index).data(TabModel::WebTabRole).value<WebTab*>(), tab2);
    QCOMPARE(model.index(1, 0, tab1index).data(TabModel::WebTabRole).value<WebTab*>(), tab4);
    QCOMPARE(model.index(2, 0, tab1index).data(TabModel::WebTabRole).value<WebTab*>(), tab3);

    tab4->addChildTab(tab5);
    tab4->addChildTab(tab6);

    QCOMPARE(model.rowCount(tab4index), 2);
    QCOMPARE(model.index(0, 0, tab4index).data(TabModel::WebTabRole).value<WebTab*>(), tab5);
    QCOMPARE(model.index(1, 0, tab4index).data(TabModel::WebTabRole).value<WebTab*>(), tab6);

    w->tabWidget()->closeTab(tab4->tabIndex());

    QCOMPARE(model.rowCount(tab1index), 4);
    QCOMPARE(model.index(0, 0, tab1index).data(TabModel::WebTabRole).value<WebTab*>(), tab2);
    QCOMPARE(model.index(1, 0, tab1index).data(TabModel::WebTabRole).value<WebTab*>(), tab5);
    QCOMPARE(model.index(2, 0, tab1index).data(TabModel::WebTabRole).value<WebTab*>(), tab6);
    QCOMPARE(model.index(3, 0, tab1index).data(TabModel::WebTabRole).value<WebTab*>(), tab3);

    tab1->addChildTab(tab3, 0);

    QCOMPARE(model.rowCount(tab1index), 4);
    QCOMPARE(model.index(0, 0, tab1index).data(TabModel::WebTabRole).value<WebTab*>(), tab3);
    QCOMPARE(model.index(1, 0, tab1index).data(TabModel::WebTabRole).value<WebTab*>(), tab2);
    QCOMPARE(model.index(2, 0, tab1index).data(TabModel::WebTabRole).value<WebTab*>(), tab5);
    QCOMPARE(model.index(3, 0, tab1index).data(TabModel::WebTabRole).value<WebTab*>(), tab6);

    tab2->setParentTab(nullptr);

    QCOMPARE(model.rowCount(tab1index), 3);
    QCOMPARE(model.index(0, 0).data(TabModel::WebTabRole).value<WebTab*>(), tab2);
    QCOMPARE(model.index(0, 0, tab1index).data(TabModel::WebTabRole).value<WebTab*>(), tab3);
    QCOMPARE(model.index(1, 0, tab1index).data(TabModel::WebTabRole).value<WebTab*>(), tab5);
    QCOMPARE(model.index(2, 0, tab1index).data(TabModel::WebTabRole).value<WebTab*>(), tab6);

    w->tabWidget()->closeTab(tab1->tabIndex());

    QCOMPARE(model.rowCount(QModelIndex()), 4);
    QCOMPARE(model.index(0, 0).data(TabModel::WebTabRole).value<WebTab*>(), tab2);
    QCOMPARE(model.index(1, 0).data(TabModel::WebTabRole).value<WebTab*>(), tab3);
    QCOMPARE(model.index(2, 0).data(TabModel::WebTabRole).value<WebTab*>(), tab5);
    QCOMPARE(model.index(3, 0).data(TabModel::WebTabRole).value<WebTab*>(), tab6);

    QTest::qWait(10);
    delete w;
}

FALKONTEST_MAIN(TabModelTest)
