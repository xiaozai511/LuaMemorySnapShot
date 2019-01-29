#ifndef MEMORYREFRENCEINFO_H
#define MEMORYREFRENCEINFO_H

#include <QString>
#include <QList>
#include <QMap>
#include <QSet>
#include <functional>
#include <QThread>
#include <unordered_map>

class MemoryRefrenceInfoMgr;

class MemoryRefrenceInfo
{
protected:
	MemoryRefrenceInfo();
	~MemoryRefrenceInfo();

public:
	friend class MemoryRefrenceInfoMgr;

	static QByteArray TYPE_TABLE;
	static QByteArray TYPE_FUNCTION;
	static QByteArray TYPE_STRING;
	static QByteArray TYPE_USERDATA;
	static QByteArray TYPE_THREAD;

	void setType(const QString& _type);
	const QString& getType();

	void setName(const QString& _name);
	const QString& getName();

	void setRefTree(const QString& _tree);
	const QString& getRefTree();

	void setValue(const QString& _value);
	const QList<QString>& getValue();
	QString getJoinValue();

	void setRefCount(int _count);
	int getRefCount();
	int getSumRefCount();

	const QList<MemoryRefrenceInfo*>& getChildren();
	void addChild(MemoryRefrenceInfo* info);

	MemoryRefrenceInfo* getParent();
	void setParent(MemoryRefrenceInfo* info);

	MemoryRefrenceInfo* getRoot();

	void sort(bool bUpper = false);

	friend operator < (MemoryRefrenceInfo& info1, MemoryRefrenceInfo& info2) {
		return info1.getSumRefCount() < info2.getSumRefCount();
	};

protected:
	QString m_type;	// Table/Function/String
	QString m_name;	// Name
	QString m_tree;	// Refrence Path

	QList<QString> m_value;	//value可能不只一个

	int m_refCount;

	MemoryRefrenceInfo* m_parent;
	QList<MemoryRefrenceInfo*> m_children;
};

namespace std {
template<> struct hash<QString> {
	std::size_t operator () (const QString& str) const {
		return qHash(str);
	}
};
}

class MemoryRefrenceInfoMgr : public QThread
{
	Q_OBJECT
public:
	MemoryRefrenceInfoMgr();
	~MemoryRefrenceInfoMgr();

//	static MemoryRefrenceInfoMgr* getInstance();
//	static void destroyInstance();

	//解析文件，参数infoMgrBase表示对比文件，如果解析结果在infoMgrBase中已经存在，则跳过
	void parsFile(const QString& filepath, MemoryRefrenceInfoMgr* infoMgrBase = nullptr);
	MemoryRefrenceInfo* get(const QString& name, const QString& refTree, bool bCreate = false);
	MemoryRefrenceInfo* getByValue(const QString& name, const QString& value);
	void clear();
	QList<MemoryRefrenceInfo*> getAllRootInfos();
	void sortAll(QList<MemoryRefrenceInfo*>& infos, bool bUpper = false);
	void setRunning(bool bRun);
	void start(Priority = InheritPriority);

signals:
	void parseRunning(const QString& filepath, int usetime, int progress);
	void parseFinished(const QString& filepath);

protected:
	void parse(const QString& strInfo, MemoryRefrenceInfoMgr* infoMgrBase = nullptr);
	void add(MemoryRefrenceInfo* info);
	void remove(MemoryRefrenceInfo* info);

	void run();

protected:
//	typedef QMultiMap<QString, MemoryRefrenceInfo*> CacheInfoMap;
	typedef std::unordered_multimap<QString, MemoryRefrenceInfo*> CacheInfoMap;
	CacheInfoMap m_cacheInfos;
//	static MemoryRefrenceInfoMgr* sm_instance;
	typedef CacheInfoMap::iterator CacheInfoIter;
	QString m_filePath;
	MemoryRefrenceInfoMgr* m_infoMgrBase;

	bool m_bRunning;
};

#endif // MEMORYREFRENCEINFO_H
