#pragma once
#include "EditorAction.h"
#include "PropertyPage.h"


class EditorShareAction : public EditorAction
{
public:	
	virtual EditorAction * clone() const
	{
		return new EditorShareAction(*this);
	}
	virtual ActionPropertyPage * propertyPage() const;
	virtual boost::uuids::uuid actionType() const;

	virtual QString toString(Action * action) const;
	//virtual QString toData(Action * action) const;
};


class EditorShareActionPropertyPage : public ActionPropertyPage
{
	Q_OBJECT
public:
	EditorShareActionPropertyPage();
	~EditorShareActionPropertyPage();
	
	virtual void setDocument(EditorDocument * document);

	virtual bool isValid() const;

public slots:
	virtual void update();
	virtual void onChanged(Property * property);

private:	
	ComboBoxProperty * targetType_;	
	EditTextProperty * msg_;

	ComboBoxProperty * imgPath_;
	
	int imgcnt_;
	std::map<int, const std::string *> filePath_;	
};