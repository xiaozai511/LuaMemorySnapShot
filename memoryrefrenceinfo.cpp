#include "memoryrefrenceinfo.h"
#include "common.h"
#include <QTime>
#include <QStringList>

QByteArray MemoryRefrenceInfo::TYPE_TABLE("table:");
QByteArray MemoryRefrenceInfo::TYPE_FUNCTION("function:");
QByteArray MemoryRefrenceInfo::TYPE_STRING("string:");
QByteArray MemoryRefrenceInfo::TYPE_USERDATA("userdata:");
QByteArray MemoryRefrenceInfo::TYPE_THREAD("thread:");

MemoryRefrenceInfo::MemoryRefrenceInfo() :
	m_type("")
  , m_name("")
  , m_tree("")
//  , m_value("")
  , m_refCount(0)
  , m_parent(nullptr)
{
}

MemoryRefrenceInfo::~MemoryRefrenceInfo()
{
}

const QList<MemoryRefrenceInfo*>& MemoryRefrenceInfo::getChildren()
{
	return m_children;
}

void MemoryRefrenceInfo::addChild(MemoryRefrenceInfo* info)
{
	if(info && !m_children.contains(info)) {
		m_children.append(info);
		info->setParent(this);
	}
}

MemoryRefrenceInfo* MemoryRefrenceInfo::getParent()
{
	return m_parent;
}

void MemoryRefrenceInfo::setParent(MemoryRefrenceInfo* info)
{
	if(info) {
		m_parent = info;
	}
}

void MemoryRefrenceInfo::setType(const QString& _type)
{
	if(_type == MemoryRefrenceInfo::TYPE_FUNCTION) {
		m_type = "Function";
	} else if(_type == MemoryRefrenceInfo::TYPE_TABLE) {
		m_type = "Table";
	} else if(_type == MemoryRefrenceInfo::TYPE_STRING) {
		m_type = "String";
	} else if(_type == MemoryRefrenceInfo::TYPE_USERDATA) {
		m_type = "UserData";
	} else if(_type == MemoryRefrenceInfo::TYPE_THREAD) {
		m_type = "Thread";
	} else {
		qDebug() << "Error Type!" << _type;
	}
}

const QString& MemoryRefrenceInfo::getType()
{
	return m_type;
}

void MemoryRefrenceInfo::setName(const QString& _name)
{
	m_name = _name;
}

const QString& MemoryRefrenceInfo::getName()
{
	return m_name;
}

void MemoryRefrenceInfo::setRefTree(const QString& _tree)
{
	m_tree = _tree;
}

const QString& MemoryRefrenceInfo::getRefTree()
{
	return m_tree;
}

void MemoryRefrenceInfo::setValue(const QString& _value)
{
//	m_value = _value;
	if(!m_value.contains(_value)) {
		m_value.append(_value);
	}
}

const QList<QString>& MemoryRefrenceInfo::getValue()
{
	return m_value;
}

QString MemoryRefrenceInfo::getJoinValue()
{
//	return m_value;
	return m_value.join(",");
}

void MemoryRefrenceInfo::setRefCount(int _count)
{
	m_refCount = _count;
}

int MemoryRefrenceInfo::getRefCount()
{
	return m_refCount;
}

int MemoryRefrenceInfo::getSumRefCount()
{
	int count = m_refCount;

	for(auto child : m_children) {
		count += child->getSumRefCount();
	}

	return count;
}

MemoryRefrenceInfo* MemoryRefrenceInfo::getRoot()
{
	return m_parent ? m_parent->getRoot() : this;
}

void MemoryRefrenceInfo::sort(bool bUpper)
{
	if(m_children.empty()) {
		return;
	}

	static auto lessThan = [](MemoryRefrenceInfo* info1, MemoryRefrenceInfo* info2)->bool {
		return info1->getSumRefCount() < info2->getSumRefCount();
	};

	static auto greaterThan = [](MemoryRefrenceInfo* info1, MemoryRefrenceInfo* info2)->bool {
		return info1->getSumRefCount() > info2->getSumRefCount();
	};

	auto comFunc = bUpper ? lessThan : greaterThan;

	qSort(m_children.begin(), m_children.end(), comFunc);

	for(auto child : m_children) {
		child->sort(bUpper);
	}
}

//-----------------------------------------------------------------------

//MemoryRefrenceInfoMgr* MemoryRefrenceInfoMgr::sm_instance = nullptr;

MemoryRefrenceInfoMgr::MemoryRefrenceInfoMgr() :
	  m_filePath("")
	, m_infoMgrBase(nullptr)
	, m_bRunning(false)
{

}

MemoryRefrenceInfoMgr::~MemoryRefrenceInfoMgr()
{
	for(auto iter = m_cacheInfos.begin(); iter != m_cacheInfos.end(); ++iter) {
		if(iter->second) {
			delete iter->second;
		}
	}

	m_cacheInfos.clear();
}

void MemoryRefrenceInfoMgr::parse(const QString& strInfo, MemoryRefrenceInfoMgr* infoMgrBase)
{
	auto time1 = QTime::currentTime();
	//split string
	//QStringList infos = strInfo.split(QRegExp("[ \t]"), QString::KeepEmptyParts);
	QStringList infos;
	QString tmpStr;
	bool bQuot = false;
	for(int i = 0; i < strInfo.length(); ++i) {
		QChar ch = strInfo.at(i);
		if(ch == "\"") {
			bQuot = !bQuot;
			tmpStr.append(ch);
		} else if(!bQuot && (ch == " " || ch == "\t")) {
			if(tmpStr.length() > 0) {
				infos.push_back(tmpStr);
			}
			tmpStr.clear();
		} else {
			tmpStr.append(ch);
		}
	}
	if(tmpStr.length() > 0) {
		infos.push_back(tmpStr);
	}
	auto time2 = QTime::currentTime();

	if(infos.empty() || infos.size() < 4 || infos.size() > 5) {
		return;
	}

	QString strType = infos[0];
	QString strValue = infos[1];

	//解析函数、变量名、引用树
	QString strRefTree;
	if(infos[0] == MemoryRefrenceInfo::TYPE_FUNCTION) {
		strRefTree = infos[2];
		QString strDef;
		int index = strRefTree.indexOf("[line:");
		if(index != -1) {
			strDef = strRefTree.right(strRefTree.length() - index);
			strRefTree = strRefTree.left(index);
		}

		if(infos.size() == 5) {	//去掉文件定义段，如[line:88@file:[string \"scripts/engine/baseclass.lua\"]]
			strDef.append(infos[3]);
			infos.removeAt(3);
		}

		strValue.append(strDef);
	} else if(infos[0] == MemoryRefrenceInfo::TYPE_TABLE) {
		strRefTree = infos[2];
	} else if(infos[0] == MemoryRefrenceInfo::TYPE_STRING) {
		strRefTree = infos[2];
		strRefTree = strRefTree.section("[string]", 0, 0);
	} else if(infos[0] == MemoryRefrenceInfo::TYPE_USERDATA) {
		strRefTree = infos[2];
	} else if(infos[0] == MemoryRefrenceInfo::TYPE_THREAD) {
		strRefTree = infos[2];
	} else {
		qDebug() << "Type error, must be \"Table/Function/String\"!" << strInfo;
		return;
	}

	if(!strRefTree.startsWith("registry")) {
		//字符串内有空格时，解析有问题，暂时return掉，后面再修复
		return;
	}

	//register
	static auto _getInfoName = [](QString& strRefTree, QString& strName) {
		int index = strRefTree.lastIndexOf(".");
		if(index == -1) {
			strName = strRefTree;
			strRefTree.clear();
		} else {
			strName = strRefTree.right(strRefTree.length() - index - 1);
			strRefTree = strRefTree.left(index);
		}
	};

	auto _getAndCreateInfo = [=](QString& strRefTree, const QString& strValue = "", MemoryRefrenceInfoMgr* infoMgrBase = nullptr)->MemoryRefrenceInfo* {
		QString strName;
		_getInfoName(strRefTree, strName);
		if(infoMgrBase && infoMgrBase->getByValue(strName, strValue)) {
			return nullptr;
		} else {
			return get(strName, strRefTree, true);
		}

	};

	//create cur info
	MemoryRefrenceInfo* info = _getAndCreateInfo(strRefTree, strValue, infoMgrBase);
	auto time3 = QTime::currentTime();
	if(!info) {
		return;
	}
	info->setType(strType);
	info->setValue(strValue);
	info->setRefCount(infos[3].toInt() + info->getRefCount());

	//register parent
	MemoryRefrenceInfo* pChild = info;
	while(strRefTree.length() > 0) {
		MemoryRefrenceInfo* pParent =  _getAndCreateInfo(strRefTree);
		if(pParent) {
			pParent->addChild(pChild);
			pChild = pParent;
		}
	}
	auto time4 = QTime::currentTime();

	qDebug() << m_cacheInfos.size()
			 << time1.msecsTo(time2)
			 << time2.msecsTo(time3)
			 << time3.msecsTo(time4);
}

void MemoryRefrenceInfoMgr::parsFile(const QString& filepath, MemoryRefrenceInfoMgr* infoMgrBase)
{
	m_filePath = filepath;
	m_infoMgrBase = infoMgrBase;
	m_cacheInfos.clear();
	this->start();
}

MemoryRefrenceInfo* MemoryRefrenceInfoMgr::get(const QString& name, const QString& refTree, bool bCreate)
{
	MemoryRefrenceInfo* info = nullptr;
	auto range = m_cacheInfos.equal_range(name);
	for(auto iter = range.first; iter != range.second; ++iter) {
		if(iter->second->getRefTree() == refTree) {
			info = iter->second;
			break;
		}
	}

	if(!info && bCreate) {
		info = new MemoryRefrenceInfo;
		info->setName(name);
		info->setRefTree(refTree);
		add(info);
	}

	return info;
}

MemoryRefrenceInfo* MemoryRefrenceInfoMgr::getByValue(const QString& name, const QString& value)
{
	MemoryRefrenceInfo* info = nullptr;
	auto range = m_cacheInfos.equal_range(name);
	for(auto iter = range.first; iter != range.second; ++iter) {
		const QList<QString>& values = iter->second->getValue();
		if((value.length() == 0 && values.empty()) || values.contains(value)) {
			info = iter->second;
			break;
		}
	}

	return info;
}

void MemoryRefrenceInfoMgr::add(MemoryRefrenceInfo* info)
{
	if(info) {
//		m_cacheInfos.insert(info->getName(), info);
		m_cacheInfos.insert(std::make_pair(info->getName(), info));
	}
}

void MemoryRefrenceInfoMgr::clear()
{
	m_cacheInfos.clear();
}

void MemoryRefrenceInfoMgr::remove(MemoryRefrenceInfo* info)
{
	if(info) {
		//m_cacheInfos.remove(info->getName());
		for(auto iter = m_cacheInfos.begin(); iter != m_cacheInfos.end(); ++iter) {
			if(iter->second == info) {
				m_cacheInfos.erase(iter);
				break;
			}
		}
	}
}

void MemoryRefrenceInfoMgr::sortAll(QList<MemoryRefrenceInfo*>& infos, bool bUpper)
{
	static auto lessThan = [](MemoryRefrenceInfo* info1, MemoryRefrenceInfo* info2)->bool {
		return info1->getSumRefCount() < info2->getSumRefCount();
	};

	static auto greaterThan = [](MemoryRefrenceInfo* info1, MemoryRefrenceInfo* info2)->bool {
		return info1->getSumRefCount() > info2->getSumRefCount();
	};

	auto comFunc = bUpper ? lessThan : greaterThan;
	qSort(infos.begin(), infos.end(), comFunc);

	for(auto info : infos) {
		info->sort(bUpper);
	}
}

QList<MemoryRefrenceInfo*> MemoryRefrenceInfoMgr::getAllRootInfos()
{
	QList<MemoryRefrenceInfo*> rootInfos;
	for(auto iter = m_cacheInfos.begin(); iter != m_cacheInfos.end(); ++iter) {
		MemoryRefrenceInfo* root = iter->second->getRoot();
		if(!rootInfos.contains(root)) {
			rootInfos.append(root);
		}
	}

	return rootInfos;
}

void MemoryRefrenceInfoMgr::start(Priority priority)
{
	m_bRunning = true;
	QThread::start(priority);
}

void MemoryRefrenceInfoMgr::setRunning(bool bRun)
{
	m_bRunning = bRun;
}

void MemoryRefrenceInfoMgr::run()
{
#if 1
	//test
	this->parse("function: 000000A4E124E570	registry.2[_G].RankListModule.[metatable].__newindex.[ups:table:vtbl].Instance.delete_me[line:88@file:[string \"scripts/engine/baseclass.lua\"]]	2780");
	this->parse("function: 00007FFA73E43530	registry.2[_G].StringUtils.split.[ups:function:strSub][line:-1@file:[C]]	7");
	this->parse("string: \"equip_ icon_10007\"	registry.2[_G].ItemConfig.data.913207.Icon[string]	6");
	this->parse("table: 000000A4F47047B0	registry.2[_G].WorldEnterController.__init.[ups:table:MOVE_TYPE]	6");
#else
	auto _timeBegin = QTime::currentTime();
	FILE* pFile = fopen(m_filePath.toUtf8(), "r");
	if(pFile) {
		fseek(pFile, 0, SEEK_END);
		long fileSize = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);
		static int maxLen = 1024 * 10;
		char line[maxLen] = {};
		while (m_bRunning && fgets(line, maxLen, pFile)) {
			this->parse(line, m_infoMgrBase);
			int progress = ftell(pFile) * 100 / fileSize;
			emit parseRunning(m_filePath,
							  _timeBegin.msecsTo(QTime::currentTime()),
							  progress);
		}
		fclose(pFile);
	}
#endif

	//中止时不发事件
	if (m_bRunning) {
		emit parseFinished(m_filePath);
	}

	//解析单次有效，解析完置空
	m_filePath.clear();
	m_infoMgrBase = nullptr;
}
