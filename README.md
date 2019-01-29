# LuaMemorySnapShot
Lua内存快照对比分析工具
<br><br>
# About
近期正好有分析Lua内存的需求，找到了其他大神的内存快照工具，但是导出来的txt文件查看很不方便，所以就动手做了一个QT版本的可视化工具。可以直观的查看、对比、过滤所有变量及其引用关系等。
<br>
当前在处理大文件时，解析比较耗时，跟踪发现耗时基本都是引用关系链的处理上，待后续有时间再优化吧！
<br>
【快照工具原地址】(https://github.com/yaukeywang/LuaMemorySnapshotDump)
<br><br>
# Usage
## 导出快照文件
1.将memory/MemoryReferenceInfo.lua集成到工程中
<br>
2.参考memory/MemorySnapShot.lua中MemorySnapShot函数的用法，导出内存快照
<br><br>
## 分析快照
1.使用QT5.9.2编译
<br>
2.配置快照文件目录，或者直接查找到目录打开
<br>
3.使用示例
<br><br>
3.1 查看
<br>
![](https://github.com/xiaozai511/LuaMemorySnapShot/blob/master/example/refrence.jpg)
<br><br>
3.2 对比
<br>
![](https://github.com/xiaozai511/LuaMemorySnapShot/blob/master/example/compare.jpg)
<br><br>
3.3 过滤
<br>
![](https://github.com/xiaozai511/LuaMemorySnapShot/blob/master/example/filter.jpg)
