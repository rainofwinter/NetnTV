#include "stdafx.h"
#include "ActionDlg.h"
#include "EditorGlobal.h"
#include "Global.h"
#include "Action.h"
#include "EditorAction.h"
#include "PropertyPage.h"
#include "EditorDocument.h"

#include "ActionTargetListPane.h"
#include "PaneLabel.h"

using namespace std;


ActionPropertiesWidget::ActionPropertiesWidget()
{	
	setWidgetResizable(true);
	layout_ = 0;
	action_ = 0;
	scrollContents_ = new QWidget;
	setWidget(scrollContents_);
}

void ActionPropertiesWidget::setPropertyPage(ActionPropertyPage * page)
{
	page_.reset(page);

	delete layout_;
	if (page)
	{
		page->setAction(action_);
		page->setDocument(document_);		
		page->update();
		layout_ = new QVBoxLayout;
		layout_->setSpacing(0);
		layout_->setMargin(4);
		layout_->addWidget(page->widget());
		layout_->addItem(new QSpacerItem(
			0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));		

		scrollContents_->setLayout(layout_);
		connect(page, SIGNAL(changed(Property *)), this, SIGNAL(changed(Property *)));
	}
	else 
	{
		layout_ = 0;
	}
	
}

void ActionPropertiesWidget::setAction(Action * action, ActionPropertyPage * page)
{
	action_ = action;
	setPropertyPage(page);
	if (page_)
	{
		page_->setAction(action);
		page_->update();
	}
}

bool ActionPropertiesWidget::isValid() const
{
	if (!page_) return true;
	else return page_->isValid();
}

////////////////////////////////////////////////////////////////////////////////


void ActionsWidgetDelegate::paint(QPainter *painter, 
	const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItemV4 opt = option;
	opt.state &= ~QStyle::State_HasFocus;
	opt.state |= QStyle::State_Active;	
	
	if (index.column() == 0)
	{		
		initStyleOption(&opt, index);
		
		const QStyle * style = 
			opt.SO_DockWidget ? opt.widget->style() : QApplication::style();
		const QWidget * widget = opt.widget;
	
		style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);
	}
	else
		QStyledItemDelegate::paint(painter, option, index);
}

QSize ActionsWidgetDelegate::sizeHint(
	const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	return QSize(0, 20);
}
////////////////////////////////////////////////////////////////////////////////
void ActionsWidget::selectItem(int index)
{
	QItemSelectionModel * selModel = selectionModel();
	selModel->clearSelection();
	selModel->select(
		model()->index(index, 0, QModelIndex()), QItemSelectionModel::Select);
}

void ActionsWidget::selectionChanged(
	const QItemSelection &selected, const QItemSelection &deselected)
{
	QItemSelectionModel * selModel = selectionModel();

	QModelIndexList indices = selModel->selectedIndexes();

	if (indices.empty())
		emit actionSelectionChanged(-1);
	else
		emit actionSelectionChanged(indices.front().row());
}
////////////////////////////////////////////////////////////////////////////////

ActionDlg::ActionDlg(
	EditorDocument * document, Action * action, QWidget * parent)
: QDialog(parent), document_(document)
{
	ui.setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); 

	targetListPane_ = new ActionTargetListPane;
	QVBoxLayout * layout1 = new QVBoxLayout;	
	layout1->setMargin(0);
	ui.widget1->setLayout(layout1);
	layout1->addWidget(targetListPane_);
	targetListPane_->show();
	targetListPane_->setDocument(document_);
	ui.splitter->setCollapsible(0, false);
	
	QVBoxLayout * layout2 = new QVBoxLayout;
	layout2->setMargin(0);
	layout2->setSpacing(0);
	ui.widget2->setLayout(layout2);
	actionsLabel_ = new PaneLabel(tr("Supported Actions"));
	layout2->addWidget(actionsLabel_);
	actionsList_ = new ActionsWidget;	
	layout2->addWidget(actionsList_);
	ui.splitter->setCollapsible(1, false);

	QVBoxLayout * layout3 = new QVBoxLayout;
	layout3->setMargin(0);
	layout3->setSpacing(0);
	ui.widget3->setLayout(layout3);
	propertiesLabel_ = new PaneLabel(tr("Action Properties"));
	layout3->addWidget(propertiesLabel_);
	properties_ = new ActionPropertiesWidget;
	layout3->addWidget(properties_);
	properties_->setDocument(document_);
	ui.splitter->setCollapsible(2, false);

	connect(properties_, SIGNAL(changed(Property *)), 
			this, SLOT(onActionPropertyChanged(Property *)));	

	connect(targetListPane_, SIGNAL(itemSelected(SceneObject *)),
		this, SLOT(onSceneObjectSelected(SceneObject *)));
	
	targetListPane_->selectItem(0);

	connect(actionsList_, SIGNAL(actionSelectionChanged(int)), this, 
		SLOT(onActionSelectionChanged(int)));
	onActionSelectionChanged(-1);

	//user has passed in an existing action
	if (action)
	{
		int index = -1;

		SceneObject * targetObj = action->targetObject();
		targetListPane_->selectObject(targetObj);
		
		for (int i = 0; i < (int)actions_.size(); ++i)
		{
			if (actions_[i]->type() == action->type())
				index = i;
		}
		actionsList_->selectItem(index);

		setAction(action);	
	}
}


void ActionDlg::onSceneObjectSelected(SceneObject * obj)
{
	targetObject_ = obj;

	actionsList_->clear();
	AppObject* appobj = document_->selectedAppObject();
	if (appobj)
		Global::instance().supportedActions(&actions_, appobj);
	else
		Global::instance().supportedActions(&actions_, obj);

	BOOST_FOREACH(Action * action, actions_)
	{
		QStringList strList(QString::fromLocal8Bit(action->typeStr()));
		actionsList_->addTopLevelItem(new QTreeWidgetItem(strList));
	}		
}

void ActionDlg::onActionSelectionChanged(int index)
{
	if (index >= 0)
	{
		setAction(actions_[index]);		
	}
	else
	{
		setAction(0);
	}		
}

void ActionDlg::setAction(Action * action)
{
	if (action)
	{
		EditorScene * edScene = 
			document_->editorScene(document_->selectedScene()).get();	

		EditorAction * edAction = 
			EditorGlobal::instance().editorAction(action);

		action_.reset(action->clone());

		ActionPropertyPage * propPage = 0;
		if (edAction) propPage = edAction->propertyPage();
		action_->setTargetObject(targetObject_);
		action_->setDocument(document_->document());
		properties_->setAction(action_.get(), propPage);		

		onActionPropertyChanged(0);
	}
	else
	{
		action_.reset(0);
		properties_->setAction(0, 0);
		ui.okButton->setEnabled(false);
	}
}


void ActionDlg::onActionPropertyChanged(Property *)
{
	setOkButtonState();
}

void ActionDlg::setOkButtonState()
{
	ui.okButton->setEnabled(properties_->isValid());
}

Action * ActionDlg::createRequestedAction() const
{
	if (action_) return action_->clone();
	else return 0;
}