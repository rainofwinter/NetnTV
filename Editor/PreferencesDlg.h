#pragma once
#include "ui_preferencesDlg.h"
#include "Property.h"
class Scene;
class FileNameProperty;
class ColorProperty;
class EditProperty;
class BoolProperty;

class PreferencesDlg : public QDialog
{
	Q_OBJECT
public:
	PreferencesDlg(QWidget * parent);
	~PreferencesDlg();
protected slots:
	void onOk();
private:
	Ui::PreferencesDlg ui;

	FileNameProperty * playerLocation_;
	ColorProperty * sceneOutlineColor_;
	EditProperty * gridSpacing_;
	BoolProperty * showGrid_;
	EditProperty * tempfileSaveInterval_;
	PropertyPage * ppage;
	QVBoxLayout * layout;
};