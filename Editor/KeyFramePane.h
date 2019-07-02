#pragma once
#include "PropertyPage.h"
#include "Property.h"


class PaneButton;
class EditorDocument;

class Interpolator;

class KeyFramePropertyPage : public PropertyPage
{
public:
	KeyFramePropertyPage();
	~KeyFramePropertyPage();

	virtual void update();
	virtual void onChanged(Property * property);

	void clear();

private:
	EditProperty * time_;
	ComboBoxProperty * interpolation_;
	TimeValidator timeValidator_;

	std::vector<Interpolator *> interps_;
};

class MainWindow;
class HeaderLabel;

class KeyFramePane : public QWidget
{
	Q_OBJECT
public:
	KeyFramePane(MainWindow * mainWindow);
	~KeyFramePane();
	void setDocument(EditorDocument * document);
	virtual QSize sizeHint() const {return QSize(150,0);}
private slots:
	void changeButtonsState();
	void keyFrameSelectionChanged();
//	void onDelete();
private:
	QLabel * label_;
	PaneButton * editButton_, * delButton_;
	EditorDocument * document_;
	QScrollArea * scrollArea_;
	KeyFramePropertyPage * keyFrameProperties_;
};