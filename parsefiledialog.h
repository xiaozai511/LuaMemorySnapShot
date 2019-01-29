#ifndef PARSEFILEDIALOG_H
#define PARSEFILEDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class ParseFileDialog;
}
class MemoryRefrenceInfoMgr;

class ParseFileDialog : public QDialog
{
	Q_OBJECT

public:
//	explicit ParseFileDialog(QWidget *parent = 0);
	ParseFileDialog(const QString& filePath, MemoryRefrenceInfoMgr* infoMgr, QWidget *parent);
	ParseFileDialog(const QString& before, const QString& after, MemoryRefrenceInfoMgr* infoMgr, QWidget *parent);
	~ParseFileDialog();

protected slots:
	void onParseRunning(const QString& filepath, int usetime, int progress);
	void onBaseParseFinished();
	void onParseFinished();

protected:
	void closeEvent(QCloseEvent* event);

	void stopParsing();

private:
	Ui::ParseFileDialog *ui;

	QString m_filePath;
	QString m_filePathBase;

	MemoryRefrenceInfoMgr* m_infoMgr;
	MemoryRefrenceInfoMgr* m_infoMgrBase;
	bool bCompare;

	bool bAllOver;
};

#endif // PARSEFILEDIALOG_H
