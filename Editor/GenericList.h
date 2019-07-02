#pragma once

struct GenericListItem
{
	GenericListItem(const QString & str, void * data)
	{
		this->string = str;
		this->data = data;
	}
	QString string;
	void * data;
};
Q_DECLARE_METATYPE(GenericListItem *)

class GenericListModel : public QAbstractItemModel
{
	friend class GenericList;
	Q_OBJECT
public:
	GenericListModel();
	~GenericListModel();

	void addTopLevelItem(const QString & str, void * data);
	void clearItems();

	virtual QStringList mimeTypes() const;
	virtual QMimeData * mimeData(const QModelIndexList & indexes) const;

	virtual QVariant data(const QModelIndex &index, int role) const;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
	virtual Qt::DropActions supportedDropActions() const;
	virtual QVariant headerData(
		int section, Qt::Orientation orientation, int role) const;

	virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;
	virtual QModelIndex parent(const QModelIndex &index) const;
	virtual int rowCount(const QModelIndex &parent) const;
	virtual int columnCount(const QModelIndex &parent) const;
	
	virtual bool dropMimeData(const QMimeData *data, 
		Qt::DropAction action, int row, int column, const QModelIndex &parent);

	bool isItemSelected(GenericListItem * item) const;
	void setSelectedItems(const std::vector<GenericListItem *> & selObjs)
	{
		selItems_ = selObjs;
	}

	void setEditable(bool editable);
	void setReorderable(bool reorderable);

	const std::vector<GenericListItem *> & selectedItems() const
	{return selItems_;}

	
private:
	void deleteSelectedItems();
	void insertBefore(GenericListItem * newObj, GenericListItem * refObj);

private:
	std::vector<GenericListItem *> items_;
	std::vector<GenericListItem *> selItems_;

	bool editable_;
	bool reorderable_;
};



////////////////////////////////////////////////////////////////////////////////
class GenericList;

class GenericListDelegate : public QStyledItemDelegate
{
Q_OBJECT
public:
	GenericListDelegate();
	virtual void paint(QPainter *painter, 
		const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

	virtual QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, 
		const QModelIndex & index) const;

	virtual void setModelData(QWidget *editor, 
		QAbstractItemModel *model, const QModelIndex &index) const;

	virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
signals:
	void itemEdited(GenericListItem * item);
protected slots:

	void commitAndCloseEditor();
	void cancelEdit();

private:

};


///////////////////////////////////////////////////////////////////////////////

class GenericList : public QTreeView
{
Q_OBJECT
public:
	enum SelectionMode
	{
		SingleMode,
		MultipleMode
	};
public:
	GenericList();
	~GenericList();
	void addTopLevelItem(const QString & str, void * data);
	void clearItems();
	void setEditable(bool editable);
	void setReorderable(bool reorderable);
	const std::vector<GenericListItem *> & selectedItems() const;
	const std::vector<GenericListItem *> & items() const;
	void deleteSelectedItems();
	void setMode(SelectionMode mode);

	int count () const;

	void selectIndex(int index);
	int selectedIndex() const;
signals:
	void selectionChanged();
	void layoutChanged();	
	void itemEdited(GenericListItem * item);
protected slots:
	void onLayoutChanged();

protected:	
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseDoubleClickEvent(QMouseEvent * event);
	virtual void expanded(const QModelIndex &index);
	virtual void collapsed(const QModelIndex &index);
		
	virtual void selectionChanged(
		const QItemSelection &selected, const QItemSelection &deselected);

private:	
	bool ignoreSelChanged_;
	GenericListModel * model_;
	GenericListDelegate * delegate_;
};

