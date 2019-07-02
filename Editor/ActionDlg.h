#pragma once
#include <QDialog>
#include "ui_actionDlg.h"
#include "Types.h"

class EditorDocument;
class ActionTargetListPane;
class PaneLabel;
class ActionPropertyPage;
class Property;

class ActionPropertiesWidget : public QScrollArea
{
	Q_OBJECT
public:
	ActionPropertiesWidget();
	virtual QSize sizeHint() const {return QSize(150, 150);}
	void setAction(Action * action, ActionPropertyPage * page);
	
	void setDocument(EditorDocument * document) {document_ = document;}	
	bool isValid() const;

signals:
	void changed(Property *);
private:
	void setPropertyPage(ActionPropertyPage * page);

	boost::scoped_ptr<ActionPropertyPage> page_;
	
	QWidget * scrollContents_;
	Action * action_;
	EditorDocument * document_;
	QVBoxLayout * layout_;
};

///////////////////////////////////////////////////////////////////////////////

class ActionsWidgetDelegate : public QStyledItemDelegate
{
public:
	virtual void paint(QPainter *painter, 
		const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

};
///////////////////////////////////////////////////////////////////////////////
class ActionsWidget : public QTreeWidget
{
	Q_OBJECT
public:
	ActionsWidget()
	{
		delegate_ = new ActionsWidgetDelegate;
		this->setItemDelegate(delegate_);
		setHeaderHidden(true);
		setIndentation(0);
		//setFrameShape(QFrame::NoFrame);
		setRootIsDecorated(false);
	}
	~ActionsWidget()
	{
		delete delegate_;
	}
	void selectItem(int index);
	virtual QSize sizeHint() const {return QSize(150, 150);}
signals:
	void actionSelectionChanged(int index);
protected:
	virtual void selectionChanged(
		const QItemSelection &selected, const QItemSelection &deselected);
private:
	ActionsWidgetDelegate * delegate_;
};

///////////////////////////////////////////////////////////////////////////////


class ActionDlg : public QDialog
{
	Q_OBJECT
public:
	ActionDlg(EditorDocument * document, Action * action, QWidget * parent);
	Action * createRequestedAction() const;

private slots:
	void onSceneObjectSelected(SceneObject * obj);	
	void onActionPropertyChanged(Property *);
	void onActionSelectionChanged(int index);

private:
	void setAction(Action * action);
	void setOkButtonState();

	Ui::ActionDlg ui;
	ActionTargetListPane * targetListPane_;
	EditorDocument * document_;
	
	ActionsWidget * actionsList_;
	PaneLabel * actionsLabel_;

	ActionPropertiesWidget * properties_;
	PaneLabel * propertiesLabel_;
	std::vector<Action *> actions_;

	SceneObject * targetObject_;
	boost::scoped_ptr<Action> action_;
};


