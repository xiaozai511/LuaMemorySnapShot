# LuaMemorySnapShot
Lua内存快照对比分析工具
<br><br>
# About
近期正好有分析Lua内存的需求，找到了其他大神的内存快照工具，但是导出来的txt文件查看很不方便，所以就动手做了一个QT版本的可视化工具。
<br>
当前在处理大文件时，解析比较耗时，跟踪发现耗时基本都是引用关系链的处理上，待后续有时间再优化吧！
<br>
【快照工具原地址：yaukeywang/LuaMemorySnapshotDump】(https://github.com/yaukeywang/LuaMemorySnapshotDump)
<br><br>
# Usage
## 导出快照文件
1.将memory/MemoryReferenceInfo.lua集成到工程中
<br>
2.参考memory/MemorySnapShot.lua中MemorySnapShot函数的用法，导出内存快照
<br><br>
## 分析快照
1.使用QT5.9.2配置编译
<br>
2.使用示例
<br>
[](https://github.com/xiaozai511/LuaMemorySnapShot/blob/master/example/refrence.jpg)
<br>
![111](https://github.com/xiaozai511/LuaMemorySnapShot/blob/master/example/compare.jpg)
<br>
![111](https://github.com/xiaozai511/LuaMemorySnapShot/blob/master/example/filter.jpg)
