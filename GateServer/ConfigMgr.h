#pragma once
#include <fstream>  
#include <boost/property_tree/ptree.hpp>  //属性树
#include <boost/property_tree/ini_parser.hpp>//INI文件解析器
#include <boost/filesystem.hpp> //跨平台文件系统操作
#include <map>
#include <iostream>

class SectionInfo {//单个section的信息(Port = 8080)
public:
	SectionInfo() = default;
	~SectionInfo();
	SectionInfo(const SectionInfo& src);
	SectionInfo& operator = (const SectionInfo& src);
	std::string operator[](const std::string& key);
	
	std::map<std::string, std::string> _section_datas;
};

class ConfigMgr {//整个配置文件管理类器
public:
	~ConfigMgr();
	ConfigMgr(const ConfigMgr&) = delete;
	ConfigMgr& operator = (const ConfigMgr& src) = delete;
	static ConfigMgr& Inst();
	SectionInfo operator[](const std::string& section);

private:
	ConfigMgr();
	std::map<std::string, SectionInfo> _config_map;
};