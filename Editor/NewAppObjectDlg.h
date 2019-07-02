#pragma once
#include <QDialog>
#include "ui_newAppObjectDlg.h"

///////////////////////////////////////////////////////////////////////////////
class EditorDocument;
class Scene;
class AppObject;

class NewAppObjectDlg : public QDialog
{
	Q_OBJECT
public:
	NewAppObjectDlg(EditorDocument * document, QWidget * parent);
	
	AppObject * createAppObject() const;

private slots:
	void onItemSelectionChanged();
	void onItemDoubleClicked(QListWidgetItem * item);
private:
	Ui::NewAppObjectDlg ui;
	EditorDocument * document_;
	std::vector<AppObject *> sceneObjs_;
	
};

