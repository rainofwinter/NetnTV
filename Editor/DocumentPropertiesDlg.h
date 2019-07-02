#pragma once
#include "ui_documentPropertiesDlg.h"
#include "Orientation.h"

class EditorDocument;
class DocumentTemplate;
class Command;

class DocumentPropertiesDlg : public QDialog
{
	Q_OBJECT
public:
	DocumentPropertiesDlg(EditorDocument * document, QWidget * parent);
	~DocumentPropertiesDlg();

	Orientation orientation() const;
	int width() const;
	int height() const;

	std::vector<std::string> scriptFiles() const;

	std::map<std::string, std::string> scriptAccessibleFiles() const;

	DocumentTemplateSPtr newDocumentTemplate() const;

	std::string remoteReadServer() const;

	bool allowMultitouch() const;
	bool preDownload() const;

protected slots:
	void onOk();
	
	void onAddScript();
	void onRemoveScript();

	void onAddScriptFile();
	void onRemoveScriptFile();

	void onScriptsSelChanged();
	void onScriptFilesSelChanged();

private:
	Ui::DocumentPropertiesDlg ui;
	EditorDocument * document_;
	std::vector<DocumentTemplate *> docTemplates_;

	DocumentTemplateSPtr origTemplate_;
	DocumentTemplateSPtr newTemplate_;
	
};