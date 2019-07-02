#pragma once
#include "Property.h"
#include <stack>
///////////////////////////////////////////////////////////////////////////////
class SceneObject;
class GLWidget;
class EditorDocument;

class PropertyPage : public Property
{
	Q_OBJECT
public:
	enum Arrangement
	{
		Vertical,
		Horizontal
	};
public:
	PropertyPage();	
	virtual ~PropertyPage();

	virtual QWidget * widget() const {return topWidget_;}
	virtual void setName(const QString & name);
	void setGroupBox(bool val);
	void setMargins(int l, int t, int r, int b);
	virtual void setDocument(EditorDocument * document);
	virtual void setView(GLWidget * widget);
	virtual PropertyPage * propertyPage() {return this;}

	virtual bool isValid() const {return true;}

	QGroupBox * startGroup(const QString & groupTitle);
	void endGroup();
	
	void addWidget(QWidget * widget);
	void addLabel(const QString & labelStr);
	void addProperty(Property * property, Arrangement arrangement = Horizontal);
	void addPropertyPage(PropertyPage * page);
	void addSpacing(int size);
	void addSpring();

	void setCircularTabOrder();

	virtual void onEditObject(){};
signals:
	void changed(Property * property);
public slots:
	virtual void update() {}	
	virtual void onChanged(Property * property) {emit changed(this);}
	
protected:
	std::vector<Property *> properties_;
	QGridLayout * layout_;
	QGroupBox * groupBox_;
	QWidget * topWidget_;
	EditorDocument * document_;
	GLWidget * view_;
	std::stack<QGridLayout *> layouts_;

	QMargins margins_;
};

///////////////////////////////////////////////////////////////////////////////
class Action;

class ActionPropertyPage : public PropertyPage
{
public:
	void setAction(Action * action) {action_ = action;}
protected:
	Action * action_;
};

///////////////////////////////////////////////////////////////////////////////
class Event;

class EventPropertyPage : public PropertyPage
{
public:
	void setEvent(Event * event) {event_ = event;}
protected:
	Event * event_;
};

///////////////////////////////////////////////////////////////////////////////
class CorePropertiesPage : public PropertyPage
{
	Q_OBJECT
public:
	CorePropertiesPage();
	virtual void update();

	void setIdEnabled(bool val) 
	{
		id_->setEnabled(val);
	}
	
protected slots:
	virtual void onChanged(Property * property);

private:
	EditProperty * id_;
};
///////////////////////////////////////////////////////////////////////////////
class CoreAppObjectPropertiesPage : public PropertyPage
{
	Q_OBJECT
public:
	CoreAppObjectPropertiesPage();
	virtual void update();

protected slots:
	virtual void onChanged(Property * property);

private:
	EditProperty * id_;
};
///////////////////////////////////////////////////////////////////////////////
class ScalePropertiesPage : public PropertyPage
{
public:
	ScalePropertiesPage();
	virtual void update();

private:
	virtual void onChanged(Property * property);
private:
	enum 
	{
		Proportion = 0,
		Size = 1
	};
	EditProperty * scaleX_, * scaleY_, * scaleZ_;
	ComboBoxProperty * mode_;
	QDoubleValidator doubleValidator_;
};

///////////////////////////////////////////////////////////////////////////////
class ScaleAppObjectPropertiesPage : public PropertyPage
{
public:
	ScaleAppObjectPropertiesPage();
	virtual void update();
	
private:
	virtual void onChanged(Property * property);
private:
	enum 
	{
		Proportion = 0,
		Size = 1
	};
	EditProperty * scaleX_, * scaleY_;
	ComboBoxProperty * mode_;
	QDoubleValidator doubleValidator_;
};


///////////////////////////////////////////////////////////////////////////////
class TransformPropertiesPage : public PropertyPage
{
public:
	TransformPropertiesPage();
	virtual void update();

private:
	virtual void onChanged(Property * property);

private:
	EditProperty * transX_, * transY_, * transZ_;
	EditProperty * rotX_, * rotY_, * rotZ_;
	ScalePropertiesPage * scale_;
	EditProperty * pivotX_, * pivotY_, * pivotZ_;
	QDoubleValidator doubleValidator_;
};
///////////////////////////////////////////////////////////////////////////////
class TransformAppObjectPropertiesPage : public PropertyPage
{
public:
	TransformAppObjectPropertiesPage();
	virtual void update();

private:
	virtual void onChanged(Property * property);

private:
	EditProperty * transX_, * transY_;
	//EditProperty * rotation_;
	ScaleAppObjectPropertiesPage * scale_;
	EditProperty * pivotX_, * pivotY_;
	QDoubleValidator doubleValidator_;
};
///////////////////////////////////////////////////////////////////////////////
class VisualPropertiesPage : public PropertyPage
{
	Q_OBJECT
public:
	VisualPropertiesPage(bool hasOpacity = true);
	~VisualPropertiesPage();
	virtual void update();
protected slots:
	virtual void onChanged(Property * property);
private:
	TransformPropertiesPage * transform_;
	EditProperty * opacity_;
	BoolProperty * visible_;
	UnitDoubleValidator * opacityValidator_;
};

///////////////////////////////////////////////////////////////////////////////
class VisualAppObjectPropertiesPage : public PropertyPage
{
	Q_OBJECT
public:
	VisualAppObjectPropertiesPage();
	~VisualAppObjectPropertiesPage();
	virtual void update();
protected slots:
	virtual void onChanged(Property * property);
private:

	EditProperty * x_, * y_, * width_, * height_;

	EditProperty * opacity_;
	BoolProperty * visible_;
	UnitDoubleValidator * opacityValidator_;

	QDoubleValidator posValidator_;
	QDoubleValidator dimValidator_;
};